# Build Analyzer - Example Output

## Scenario: Build failure after agent modified dash ability code

### Build Analysis Summary
- **Status**: FAILED
- **Total Time**: 2 minutes 14 seconds
- **Errors**: 1 compilation
- **Warnings**: 3 (1 significant)

### Issues Found

**Issue 1 (ERROR)**
- **File**: Source/ActionRoguelike/Private/Actions/SActionDash.cpp:47
- **Error**: `error C2039: 'GetCharacterMovement' is not a member of 'ASCharacter'`
- **Root Cause**: The agent used the base ACharacter API but SCharacter wraps movement differently. The correct accessor in this project is `GetMovementComponent()` cast to the project's custom movement component.
- **Fix**:
  ```cpp
  // Before (incorrect):
  UCharacterMovementComponent* Movement = GetCharacterMovement();
  
  // After (correct):
  USCharacterMovementComponent* Movement = Cast<USCharacterMovementComponent>(GetMovementComponent());
  ```
- **Why**: SCharacter uses a custom movement component (USCharacterMovementComponent). Direct ACharacter accessors bypass the custom class.

**Warning (SIGNIFICANT)**
- **File**: Source/ActionRoguelike/Public/Actions/SActionDash.h:12
- **Warning**: `include "GameFramework/Character.h" is redundant - already included via SCharacter.h`
- **Impact**: Adds ~0.3s to this translation unit's compile time
- **Fix**: Remove the redundant include

### Optimization Recommendations
1. **[HIGH]** SActionDash.h includes SCharacter.h but only needs a forward declaration. Move full include to .cpp. Estimated savings: 1.2s per incremental build touching this header.
2. **[MEDIUM]** Module ActionRoguelike has 67 translation units. Consider splitting gameplay actions into a separate module. Estimated savings: 30% faster incremental builds for action-only changes.

### Metrics
- Slowest translation units: SGameModeBase.cpp (8.2s), SCharacter.cpp (6.1s), SActionComponent.cpp (5.4s)
- Average include depth: 14 levels
- Estimated total savings if all recommendations applied: ~45 seconds per full build
