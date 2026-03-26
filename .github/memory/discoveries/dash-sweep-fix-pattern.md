# Discovery: Dash Sweep Trace Fix Pattern

**Date**: 2026-03-26
**Confidence**: HIGH (verified by PR merge)
**Tags**: dash, collision, sweep-trace, wall-clipping, movement
**Source**: GAME-1 fix (PR #1)

## What We Learned

The Blueprint Dash action used `SetActorLocation()` with only an endpoint collision check.
This caused the character to teleport through thin walls (< 50 units) because the collision
capsule was never checked along the movement path.

## The Pattern

For any high-speed movement action (dash, blink, charge), always use:

1. **`SweepSingleByChannel`** with the character's capsule shape to trace the full path
2. **`SafeMoveUpdatedComponent`** for authoritative replicated movement
3. A **wall margin** (2+ units) to pull back from hit points and prevent interpenetration

## Code Reference

```cpp
// In PerformSweepDash() - RogueAction_Dash.cpp
const bool bHitSomething = GetWorld()->SweepSingleByChannel(
    SweepHit, StartLocation, DesiredEndLocation,
    FQuat::Identity, ECC_Pawn, CapsuleShape, QueryParams);

if (bHitSomething)
{
    FinalLocation = SweepHit.Location + (SweepHit.Normal * WallMargin);
}
```

## Impact on Other Bugs

GAME-2 (melee through walls) shares the same collision pattern — the fix approach
here (sweep + LOS check) should inform that fix too.
