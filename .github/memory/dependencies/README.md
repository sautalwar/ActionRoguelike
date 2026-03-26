# Cross-System Dependencies

This directory tracks dependencies between systems/modules that aren't obvious from the code structure. Agents should check here when modifying code that touches multiple subsystems.

## Format

Each file documents a dependency:
- What depends on what
- Why the dependency exists
- What breaks if you don't respect it
- How to handle it correctly
