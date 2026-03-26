# Warning: Never Use Tick in Action Subclasses

**Severity**: CRITICAL
**Author**: Performance team
**Date**: 2025-01-10
**Tags**: action-system, performance, tick

## The Rule

Do NOT override `Tick()` or enable ticking in any `USAction` subclass. Actions are managed objects, not actors — they don't have an inherent tick. Manually adding tick behavior creates performance problems and fights the Action System's design.

## Why This Is Critical

1. **Performance**: Each ticking action adds per-frame cost. With 10 abilities on 20 characters, that's 200 tick calls per frame. At 60fps, that's 12,000 unnecessary function calls per second.

2. **Design Violation**: The Action System uses Start/Stop semantics with duration timers. Tick-based logic breaks this model and creates state management bugs.

3. **Network**: Ticked state changes don't replicate correctly through the Action System's replication path. You'll get desync in multiplayer.

## What to Do Instead

### For periodic checks:
```cpp
// CORRECT: Use a timer
GetWorld()->GetTimerManager().SetTimer(
    TimerHandle, this, &USAction_YourAbility::OnTimerTick,
    CheckInterval, true // looping
);

// INCORRECT: Tick override
void USAction_YourAbility::Tick(float DeltaTime) { ... }
```

### For duration-based effects:
```cpp
// CORRECT: Use the Action System's duration
// Set Duration property, override StopAction for cleanup

// INCORRECT: Counting time in Tick
void USAction_MyBuff::Tick(float DeltaTime)
{
    ElapsedTime += DeltaTime;
    if (ElapsedTime >= Duration) StopAction(GetOwningComponent()->GetOwner());
}
```

### For continuous movement (like dash):
```cpp
// CORRECT: Use a timeline or root motion
// Set up FTimeline in StartAction, drive movement from curve

// INCORRECT: Moving character in Tick
void USAction_Dash::Tick(float DeltaTime)
{
    Owner->AddMovementInput(Direction, Speed * DeltaTime);
}
```

## How Future Agents Should Use This

If you're tempted to add Tick to an action:
1. STOP — find the timer or duration alternative
2. Check patterns/create-new-action.md for the correct approach
3. If you truly need per-frame updates, use FTimeline (it hooks into the actor's tick, not the action's)
