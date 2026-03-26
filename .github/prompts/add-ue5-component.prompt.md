---
description: "Add a new ActorComponent to the game using UE5 patterns"
---

# Add a New UE5 ActorComponent

Use this prompt when you need to create a new `UActorComponent` (or `USceneComponent`) subclass and integrate it into the game. Follow each section in order.

---

## 1. Create the Header File

Create `Source/<Module>/Public/<ComponentName>.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"  // or SceneComponent.h
#include "<ComponentName>.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class <MODULE>_API U<ComponentName> : public UActorComponent
{
    GENERATED_BODY()

public:
    U<ComponentName>();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;
};
```

**Key decisions:**
- Use `UActorComponent` for logic-only components (no transform)
- Use `USceneComponent` if the component needs a position in the world
- Set `PrimaryComponentTick.bCanEverTick = false` in the constructor unless ticking is required
- Add `BlueprintSpawnableComponent` only if designers need to add it in the editor

---

## 2. Create the Implementation File

Create `Source/<Module>/Private/<ComponentName>.cpp`:

- Include the header and any dependencies
- Constructor: initialize defaults, set tick, set replication
- `BeginPlay()`: bind delegates, resolve references, validate configuration
- `TickComponent()`: keep minimal — offload heavy work to helper functions

**UPROPERTY setup for exposed configuration:**

```cpp
// Editable in the Details panel, replicated, with tooltip
UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "MyCategory",
    meta = (ClampMin = "0.0", ClampMax = "100.0", ToolTip = "Damage per second"))
float DamageRate = 10.0f;
```

**Common UPROPERTY specifiers:**
| Use Case | Specifiers |
|---|---|
| Designer-tunable value | `EditAnywhere, BlueprintReadWrite, Category` |
| Runtime-only state | `BlueprintReadOnly, Transient` |
| Internal bookkeeping | `VisibleInstanceOnly` (or no specifier) |
| Delegate / Event | `BlueprintAssignable, Category` |

---

## 3. Component Registration in the Owning Actor

In the owning actor's constructor:

```cpp
MyComponent = CreateDefaultSubobject<UMyComponent>(TEXT("MyComponent"));
```

If it is a `USceneComponent` and needs to attach:

```cpp
MyComponent->SetupAttachment(RootComponent);
```

**Do NOT** create components in `BeginPlay` unless they are truly dynamic (spawned at runtime). Prefer `CreateDefaultSubobject` for deterministic initialization.

---

## 4. Multiplayer Replication

If the component holds gameplay state that must sync across clients:

**In the constructor:**
```cpp
SetIsReplicatedByDefault(true);
```

**Override `GetLifetimeReplicatedProps`:**
```cpp
void UMyComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UMyComponent, DamageRate);
    // Use DOREPLIFETIME_CONDITION for conditional replication
    DOREPLIFETIME_CONDITION(UMyComponent, InternalState, COND_OwnerOnly);
}
```

**Replication checklist:**
- [ ] Only replicate state that clients need — minimize bandwidth
- [ ] Use `COND_OwnerOnly` for state only the owning player needs
- [ ] Use `RepNotify` (`ReplicatedUsing`) when clients need to react to changes
- [ ] Test with `net.PktLoss=5` and `net.PktLag=100` to verify robustness
- [ ] Authority checks: gameplay mutations should happen on the server (`HasAuthority()`)

---

## 5. Blueprint Exposure

**Functions callable from Blueprint:**
```cpp
UFUNCTION(BlueprintCallable, Category = "MyCategory")
void ActivateAbility();

UFUNCTION(BlueprintPure, Category = "MyCategory")
float GetCurrentCharge() const;
```

**Events that Blueprints can bind to:**
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChargeChanged, float, NewCharge);

UPROPERTY(BlueprintAssignable, Category = "MyCategory|Events")
FOnChargeChanged OnChargeChanged;
```

**Blueprint exposure checklist:**
- [ ] Use `BlueprintCallable` for actions, `BlueprintPure` for queries
- [ ] Provide meaningful `Category` strings for organization in the Blueprint editor
- [ ] Add `meta = (ToolTip = "...")` for designer-facing documentation
- [ ] Avoid exposing internal implementation details — create clean API boundaries

---

## 6. DataAsset Configuration (If Needed)

If the component is data-driven (e.g., different abilities, weapon stats):

**Create a DataAsset class:**
```cpp
UCLASS()
class UMyComponentData : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
    float BaseDamage = 10.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
    TObjectPtr<UCurveFloat> DamageFalloff;
};
```

**Reference it from the component:**
```cpp
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
TObjectPtr<UMyComponentData> ComponentData;
```

**DataAsset guidelines:**
- Use DataAssets for values designers frequently tune
- Keep the DataAsset focused — one per concern, not one mega-config
- Validate DataAsset references in `BeginPlay` and log errors if null

---

## 7. Tests

Create tests matching the project's existing test framework:

```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMyComponentTest_Initialization,
    "Game.Components.MyComponent.Initialization",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FMyComponentTest_Initialization::RunTest(const FString& Parameters)
{
    // Arrange: spawn actor with component
    // Act: trigger behavior
    // Assert: verify expected state
    return true;
}
```

**Test coverage targets:**
- [ ] Component initializes with correct defaults
- [ ] Core behavior works under normal conditions
- [ ] Edge cases: null DataAsset, missing owner, zero delta time
- [ ] Replication: state syncs correctly (if replicated)
- [ ] Blueprint events fire with correct parameters

---

## Final Checklist

- [ ] Header and cpp created in correct module directories
- [ ] UCLASS, UPROPERTY, UFUNCTION macros are correct
- [ ] Component registered in owning actor via `CreateDefaultSubobject`
- [ ] Replication configured if component holds gameplay state
- [ ] Blueprint API is clean and documented
- [ ] DataAsset created if component is data-driven
- [ ] Tests written and passing
- [ ] No per-frame allocations in TickComponent
- [ ] Compiles with zero warnings
