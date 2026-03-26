# Copilot Instructions — ActionRoguelike (UE 5.6 C++)

These conventions apply to **every** Copilot session in this repository.
They ensure generated code is idiomatic Unreal Engine 5 C++ and consistent
with the ActionRoguelike project standards.

---

## Naming Conventions

| Prefix | Meaning | Example |
|--------|---------|---------|
| `A` | Actor | `ASCharacter`, `ASProjectileBase` |
| `U` | UObject / Component | `USActionComponent`, `USAttributeComponent` |
| `F` | Struct | `FCreatureRow`, `FActionData` |
| `E` | Enum | `EActionState`, `EDamageType` |
| `I` | Interface | `ISGameplayInterface` |
| `S` | Project prefix (ActionRoguelike) | Applied **after** the type prefix |
| `b` | Boolean member | `bIsRunning`, `bCanFire` |

- Use **PascalCase** for all types and functions.
- Use **camelCase** for local variables.
- Prefix private members with nothing extra — rely on `private:` visibility.

---

## UPROPERTY / UFUNCTION Macros

Every member variable that should survive garbage collection or be visible to
Blueprint **must** have a `UPROPERTY()` specifier.

```cpp
// Blueprint-visible, editable in the Details panel
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
float BaseDamage = 20.f;

// Visible in Blueprint graphs but not editable
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
TObjectPtr<USActionComponent> ActionComp;

// Replicated property
UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "Attributes")
float Health;
```

Functions exposed to Blueprint:

```cpp
UFUNCTION(BlueprintCallable, Category = "Actions")
void StartAction(AActor* Instigator, FName ActionName);

UFUNCTION(BlueprintPure, Category = "Attributes")
bool IsAlive() const;
```

---

## GameplayTags over Raw Strings

Use `FGameplayTag` and `FGameplayTagContainer` for system identification
instead of `FName` or raw strings:

```cpp
// Good
FGameplayTag ActionTag = FGameplayTag::RequestGameplayTag("Action.Attack.Projectile");

// Bad
FName ActionName = "ProjectileAttack";
```

Register tags in `DefaultGameplayTags.ini` or via a `UDataAsset`.

---

## Memory Management

| Do This | Never Do This |
|---------|---------------|
| `NewObject<UMyObject>(Outer)` | `new UMyObject()` |
| `CreateDefaultSubobject<UComp>(TEXT("Name"))` | `new UComp()` |
| `GetWorld()->SpawnActor<AMyActor>(...)` | `new AMyActor()` |
| `UPROPERTY()` on all UObject* members | Raw `UObject*` without UPROPERTY |
| `TWeakObjectPtr<AActor>` for non-owning refs | Raw pointers to actors you don't own |
| `TSharedPtr<>` / `TUniquePtr<>` for non-UObject | `std::shared_ptr` / `std::unique_ptr` |

**Never** use `new` / `delete` for UObject-derived types.
**Always** mark `UObject*` members with `UPROPERTY()` so the garbage
collector can track them.

---

## Architecture Patterns

- **Composition over inheritance** — prefer adding `UActorComponent` subclasses
  to actors rather than deep class hierarchies.
- **GameplayAbilitySystem (GAS)** — use `USAction` / `USActionComponent` for
  all player and AI abilities. Actions have GameplayTags for granting/blocking.
- **DataAssets over hardcoded values** — store tuning data in
  `UPrimaryDataAsset` subclasses loaded via the Asset Manager.
- **Event-driven** — use `FOnAttributeChanged`-style delegates and
  `BlueprintAssignable` multicast delegates to decouple systems.

---

## Container & String Types

| UE5 Type | C++ Standard Equivalent | Use |
|----------|------------------------|-----|
| `FString` | `std::string` | General text |
| `FName` | Interned string | Identifiers, tags |
| `FText` | Localized string | UI display |
| `TArray<T>` | `std::vector<T>` | Dynamic arrays |
| `TMap<K,V>` | `std::unordered_map<K,V>` | Hash maps |
| `TSet<T>` | `std::unordered_set<T>` | Hash sets |

**Never** use `std::string`, `std::vector`, `std::map` in gameplay code.

---

## Logging

Use `UE_LOG` with a project-specific category:

