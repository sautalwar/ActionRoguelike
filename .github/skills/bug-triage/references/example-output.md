# Bug Triage - Example Output

## Bug Triage: GAME-1234

### Classification
- **Severity**: P2 - Significant
- **Category**: Gameplay Logic
- **Complexity**: S (1-4 hours)
- **Risk**: Low (isolated to Action System)

### Affected Subsystems
- Primary: Action System (SActionDash, SActionComponent)
- Secondary: Character Movement (collision detection path during dash)
- Files likely involved:
  - Source/ActionRoguelike/Private/Actions/SActionDash.cpp (dash execution logic)
  - Source/ActionRoguelike/Public/Actions/SActionDash.h (if sweep params are in header)
  - Source/ActionRoguelike/Private/SCharacter.cpp (only if movement integration changes)

### Root Cause Hypothesis
"Dash ability passes through walls when used near corners"
- The dash action likely performs a linear movement over multiple frames or a single teleport
- Near corners, the collision sweep (if any) can pass through the gap between two wall segments
- Most likely: the sweep uses a line trace instead of a capsule sweep, missing the character's full collision volume
- Alternative: the dash moves the character in a single SetActorLocation call that skips collision entirely

### Verification Steps
1. Open SActionDash.cpp, find the movement/teleport logic
2. Check if it uses SweepSingleByChannel (correct) or SetActorLocation (incorrect)
3. If sweep: check sweep shape - should match character capsule radius
4. If teleport: need to add sweep check before final position

### Recommended Approach
1. Replace any direct position set with a capsule sweep matching character dimensions
2. If sweep already exists, increase sweep radius by character capsule half-height to catch corner geometry
3. Add a "slide along surface" fallback so the dash stops at the wall instead of canceling entirely
4. Create test map with corner geometry at various angles (45, 90, 135 degrees)

### Assignment Recommendation
- **Assign to**: @copilot (autonomous agent)
- **Reason**: P2 severity, Small complexity, isolated to 1-2 files, clear fix pattern
- **Human review**: Required (gameplay-affecting change, needs playtesting)
- **Estimated autonomous time**: ~20 minutes

### Related Context
- GAME-1100 (melee attack through walls) - previously fixed by upgrading line trace to capsule sweep
- Same fix pattern likely applies here
- TestDashAbility.cpp exists but doesn't test corner collision specifically
