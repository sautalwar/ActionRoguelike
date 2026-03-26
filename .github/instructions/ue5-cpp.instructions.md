---
applyTo: "**/*.cpp,**/*.h"
---

# UE5 C++ File Conventions

These rules apply automatically to every `.cpp` and `.h` file in the repo.

---

## Header Files (.h)

- **Forward-declare** classes in headers instead of including their full header:
  ```cpp
  // Good — forward declare in .h
  class USActionComponent;
  class ASProjectileBase;

  // Bad — unnecessary #include in .h
  #include "SActionComponent.h"
  ```
- Use `#pragma once` as the include guard (UE5 standard).
- `GENERATED_BODY()` must be the **first line** inside the class body:
  ```cpp
  UCLASS()
  class ACTIONROGUELIKE_API ASCharacter : public ACharacter
  {
      GENERATED_BODY()

  public:
      ASCharacter();
      // ...
  };
  ```
- Order class sections: `public` → `protected` → `private`.
- Declare `UPROPERTY()` members before functions in each section.

---

## Source Files (.cpp)

- Include the **matching header first**, then engine headers, then project headers:
  ```cpp
  #include "SCharacter.h"          // 1. Matching header
  #include "GameFramework/Actor.h" // 2. Engine headers
  #include "SActionComponent.h"    // 3. Project headers
  ```
- Place all `#include` directives in `.cpp` files, not headers (unless required
  for inline/template code).

---

## UE5 String & Container Usage

Always use Unreal types in gameplay code:

```cpp
// Strings
FString PlayerName = TEXT("Hero");         // General text
FName   TagName    = FName("Action.Fire"); // Identifiers
FText   UILabel    = NSLOCTEXT("Game", "HealthLabel", "Health"); // Localized UI

// Containers
TArray<AActor*> Targets;
TMap<FGameplayTag, float> CooldownMap;
TSet<FName> ActiveBuffs;

// Never use std::string, std::vector, std::map, std::set in gameplay code
```

---

## Logging Patterns

Use categorized `UE_LOG` — never `printf`, `std::cout`, or `OutputDebugString`:

```cpp
// Declare in header or module-level .cpp
DECLARE_LOG_CATEGORY_EXTERN(LogActionRoguelike, Log, All);

// Use in .cpp
UE_LOG(LogActionRoguelike, Log,     TEXT("Spawned projectile: %s"), *GetNameSafe(NewProjectile));
UE_LOG(LogActionRoguelike, Warning, TEXT("Missing ActionComp on %s"), *GetNameSafe(OwnerActor));
UE_LOG(LogActionRoguelike, Error,   TEXT("Null target in %s"), *FString(__FUNCTION__));
```

For temporary debug output during development only:

```cpp
if (GEngine)
{
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT("Damage: %.1f"), Damage));
}
```

---

## Const Correctness

- Mark **all** query/getter functions `const`:
  ```cpp
  UFUNCTION(BlueprintPure, Category = "Attributes")
  bool IsAlive() const;

  UFUNCTION(BlueprintPure, Category = "Attributes")
  float GetHealth() const;
  ```
- Use `const` references for input parameters that shouldn't be modified:
  ```cpp
  void ApplyDamage(const FDamageEvent& DamageEvent, AActor* DamageCauser);
  ```
- Prefer `const TArray<>&` over `TArray<>` for read-only array parameters.

---

## FORCEINLINE Guidelines

- Use `FORCEINLINE` **only** for trivial getters/setters (1–2 lines):
  ```cpp
  FORCEINLINE USActionComponent* GetActionComp() const { return ActionComp; }
  ```
- **Do not** use `FORCEINLINE` on:
  - Virtual functions
  - Functions with branching logic
  - Functions longer than ~3 lines
  - Functions called from Blueprint (UFUNCTION)
- When in doubt, let the compiler decide — omit `FORCEINLINE`.

---

## UPROPERTY / UFUNCTION Quick Reference

```cpp
// Editable in editor, visible in Blueprint
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
float BaseDamage;

// Set once in constructor, read-only in Blueprint
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
TObjectPtr<UStaticMeshComponent> MeshComp;

// Replicated with notify
UPROPERTY(ReplicatedUsing = OnRep_Health)
float Health;

// Blueprint-callable action
UFUNCTION(BlueprintCallable, Category = "Actions")
bool StartAction(AActor* Instigator, FName ActionName);

// Server RPC
UFUNCTION(Server, Reliable)
void ServerRPC_FireProjectile(FVector Location, FRotator Rotation);
```

---

## Error Handling in C++

```cpp
// Fatal — invariant must be true
check(ActionComp != nullptr);
checkf(Index >= 0, TEXT("Invalid index: %d"), Index);

// Non-fatal — log and continue
ensure(TargetActor != nullptr);
ensureMsgf(Health > 0.f, TEXT("Health is %.1f for %s"), Health, *GetNameSafe(this));

// Evaluated even in shipping builds
verify(World != nullptr);
```

Prefer `ensure` over `check` in gameplay systems so QA builds don't crash.
