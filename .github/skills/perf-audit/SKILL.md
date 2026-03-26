---
name: perf-audit
description: Scan C++ code for UE5-specific performance anti-patterns. Identifies Tick abuse, unnecessary garbage collection pressure, allocation hotspots, and common frame-rate killers in game code. Use during code review or when investigating performance issues. WHEN: performance review, optimize code, frame rate drops, Tick overhead, GC pressure, memory allocation, CPU profiling, GPU bottleneck, performance anti-patterns, hot path analysis, game performance.
---

# UE5 Performance Audit Skill

Scans C++ game code for performance anti-patterns specific to Unreal Engine 5. Every millisecond matters at 60fps (16.6ms budget) or 120fps (8.3ms budget). This skill catches the patterns that silently eat your frame budget.

## When to Use

- During code review of gameplay code
- After implementing a new system or feature
- When investigating frame rate drops or hitches
- Before a milestone/certification build
- When the autonomous agent generates code that needs performance validation

## The Frame Budget Reality

At 60fps, you have 16.6ms per frame for EVERYTHING:
- Engine tick: ~2ms
- Physics: ~2-4ms
- Rendering: ~6-8ms
- Gameplay/AI: ~2-4ms budget
- Audio, networking, etc: ~1-2ms

Your gameplay code gets 2-4ms. Every anti-pattern in this list steals from that budget.

## Anti-Patterns to Detect

### Category 1: Tick Abuse (Most Common in Game Code)

**Pattern: Unnecessary Tick**
```cpp
// BAD: Polling every frame for something that changes rarely
void AMyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (Health != LastHealth) { UpdateHealthBar(); }
}
```
**Fix**: Use delegates/events instead of polling. Bind to `OnHealthChanged` delegate.
**Impact**: Each ticking actor adds ~0.01-0.05ms. 200 ticking actors = 2-10ms wasted.

**Pattern: Tick Not Disabled When Idle**
```cpp
// BAD: Actor ticks even when nothing is happening
AMyActor::AMyActor()
{
    PrimaryActorTick.bCanEverTick = true;
    // Never calls SetActorTickEnabled(false)
}
```
**Fix**: Start with tick disabled. Enable only when needed. Disable when idle.

**Pattern: Heavy Work in Tick**
```cpp
// BAD: Line traces or overlaps every frame
void AMyActor::Tick(float DeltaTime)
{
    TArray<FHitResult> Hits;
    GetWorld()->SweepMultiByChannel(Hits, Start, End, ...); // Every frame!
}
```
**Fix**: Use timers (`GetWorldTimerManager().SetTimer()`) to run expensive checks at lower frequency (every 0.1s instead of every frame).

### Category 2: Garbage Collection Pressure

**Pattern: Spawning UObjects in Hot Path**
```cpp
// BAD: Creating UObjects every frame
void AMyActor::Tick(float DeltaTime)
{
    UMyComponent* Temp = NewObject<UMyComponent>(this); // GC pressure!
    // ... use and abandon
}
```
**Fix**: Pool objects. Pre-allocate and reuse. Use structs (FStruct) instead of UObjects where possible.
**Impact**: Each GC cycle can cause 1-10ms hitches. More UObjects = more frequent GC.

**Pattern: Missing UPROPERTY on UObject Pointers**
```cpp
// BAD: GC can't see this pointer - potential dangling reference AND not tracked for GC scheduling
UMyObject* CachedObject; // No UPROPERTY()!
```
**Fix**: Always use `UPROPERTY()` on UObject pointers. Use `TWeakObjectPtr` for non-owning references.

**Pattern: TArray Reallocation in Loops**
```cpp
// BAD: Array grows inside tight loop
TArray<FVector> Points;
for (int i = 0; i < 1000; i++)
{
    Points.Add(CalculatePoint(i)); // Reallocates multiple times
}
```
**Fix**: `Points.Reserve(1000)` before the loop. Or use `TArray::SetNum()`.

### Category 3: String and Container Operations

**Pattern: FString Concatenation in Hot Path**
```cpp
// BAD: Creates temporary FString objects
FString Result = FString::Printf(TEXT("Player %s scored %d"), *Name, Score);
UE_LOG(LogGame, Log, TEXT("%s"), *Result); // Every frame?
```
**Fix**: Use `FStringBuilderBase` for hot paths. Guard log statements with `UE_LOG` verbosity checks or `#if !UE_BUILD_SHIPPING`.

