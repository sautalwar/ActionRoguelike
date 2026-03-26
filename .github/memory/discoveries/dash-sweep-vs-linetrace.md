# Discovery: Character Abilities Must Use Capsule Sweep, Not Line Trace

**Discovered by**: GAME-1234-agent
**Date**: 2025-01-15
**Confidence**: HIGH (verified by test: TestDashAbility_CornerCollision)
**Tags**: action-system, collision, movement, character

## What Was Discovered

When implementing movement-based abilities (dash, teleport, charge), you MUST use a capsule sweep (`SweepSingleByChannel` with `FCollisionShape::MakeCapsule`) that matches the character's collision capsule dimensions. Line traces are insufficient because they don't account for the character's physical volume.

## The Problem

Line traces check a single point along a line. A character capsule is ~34cm radius and ~88cm half-height. Near wall corners, a line trace through the character's center can pass through the gap between two wall segments while the character's capsule would collide.

## The Pattern

```cpp
// CORRECT: Use capsule sweep matching character dimensions
FCollisionShape SweepShape = FCollisionShape::MakeCapsule(
    CapsuleComponent->GetScaledCapsuleRadius(),
    CapsuleComponent->GetScaledCapsuleHalfHeight()
);
FHitResult Hit;
bool bBlocked = GetWorld()->SweepSingleByChannel(
    Hit, Start, End, FQuat::Identity,
    ECC_Pawn, SweepShape
);

// INCORRECT: Line trace misses character volume
FHitResult Hit;
bool bBlocked = GetWorld()->LineTraceSingleByChannel(
    Hit, Start, End, ECC_Visibility
);
```

## Applicability

This applies to ANY ability that moves the character:
- Dash / Dodge roll
- Teleport / Blink
- Charge / Rush
- Grapple / Hook pull
- Knockback application

## Related

- GAME-1234: Original discovery (dash through walls)
- GAME-1100: Similar fix applied to melee range check (historical)