```cpp
DECLARE_LOG_CATEGORY_EXTERN(LogActionRoguelike, Log, All);

UE_LOG(LogActionRoguelike, Log, TEXT("Action started: %s"), *ActionName.ToString());
UE_LOG(LogActionRoguelike, Warning, TEXT("Health below zero for %s"), *GetNameSafe(OwnerActor));
UE_LOG(LogActionRoguelike, Error, TEXT("Failed to find ActionComp on %s"), *GetNameSafe(this));
```

**Never** use `printf`, `std::cout`, or `GEngine->AddOnScreenDebugMessage` in
production code (on-screen debug messages are fine in development only).

---

## Multiplayer / Replication

- Register replicated properties in `GetLifetimeReplicatedProps`:

```cpp
void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASCharacter, ActionComp);
    DOREPLIFETIME_CONDITION(ASCharacter, Health, COND_OwnerOnly);
}
```

- **ServerRPC** — called on client, executes on server:
  `UFUNCTION(Server, Reliable)` → function name: `ServerRPC_FireProjectile`
- **ClientRPC** — called on server, executes on owning client:
  `UFUNCTION(Client, Reliable)` → function name: `ClientRPC_ShowDamageNumber`
- **NetMulticast** — executes on all clients:
  `UFUNCTION(NetMulticast, Unreliable)` → function name: `MulticastRPC_PlayHitEffect`

Always test in **Listen Server** + **2-player PIE** before merging.

---

## Error Handling

| Macro | Behavior | Use When |
|-------|----------|----------|
| `check(expr)` | Fatal crash in all builds | Invariant that must never be false |
| `checkf(expr, fmt, ...)` | Fatal crash with message | Same, with context |
| `ensure(expr)` | Logs callstack, continues | Recoverable but unexpected |
| `ensureMsgf(expr, fmt, ...)` | Logs with message, continues | Same, with context |
| `verify(expr)` | Like check but evaluates in shipping | Side-effect expressions |

Prefer `ensure` for gameplay code so the game doesn't crash during QA.

---

## ⚠️ Hallucination Prevention — CRITICAL

When generating or suggesting UE5 code:

1. **NEVER assume a UE5 API exists** — use `find_references` or `code_search`
   to verify that a function, class, or macro is actually available in this
   project's engine version (5.6).
2. **ALWAYS check actual header files** for function signatures before calling
   them. Don't guess parameter types or order.
3. **UPROPERTY / UFUNCTION specifiers are strict** — find existing usage in
   the codebase with `code_search` to match the exact pattern before adding new
   ones. Incorrect specifiers cause compilation errors.
4. **Use class hierarchy tools** to verify inheritance before assuming a method
   exists on a base class. UE5 class hierarchies are deep — `ASCharacter` →
   `ACharacter` → `APawn` → `AActor` → `UObject`.
5. **Don't invent GameplayTags** — search `DefaultGameplayTags.ini` and
   existing DataAssets for valid tags.
6. **Don't fabricate plugin dependencies** — check `.uproject` and `.Build.cs`
   for actual module references.

## Shared Memory — Cross-Agent Knowledge

Before starting any task, read the shared memory index at `.github/memory/_index.md`. This contains knowledge discovered by previous agents that may be relevant to your work.

### Memory Reading Rules
1. ALWAYS read `_index.md` first — scan tags for your subsystem
2. Read entries tagged with your affected subsystems (max 5 entries)
3. Treat HIGH confidence entries as facts
4. Treat MEDIUM confidence entries as strong hints — verify if critical to your change
5. Treat LOW confidence entries as hypotheses — verify before relying on them

### Memory Writing Rules
When you discover something that would help future agents, write it to shared memory:
1. Create a new .md file in the appropriate category (discoveries/, decisions/, patterns/, warnings/)
2. Use the template format (see existing entries for examples)
3. Add the entry to `_index.md`
4. Set confidence level: HIGH if verified by build/test, MEDIUM if observed, LOW if hypothesis
5. Include tags for affected subsystems

### What Qualifies as a Memory-Worthy Discovery
- Unexpected module dependencies
- Build errors with non-obvious root causes
- Performance characteristics (measured, not guessed)
- Patterns that work well OR don't work in this codebase
- API quirks or undocumented behavior
- Integration points between subsystems

### What Does NOT Belong in Shared Memory
- Bug-specific implementation details (use prompt files)
- Temporary workarounds
- Speculative future plans
- Anything that changes every sprint