**Pattern: FName Creation from String at Runtime**
```cpp
// BAD: FName::FName(const TCHAR*) does a hash table lookup
void AMyActor::Tick(float DeltaTime)
{
    FName Tag = FName(TEXT("MyTag")); // Hash lookup every frame!
    if (ActorHasTag(Tag)) { ... }
}
```
**Fix**: Store FName as a static or member variable. `static const FName MyTag(TEXT("MyTag"));`

### Category 4: Blueprint/Native Interface Issues

**Pattern: BlueprintImplementableEvent Called Every Frame**
```cpp
// BAD: Crossing the Blueprint/C++ boundary every frame is expensive
void AMyActor::Tick(float DeltaTime)
{
    OnTickBlueprint(DeltaTime); // Virtual call + BP VM overhead
}
```
**Fix**: Use `BlueprintNativeEvent` with C++ implementation for hot paths. Only cross to BP for infrequent events.

### Category 5: Memory and Loading

**Pattern: LoadObject in Gameplay Code**
```cpp
// BAD: Synchronous asset load during gameplay
void AMyActor::BeginPlay()
{
    UTexture2D* Tex = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/MyTex"));
}
```
**Fix**: Use `TSoftObjectPtr` and async loading (`StreamableManager`). Or load in constructor with `ConstructorHelpers::FObjectFinder`.

**Pattern: Large TMap with Frequent Lookups**
```cpp
// BAD: TMap has pointer indirection, poor cache locality
TMap<FName, FMyData> BigMap; // 10,000 entries, looked up every frame
```
**Fix**: For hot-path lookups, consider `TArray` with binary search (better cache locality) or custom hash map.

### Category 6: Physics and Collision

**Pattern: Complex Collision Queries Every Frame**
```cpp
// BAD
TArray<FOverlapResult> Overlaps;
GetWorld()->OverlapMultiByChannel(Overlaps, Location, FQuat::Identity,
    ECC_Pawn, FCollisionShape::MakeSphere(5000.0f)); // 5000 unit radius!
```
**Fix**: Reduce query frequency, reduce radius, use collision channels to filter early, cache results.

**Pattern: Physics Simulation on Non-Essential Actors**
```cpp
// BAD: Full physics sim on decorative props
MeshComponent->SetSimulatePhysics(true); // On 500 decoration meshes
```
**Fix**: Use physics only where gameplay requires it. Set decorations to `ECollisionEnabled::QueryOnly` or `NoCollision`.

## Output Format

```
## Performance Audit: [File or Feature Name]

### Summary
- Anti-patterns found: 7
- Critical (frame budget impact): 2
- Warnings (potential issues): 3
- Info (minor optimizations): 2
- Estimated frame cost: ~1.2ms at 200 active actors

### Critical Issues

#### [CRITICAL] Tick running on 200+ actors without throttling
- **File**: AEnemyCharacter.cpp:45
- **Pattern**: Heavy Tick with line trace
- **Frame Cost**: ~0.05ms per actor x 200 = 10ms (OVER BUDGET)
- **Fix**: Move to timer-based check every 0.2s, or use EQS for batched queries
- **Priority**: Fix before next milestone

#### [CRITICAL] TArray reallocation in combat loop  
- **File**: SCombatSystem.cpp:120
- **Pattern**: Growing array in per-frame loop
- **Frame Cost**: ~0.3ms per frame during combat
- **Fix**: Reserve(MaxCombatants) at initialization

### Warnings
[similar format for lower severity items]

### Performance Scorecard
| Category | Score | Notes |
|----------|-------|-------|
| Tick Usage | C | 12 actors tick unnecessarily |
| GC Pressure | B | 2 missing UPROPERTY markers |
| Allocations | B- | 1 hot-path allocation pattern |
| String Ops | A | Clean string usage |
| Physics | B+ | Minor collision query optimization possible |
| Asset Loading | A | Proper async loading used |
```

## Integration with Workshop

- **Segment 4**: Primary demo alongside @profiler built-in agent
- **Segment 3**: Can be invoked after agent implements a fix to verify performance
- **Pain Point**: Performance matters in games - every ms counts at 60fps

Value proposition: "Your senior C++ engineer probably catches these in code review. But they review maybe 20% of commits. This skill reviews 100% of changes for performance anti-patterns. And it explains WHY each pattern is bad, with the actual frame cost."
