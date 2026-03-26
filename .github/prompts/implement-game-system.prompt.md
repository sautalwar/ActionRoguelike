---
description: "Implement a new game system or ability in UE5"
---

# Implement a New Game System

You are implementing a new game system or ability in the ActionRoguelike
project. Follow this workflow to ensure the implementation is architecturally
sound, multiplayer-safe, and well-tested.

## Input

Describe the system or ability you want to implement (e.g., "Add a Dash
ability with cooldown and i-frames" or "Create an inventory system with
weight limits").

---

## Workflow

### Step 1 — Analyze Existing Systems

Use `code_search` and `find_references` to study how similar systems are
already implemented in the project:

- **Action System**: Search for `USAction` subclasses to understand the
  ability pattern (Start/Stop, GameplayTags, cooldowns)
- **Components**: Search for `USAttributeComponent`, `USActionComponent`
  to understand the component architecture
- **Effects**: Search for `USActionEffect` subclasses for status effects
- **AI Integration**: Search for `UBTTaskNode` subclasses for AI behaviors
- **Data Assets**: Search for `UPrimaryDataAsset` subclasses for data-driven
  configuration

Document what patterns already exist and plan to reuse them.

### Step 2 — Plan the Architecture

Design the system using **composition over inheritance**:

```
New System Plan:
├── Component: USNewSystemComponent (on the actor)
├── Action: USAction_NewAbility (ability logic)
├── Effect: USActionEffect_NewBuff (if status effect)
├── DataAsset: USNewSystemDataAsset (tuning values)
├── AI Task: UBTTask_UseNewAbility (if AI should use it)
└── GameplayTags: Action.NewSystem.*, Status.NewBuff.*
```

For each class, determine:
- What it's responsible for
- Which properties need replication
- Which functions need Blueprint exposure
- How it integrates with existing systems

### Step 3 — Register GameplayTags

Add new tags to the project's tag registry:

```ini
# DefaultGameplayTags.ini
+GameplayTags=(Tag="Action.NewSystem.Activate", DevComment="Activates the new system")
+GameplayTags=(Tag="Status.NewBuff", DevComment="Applied while buff is active")
```

### Step 4 — Implement the Components and Actions

Write the implementation following `copilot-instructions.md`:

1. **Header file** (.h):
   - Forward declarations (not includes)
   - `GENERATED_BODY()` first in class
   - UPROPERTY/UFUNCTION with proper specifiers
   - Delegates for event-driven communication

2. **Source file** (.cpp):
   - Matching header first, then engine, then project includes
   - Constructor: `CreateDefaultSubobject<>()` for components
   - `BeginPlay`: Initialize runtime state
   - `GetLifetimeReplicatedProps`: Register replicated properties

3. **Data Asset**:
   - Store all tuning values (damage, cooldown, range, etc.)
   - Use `EditAnywhere` so designers can tweak in-editor
   - Load via Asset Manager or soft references

### Step 5 — Add Multiplayer Replication

For every new system, consider:

| Question | If Yes |
|----------|--------|
| Does the state need to be shared? | Add `DOREPLIFETIME` |
| Does the client need to trigger it? | Add `Server` RPC |
| Does the server need to notify clients? | Add `Client` or `NetMulticast` RPC |
| Is it cosmetic-only? | Keep it client-side, no replication needed |

Implement the replication layer:

```cpp
void USNewComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(USNewComponent, bIsActive);
    DOREPLIFETIME_CONDITION(USNewComponent, CurrentCharges, COND_OwnerOnly);
}
```

### Step 6 — Write Tests

Invoke the **@test-writer** agent to create tests covering:

| Test | Description |
|------|-------------|
| Basic functionality | System works in single-player |
| Activation/deactivation | Start and stop conditions |
| Cooldown | Cannot re-activate during cooldown |
| Interaction with existing systems | Doesn't break Action System, Attributes |
| Multiplayer | State replicates correctly to all clients |
| AI usage | AI can use the system via Behavior Tree |
| Save/Load | System state persists correctly |
| Edge cases | Rapid activation, activation during death, etc. |

### Step 7 — Integration Test

Test the complete flow in-editor:
1. Single-player: ability works as designed
2. Listen Server + 1 Client: replication works
3. AI usage: behavior tree tasks function correctly
4. Save → Quit → Load: state preserved

---

## Architecture Checklist

- [ ] Uses composition (components) not deep inheritance
- [ ] GameplayTags for identification (no raw strings)
- [ ] DataAsset for tuning values (no hardcoded numbers)
- [ ] All UObject* members have UPROPERTY()
- [ ] Multiplayer replication implemented and tested
- [ ] Follows copilot-instructions.md conventions
- [ ] Tests written and passing
- [ ] AI integration considered (even if deferred)
