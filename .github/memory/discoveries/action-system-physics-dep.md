# Discovery: Action System Requires Physics Module Dependency

**Discovered by**: GAME-1234-agent
**Date**: 2025-01-15
**Confidence**: HIGH (verified by successful build)
**Tags**: action-system, build, module-deps

## What Was Discovered

The Action System (`SActionComponent`, `SActionBase`, and all action subclasses) depends on the Physics module for collision sweep operations. This dependency is NOT listed in the module's `Build.cs` — it works because the Engine module transitively includes it.

However, when adding new collision-related code to action subclasses (like the dash wall-clip fix in GAME-1234), you may need to explicitly add `"PhysicsCore"` to `PrivateDependencyModuleNames` in `ActionRoguelike.Build.cs`.

## Context

During the GAME-1234 fix (dash ability passes through walls), the agent added a `SweepSingleByChannel` call to `SActionDash.cpp`. The build succeeded because `Engine` transitively includes the Physics headers. But a future module split could break this if the Action System is moved to its own module.

## Recommendation

- Add `"PhysicsCore"` to `PrivateDependencyModuleNames` explicitly
- This makes the dependency visible and prevents future breakage
- Cost: zero runtime impact (already linked transitively)

## How Future Agents Should Use This

If you're modifying any Action class and adding physics/collision code:
1. Check that `PhysicsCore` is in Build.cs dependencies
2. If not, add it as a PrivateDependency
3. Include `#include "Physics/PhysicsCore.h"` explicitly, don't rely on transitive includes
