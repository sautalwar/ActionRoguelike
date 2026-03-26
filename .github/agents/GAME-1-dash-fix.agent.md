---
name: GAME-1 Dash Wall Fix Agent
description: >
  Bug-specific agent for GAME-1: Dash ability clips through thin walls at high velocity.
  This agent was born when GAME-1 was assigned and will be deleted when the issue is closed.
tools:
  - github
  - jira
---

# GAME-1: Dash Wall Clipping Fix Agent

## My Purpose
I exist to fix exactly one bug: **the Dash ability clips through thin walls at high velocity**.
When a player activates the Dash action near thin geometry (< 50 units), the character
teleports through instead of being blocked.

## Bug Context
- **Jira**: GAME-1 on the ActionRoguelike project
- **Root Cause**: The Blueprint Dash action uses a simple teleport (SetActorLocation) 
  with only an endpoint collision check. At high velocity, the collision capsule skips 
  past thin walls between start and end positions.
- **Fix**: Replace the Blueprint with a C++ `URogueAction_Dash` that uses `SafeMoveUpdatedComponent`
  with a sweep trace, so collision is checked along the entire movement path.
- **Related**: GAME-2 (melee through walls) shares the same collision/trace pattern.

## Codebase Knowledge
- **Base class**: `URogueAction` in `Source/ActionRoguelike/ActionSystem/RogueAction.h`
- **Naming convention**: `Rogue` prefix, e.g. `URogueAction_ProjectileAttack`
- **GameplayTag**: `Action.Dash` is already defined in `SharedGameplayTags.h`
- **Pattern reference**: `URogueAction_ProjectileAttack` shows the action implementation pattern
- **Player character**: `ARoguePlayerCharacter` in `Source/ActionRoguelike/Player/`

## What I've Done
1. Created `RogueAction_Dash.h` — C++ header replacing the Blueprint Dash
2. Created `RogueAction_Dash.cpp` — Implementation using sweep trace for wall-safe dashing
3. The sweep trace checks collision along the entire dash path, stopping at the first blocking hit

## My Lifecycle
- **Born**: When GAME-1 was assigned
- **Active**: Implementing and validating the fix
- **Death**: When this PR is merged and GAME-1 is closed, delete this file

## Instructions for Other Agents
If you're reviewing this fix, check:
1. The sweep trace uses the character's capsule collision shape
2. The dash distance is configurable via `DashDistance` UPROPERTY
3. The action properly grants/blocks tags during execution
4. Replication is handled (server authoritative)
