# Dependency: Action System -> Character Movement

**Type**: Runtime dependency
**Direction**: Actions depend on Character Movement Component
**Tags**: action-system, movement, character

## The Dependency

Several action subclasses (Dash, Sprint, Dodge) directly interact with the character's movement component. They cast to `USCharacterMovementComponent` (the project's custom subclass, NOT the base UE5 `UCharacterMovementComponent`).

## What This Means

- If you modify `USCharacterMovementComponent`, check all movement-related actions
- If you add a new movement ability, use `Cast<USCharacterMovementComponent>` not `GetCharacterMovement()`
- The cast can return null if used on a non-character actor — always null-check

## Affected Files

- SActionDash.cpp -> uses movement component for sweep and velocity
- SActionSprint.cpp -> modifies movement speed
- SCharacter.cpp -> owns the custom movement component

## How Future Agents Should Use This

When touching movement code:
1. Search for all actions that cast to USCharacterMovementComponent
2. Verify your changes don't break the cast or the API they use
3. If you change the movement component's API, update all dependent actions
