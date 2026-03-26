# Pattern: Creating a New Action System Ability

**Author**: Team template
**Date**: 2025-01-05
**Tags**: action-system, template, how-to

## When to Use

Any time you need to add a new gameplay ability: combat move, buff, interaction, movement ability.

## Step-by-Step

### 1. Create the Header (.h)

```cpp
// Source/ActionRoguelike/Public/Actions/SAction_YourAbility.h
#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SAction_YourAbility.generated.h"

UCLASS()
class ACTIONROGUELIKE_API USAction_YourAbility : public USAction
{
    GENERATED_BODY()

public:
    USAction_YourAbility();

    virtual void StartAction_Implementation(AActor* Instigator) override;
    virtual void StopAction_Implementation(AActor* Instigator) override;
    virtual bool CanStart_Implementation(AActor* Instigator) override;

protected:
    // Designer-tunable properties
    UPROPERTY(EditDefaultsOnly, Category = "YourAbility")
    float Duration = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = "YourAbility")
    float Cooldown = 2.0f;
};
```

### 2. Create the Implementation (.cpp)

```cpp
// Source/ActionRoguelike/Private/Actions/SAction_YourAbility.cpp
#include "Actions/SAction_YourAbility.h"
#include "SActionComponent.h"
#include "GameFramework/Character.h"

USAction_YourAbility::USAction_YourAbility()
{
    ActionName = "YourAbility";
    bAutoStart = false;
}

bool USAction_YourAbility::CanStart_Implementation(AActor* Instigator)
{
    if (!Super::CanStart_Implementation(Instigator))
    {
        return false;
    }
    // Add your precondition checks here
    return true;
}

void USAction_YourAbility::StartAction_Implementation(AActor* Instigator)
{
    Super::StartAction_Implementation(Instigator);
    // Your ability logic here
}

void USAction_YourAbility::StopAction_Implementation(AActor* Instigator)
{
    // Cleanup here
    Super::StopAction_Implementation(Instigator);
}
```

### 3. Create Tests

```cpp
// Source/ActionRoguelike/Private/Tests/TestYourAbility.cpp
// Follow test-naming-convention decision
```

### 4. Register the Action

Either in the Character class or via a Data Asset:
```cpp
ActionComp->AddAction(this, USAction_YourAbility::StaticClass());
```

## Checklist

- [ ] Header in Public/Actions/
- [ ] Implementation in Private/Actions/
- [ ] .generated.h is last include
- [ ] All UPROPERTY on exposed properties
- [ ] EditDefaultsOnly for designer tuning
- [ ] Tests in Private/Tests/
- [ ] No Tick — use timers or duration system
- [ ] CanStart checks preconditions
- [ ] StopAction cleans up all state
