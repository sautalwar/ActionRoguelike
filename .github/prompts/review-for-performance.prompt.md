---
description: "Review code changes for performance impact in a game context"
---

# Performance Review for Game Code

Use this prompt to review code changes for performance issues that matter in a real-time game running at 30–60 FPS. Every millisecond in a frame budget counts.

---

## 1. Per-Frame Allocations

**What to look for:**
- `new` / `NewObject` / `MakeShared` inside `Tick`, `ReceiveDamage`, or any function called every frame
- `TArray` or `TMap` construction inside hot loops (prefer pre-allocated or member-variable containers)
- `FString::Printf` or string concatenation in per-frame code (allocates on the heap)
- Lambdas that capture by value and copy large structs

**What to flag:**
```
❌ BAD:  TArray<FHitResult> Results; (created on stack every tick, causes realloc on growth)
✅ GOOD: Member variable TArray<FHitResult> CachedHitResults; (reuse with Reset())
```

**Guidance:** Move allocations to `BeginPlay` or class members. Use `Reset()` instead of re-creating containers. Use `Reserve()` if the size is predictable.

---

## 2. Unnecessary Copies

**What to look for:**
- Function parameters taking `TArray`, `FString`, `TMap`, or `UStruct` by value instead of `const&`
- Range-for loops over containers without `const auto&`
- Returning large structs by value from frequently called functions
- `TArray::Add()` vs `TArray::Emplace()` — prefer Emplace to avoid temporaries

**What to flag:**
```
❌ BAD:  void ProcessTargets(TArray<AActor*> Targets)
✅ GOOD: void ProcessTargets(const TArray<AActor*>& Targets)
```

**Guidance:** Pass by `const&` unless the function needs ownership. Use `MoveTemp()` for transfers. Watch for implicit copies in delegate bindings.

---

## 3. Tick Function Overhead

**What to look for:**
- Components with `bCanEverTick = true` that don't need to tick every frame
- Heavy logic in `TickComponent` that could use timers or event-driven updates
- Multiple actors polling for state that could be pushed via delegates
- Missing early-out checks at the top of Tick functions

**What to flag:**
```
❌ BAD:  Tick checks distance to player every frame for 500 actors
✅ GOOD: Use a timer at 0.25s intervals, or spatial query with significance manager
```

**Guidance:** Disable tick when idle (`SetComponentTickEnabled(false)`). Use `FTimerManager` for periodic checks. Consider the Significance Manager for distance-based LOD on logic.

---

## 4. Delegate Binding in Hot Paths

**What to look for:**
- `AddDynamic` / `BindUFunction` called every frame or every hit
- Delegates bound but never unbound (memory leak, stale references)
- Multicast delegates with many subscribers being broadcast in tight loops
- Dynamic delegates where static delegates would suffice (reflection overhead)

**What to flag:**
```
❌ BAD:  OnHit.AddDynamic(this, &AMyActor::HandleHit); // in Tick
✅ GOOD: Bind once in BeginPlay, unbind in EndPlay
```

**Guidance:** Bind delegates once during initialization. Prefer static (non-dynamic) delegates for C++-only callbacks — they avoid the reflection system overhead.

---

## 5. Object Pools

**What to look for:**
- Projectiles, VFX, decals, or other short-lived objects being spawned and destroyed frequently
- `SpawnActor` / `DestroyActor` calls in combat-heavy code paths
- Missing pool return logic (objects spawned but never recycled)

**What to flag:**
```
❌ BAD:  SpawnActor<AProjectile>() on every fire, DestroyActor on impact
✅ GOOD: Acquire from pool, deactivate and return on impact
```

**Guidance:** Use an object pool for anything spawned more than a few times per second. Deactivate (`SetActorHiddenInGame`, `SetActorTickEnabled(false)`) instead of destroying.

---

## 6. Cache-Friendly Data Layouts

**What to look for:**
- Arrays of pointers to scattered heap objects when contiguous data would work
- Struct-of-Arrays vs Array-of-Structs mismatches for the access pattern
- Iterating over large actor arrays when only a few fields are needed
- `TMap` lookups in hot loops where a sorted `TArray` with binary search would be faster

**What to flag:**
```
❌ BAD:  TArray<UMyComponent*> — pointer chasing through heap
✅ GOOD: TArray<FMyComponentData> — contiguous struct data for iteration
```

**Guidance:** Keep hot data contiguous. Separate cold fields into a secondary struct. Profile with `stat startfile` / `stat stopfile` and Unreal Insights to verify.

---

## Review Output Format

For each issue found, report:

| Field | Value |
|---|---|
| **File** | Path to the file |
| **Line(s)** | Line number or range |
| **Category** | One of the 6 categories above |
| **Severity** | 🔴 Critical / 🟡 Warning / 🟢 Info |
| **Current Code** | The problematic pattern |
| **Suggested Fix** | The recommended change |
| **Rationale** | Why this matters (frame budget impact estimate if possible) |

**Severity guide:**
- 🔴 **Critical**: Will cause frame drops or hitches in normal gameplay
- 🟡 **Warning**: Suboptimal but tolerable at current scale; will become critical as content grows
- 🟢 **Info**: Best practice suggestion, low immediate impact

End the review with a summary: total issues by severity, overall performance risk assessment (Low / Medium / High), and recommended priority order for fixes.
