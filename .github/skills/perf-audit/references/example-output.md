# Example Performance Audit Output

This is a realistic example of the perf-audit skill analyzing a UE5 gameplay file.

---

## Performance Audit: AEnemyPatrolGuard.cpp

### Summary
- Anti-patterns found: 4
- Critical (frame budget impact): 2
- Warnings (potential issues): 1
- Info (minor optimizations): 1
- Estimated frame cost: ~3.8ms at 150 active patrol guards

### Critical Issues

#### [CRITICAL] SweepMultiByChannel in Tick — Line-of-sight check every frame
- **File**: AEnemyPatrolGuard.cpp:87
- **Pattern**: Heavy Work in Tick
- **Code**:
  ```cpp
  void AEnemyPatrolGuard::Tick(float DeltaTime)
  {
      Super::Tick(DeltaTime);
      TArray<FHitResult> Hits;
      FVector Start = GetActorLocation();
      FVector End = Start + GetActorForwardVector() * DetectionRange;
      GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity,
          ECC_Pawn, FCollisionShape::MakeSphere(200.0f));
      for (auto& Hit : Hits)
      {
          if (AAOSCharacter* Player = Cast<AAOSCharacter>(Hit.GetActor()))
          {
              OnPlayerDetected(Player);
          }
      }
  }
  ```
- **Frame Cost**: ~0.02ms per guard × 150 guards = **3.0ms** (OVER BUDGET for gameplay)
- **Fix**: Move detection to a timer running every 0.15s. Use `GetWorldTimerManager().SetTimer(DetectionTimerHandle, this, &AEnemyPatrolGuard::RunDetectionSweep, 0.15f, true)`. This reduces cost from 3.0ms/frame to ~0.5ms amortized. Alternatively, use UE5's AI Perception component which batches queries internally.
- **Priority**: Fix before next milestone

#### [CRITICAL] TArray allocation inside detection loop with no Reserve
- **File**: AEnemyPatrolGuard.cpp:89
- **Pattern**: TArray Reallocation in Loops
- **Code**:
  ```cpp
  TArray<FHitResult> Hits; // Allocated on stack, but grows dynamically
  ```
- **Frame Cost**: ~0.005ms per reallocation × multiple per frame = **0.75ms** aggregate across all guards
- **Fix**: Declare `TArray<FHitResult> CachedHits` as a member variable and reuse it with `CachedHits.Reset()` instead of creating a new array each tick. This avoids heap allocation every frame.
- **Priority**: Fix alongside the timer refactor above

### Warnings

#### [WARNING] FName constructed from string literal every frame
- **File**: AEnemyPatrolGuard.cpp:112
- **Pattern**: FName Creation from String at Runtime
- **Code**:
  ```cpp
  void AEnemyPatrolGuard::Tick(float DeltaTime)
  {
      // ... after detection sweep ...
      if (ActorHasTag(FName(TEXT("Alerted"))))
      {
          IncreasePatrolSpeed();
      }
  }
  ```
- **Frame Cost**: ~0.001ms per guard (FName hash lookup) — low individually but unnecessary
- **Fix**: Declare `static const FName AlertedTag(TEXT("Alerted"));` at file scope or as a class static member, then use `ActorHasTag(AlertedTag)`.

### Info

#### [INFO] Tick enabled in constructor but never conditionally disabled
- **File**: AEnemyPatrolGuard.cpp:22
- **Pattern**: Tick Not Disabled When Idle
- **Code**:
  ```cpp
  AEnemyPatrolGuard::AEnemyPatrolGuard()
  {
      PrimaryActorTick.bCanEverTick = true;
  }
  ```
- **Recommendation**: Guards that are far from the player or in unloaded sublevels still tick. Consider disabling tick when the guard is beyond a relevancy distance (`SetActorTickEnabled(false)` when distance to player > 5000 units). Re-enable when the player enters range via an overlap volume or significance manager.

### Performance Scorecard

| Category | Score | Notes |
|----------|-------|-------|
| Tick Usage | D | Heavy sweep every frame on 150+ actors |
| GC Pressure | A | No UObject allocations in hot path |
| Allocations | C | TArray allocated per-frame in Tick |
| String Ops | B+ | One FName from literal in Tick |
| Physics | C | Sweep query too frequent, radius acceptable |
| Asset Loading | A | No runtime asset loading detected |

### Recommended Fix Order
1. **Move sweep to timer** (saves ~2.5ms/frame) — 15 min fix
2. **Cache TArray as member** (saves ~0.75ms/frame) — 5 min fix
3. **Static FName** (saves ~0.15ms/frame) — 2 min fix
4. **Distance-based tick disable** (saves variable, depends on level layout) — 30 min fix

**Total estimated savings: ~3.4ms/frame** — reclaims nearly the entire gameplay budget.
