# Discovery: .generated.h Must Be Last Include

**Discovered by**: manual (team knowledge)
**Date**: 2025-01-10
**Confidence**: HIGH (UHT requirement)
**Tags**: build, headers, UHT, conventions

## What Was Discovered

In any UE5 header file that uses UCLASS(), USTRUCT(), UENUM(), or UFUNCTION() macros, the `.generated.h` include MUST be the last #include directive. Unreal Header Tool (UHT) requires this for code generation.

## The Pattern

```cpp
// CORRECT
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"  // MUST be last

// INCORRECT - will cause cryptic UHT errors
#include "CoreMinimal.h"
#include "SCharacter.generated.h"  // Wrong position!
#include "GameFramework/Character.h"
```

## Error Symptoms

If .generated.h is not last, you'll see:
- `error: expected class name` on GENERATED_BODY()
- UHT crashes with unhelpful error messages
- "Unable to find parent class" during code generation
- Build succeeds in some configurations but fails in others

## How Future Agents Should Use This

When creating new .h files:
1. Always put .generated.h as the last #include
2. Put CoreMinimal.h as the first #include
3. Never add includes after .generated.h
