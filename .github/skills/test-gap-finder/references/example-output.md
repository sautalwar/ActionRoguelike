# Example Test Gap Analysis Output

## Test Gap Analysis

### Summary
- Functions modified: 4
- Existing tests found: 2
- Coverage gaps identified: 5
- Test cases suggested: 7

### Coverage Map

| Function | File | Existing Test | Status |
|----------|------|--------------|--------|
| SActionDash::ExecuteAction | SActionDash.cpp | TestDashAbility.cpp | PARTIAL - doesn't test corner collision |
| SActionDash::CalculateSweep | SActionDash.cpp | None | GAP |
| SCharacter::CanActivateAction | SCharacter.cpp | TestCharacterActions.cpp | COVERED |
| SActionComponent::StartAction | SActionComponent.cpp | TestActionComponent.cpp | COVERED |

### Suggested Test Cases

#### [CRITICAL] Test: DashAbility_StopsAtCornerWall

```
Test Case: DashAbility_StopsAtCornerWall
Category: Functional
Priority: HIGH (directly tests the bug fix)
Setup:
  - Spawn character at position (100, 100, 0)
  - Place wall corner at (200, 200, 0) with 90-degree angle
  - Set dash distance to 300 units
Action:
  - Activate dash ability toward the corner
Expected:
  - Character position is NOT beyond the wall
  - Character position is at or near the wall surface
  - Dash ability reports completion (not cancellation)
  - No collision penetration detected
Why:
  - This is the exact scenario from GAME-1234
  - Without this test, the bug could regress
```

#### [CRITICAL] Test: DashAbility_SweepUsesCharacterCapsule

```
Test Case: DashAbility_SweepUsesCharacterCapsule
Category: Unit
Priority: CRITICAL (verifies core fix mechanism)
Setup:
  - Create SActionDash instance with known capsule radius (34.0)
Action:
  - Call CalculateSweep()
Expected:
  - Sweep radius matches character capsule component radius
  - Sweep is capsule-shaped, not line trace
Why:
  - The original bug used a line trace which missed corner geometry
  - Capsule sweep must match the actual character collision shape
```

#### [HIGH] Test: DashAbility_CornerAngles_45_90_135

```
Test Case: DashAbility_CornerAngles_45_90_135
Category: Functional (parameterized)
Priority: HIGH (edge cases of the fix)
Setup:
  - Spawn character at fixed position
  - Create wall corners at 45, 90, and 135 degree angles
  - Set dash to travel through each corner
Action:
  - Activate dash toward each corner angle
Expected:
  - Character stops at wall surface for all three angles
  - No penetration at any angle
  - Dash distance is correctly truncated
Why:
  - Different angles stress-test the sweep geometry
  - 45-degree corners are the most likely to cause edge cases
```

#### [HIGH] Test: DashAbility_StopsAtThinWall

```
Test Case: DashAbility_StopsAtThinWall
Category: Functional
Priority: HIGH (regression protection)
Setup:
  - Spawn character at position (100, 0, 0)
  - Place thin wall (10 units thick) at (200, 0, 0)
  - Set dash distance to 400 units
Action:
  - Activate dash ability through the thin wall
Expected:
  - Character stops at the near face of the thin wall
  - Character does NOT tunnel through the wall
Why:
  - Thin walls are a common tunneling scenario
  - Capsule sweep should catch this where line trace might not
```

#### [MEDIUM] Test: DashAbility_SlideAlongWallSurface

```
Test Case: DashAbility_SlideAlongWallSurface
Category: Functional
Priority: MEDIUM (related behavior)
Setup:
  - Spawn character at position (100, 100, 0)
  - Place wall at shallow angle to dash direction
  - Set dash distance to 300 units
Action:
  - Activate dash ability at shallow angle to wall
Expected:
  - Character slides along wall surface
  - Remaining dash distance is consumed along the wall
  - No abrupt stop (smooth deflection)
Why:
  - Players expect dash to feel smooth, not sticky
  - Wall sliding is a natural extension of the collision fix
```

### Existing Tests to Update

#### TestDashAbility::TestBasicDash
- **Current**: Tests dash travels full distance in open space
- **Update needed**: Add assertion that dash respects collision in corner scenario
- **File**: Source/ActionRoguelike/Private/Tests/TestDashAbility.cpp:45

### Testing Effort Estimate
- Critical tests: ~30 minutes to implement
- High priority tests: ~45 minutes to implement
- All suggested tests: ~2 hours total
- Recommendation: Implement Critical + High before merge, Medium in follow-up
