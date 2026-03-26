# Decision: Test File Naming Convention

**Decided by**: Team convention
**Date**: 2025-01-05
**Status**: Active
**Tags**: testing, conventions

## Decision

All test files follow this naming pattern:
- File: `Test{ClassName}.cpp` in `Source/{Module}/Private/Tests/`
- Test class: `F{TestName}Test`
- Automation test path: `{ProjectName}.{Module}.{Feature}.{TestCase}`

## Examples

| Production File | Test File | Test Path |
|----------------|-----------|-----------|
| SActionDash.cpp | TestDashAbility.cpp | ActionRoguelike.Actions.Dash.BasicDash |
| SCharacter.cpp | TestCharacterActions.cpp | ActionRoguelike.Character.Actions.CanActivate |
| SActionComponent.cpp | TestActionComponent.cpp | ActionRoguelike.Actions.Component.AddRemove |

## Rationale

- Predictable: given a class name, you know where tests are
- Searchable: `Test*` glob finds all test files
- Hierarchical: automation test paths allow running subsets (all Dash tests, all Action tests)

## How Future Agents Should Use This

When creating tests:
1. Name the file `Test{Feature}.cpp` in the `Private/Tests/` directory
2. Use `IMPLEMENT_SIMPLE_AUTOMATION_TEST` with the hierarchical path
3. One test file per production class or feature area
