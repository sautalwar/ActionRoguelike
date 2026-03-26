---
description: "Fix a bug reported in Jira using the full agent workflow"
---

# Fix a Bug from Jira

You are fixing a bug reported in Jira. Follow this workflow end-to-end,
using the specialized agents and MCP tools available in this repository.

## Input

Provide the Jira ticket ID (e.g., `GAME-1234`) when invoking this prompt.

---

## Workflow

### Step 1 — Read the Jira Ticket

Use the **Jira MCP server** to fetch the ticket details:
- Summary, description, reproduction steps
- Priority and affected version
- Attachments (screenshots, logs, crash dumps)
- Linked tickets or related issues

Summarize the bug in one paragraph.

### Step 2 — Analyze the Codebase

Use `code_search` and `find_references` to:
- Locate all source files related to the bug
- Trace the execution path described in the repro steps
- Identify the exact line(s) where the bug occurs
- Map dependencies between affected systems

### Step 3 — Write the Spec

Invoke the **@spec-writer** agent to produce a full technical specification:
- Overview of the bug
- Root cause analysis with file paths and line numbers
- Proposed fix with code snippets
- Multiplayer implications
- Performance considerations
- Test strategy

### Step 4 — Review the Spec

Invoke the **@spec-reviewer** agent to review the spec for:
- Missed edge cases (infinite loops, race conditions, null states)
- Multiplayer replication concerns
- Missing test scenarios
- UE5 API correctness

Address all reviewer feedback before proceeding.

### Step 5 — Implement the Fix

Write the code fix following `copilot-instructions.md`:
- Use proper UPROPERTY/UFUNCTION macros
- Handle multiplayer replication if affected
- Add `ensure()` checks for defensive coding
- Follow naming conventions

### Step 6 — Write Tests

Invoke the **@test-writer** agent to create tests:
- Positive case: the fix resolves the reported bug
- Negative case: invalid inputs don't crash
- Edge cases: boundary conditions identified in the spec review
- Multiplayer case: if replication is involved

### Step 7 — Run Tests

Build and run the test suite:
```
UnrealEditor-Cmd.exe <ProjectPath> -RunTests="ActionRoguelike" -NoSound -NullRHI
```

Iterate until all tests pass.

### Step 8 — Submit the Change

Use the **Perforce MCP server** to:
1. Create a new changelist with description referencing the Jira ticket
2. Add modified and new files to the changelist
3. Submit the changelist

Or use **Git** if the project uses Git:
1. Create a feature branch: `fix/GAME-XXXX-description`
2. Commit with message: `fix: [GAME-XXXX] description`
3. Push and create a pull request

### Step 9 — Update Jira

Use the **Jira MCP server** to:
- Transition the ticket to "In Review" or "Done"
- Add a comment with: changelist/PR number, files changed, test results
- Link the PR if applicable

---

## Checklist Before Submitting

- [ ] Root cause identified and documented
- [ ] Fix follows copilot-instructions.md conventions
- [ ] All existing tests still pass
- [ ] New tests cover the specific bug and edge cases
- [ ] Multiplayer tested (if applicable)
- [ ] Jira ticket updated with resolution details
