# Decision: All Abilities Use UActionBase Subclasses

**Decided by**: Architecture team
**Date**: 2025-01-05
**Status**: Active
**Tags**: action-system, architecture

## Decision

All gameplay abilities in this project are implemented as `USAction` subclasses managed by `USActionComponent`. This includes:
- Combat abilities (dash, melee, ranged attacks)
- Buff/debuff effects
- Movement abilities
- Interaction abilities

## Rationale

- **Consistency**: One pattern for all abilities makes the codebase predictable
- **Composability**: Actions can be granted/revoked at runtime via the component
- **Networking**: USActionComponent handles replication for all actions uniformly
- **Data-driven**: Action properties exposed via UPROPERTY for designer tuning

## The Pattern

```cpp
// 1. Create subclass of USAction
UCLASS()
class USActionDash : public USAction
{
    GENERATED_BODY()
public:
    virtual void StartAction_Implementation(AActor* Instigator) override;
    virtual void StopAction_Implementation(AActor* Instigator) override;
    virtual bool CanStart_Implementation(AActor* Instigator) override;
};

// 2. Register with ActionComponent (in character or via data asset)
ActionComp->AddAction(this, USActionDash::StaticClass());

// 3. Activate via ActionComponent
ActionComp->StartActionByName(this, "Dash");
```

## Constraints

- Do NOT create standalone ability actors — use the Action System
- Do NOT bypass USActionComponent for ability activation
- Do NOT tick in actions — use timers or the action's duration system
- All action state must be in UPROPERTY fields for replication

## How Future Agents Should Use This

When asked to implement any new ability or gameplay mechanic:
1. Create a new USAction subclass
2. Follow the existing pattern (see USActionDash, USActionMelee for examples)
3. Register via ActionComponent
4. Never create a standalone actor for an ability
