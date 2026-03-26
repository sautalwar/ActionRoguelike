# Memory Index

Last updated: 2025-01-15
Total entries: 7

## How to Use This Index
- Scan the tags column for your subsystem
- Read entries marked [HIGH] confidence first
- Entries are sorted newest-first within each category

## Discoveries

| File | Title | Tags | Confidence | Agent | Date |
|------|-------|------|------------|-------|------|
| discoveries/action-system-physics-dep.md | Action System requires Physics module | action-system, build, module-deps | HIGH | GAME-1234-agent | 2025-01-15 |
| discoveries/dash-sweep-vs-linetrace.md | Capsule sweep required for character abilities | action-system, collision, movement | HIGH | GAME-1234-agent | 2025-01-15 |
| discoveries/generated-header-ordering.md | .generated.h must be last include | build, headers, UHT | HIGH | manual | 2025-01-10 |

## Decisions

| File | Title | Tags | Date |
|------|-------|------|------|
| decisions/action-system-pattern.md | All abilities use UActionBase subclasses | action-system, architecture | 2025-01-05 |
| decisions/test-naming-convention.md | Test files follow Test{ClassName} pattern | testing, conventions | 2025-01-05 |

## Patterns

| File | Title | Tags | Date |
|------|-------|------|------|
| patterns/create-new-action.md | How to create a new Action System ability | action-system, template | 2025-01-05 |

## Warnings

| File | Title | Tags | Severity | Date |
|------|-------|------|----------|------|
| warnings/tick-in-actions.md | Never use Tick in Action subclasses | action-system, performance | CRITICAL | 2025-01-10 |
