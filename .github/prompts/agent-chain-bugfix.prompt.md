---
description: "Full agent chain: spec -> review -> implement -> test for a bug fix"
---

# Agent Chain: Bug Fix Workflow

This prompt orchestrates the full agent chain for resolving a bug from initial analysis through verified fix. Each step hands off to the next agent in sequence. Follow these steps in order.

---

## Step 1 — Spec Writer (@spec-writer)

> Generate a technical specification for the bug fix.

**What to do:**
1. Tag `@spec-writer` and provide the bug ticket ID and reproduction steps
2. The spec writer will analyze the codebase, identify the root cause, and produce a technical spec including:
   - Root cause analysis with specific file and line references
   - Proposed fix approach (with alternatives considered)
   - Affected systems and potential side effects
   - Rollback strategy

**What to expect:**
- A markdown spec document with clear sections
- File references should be verified against the actual codebase
- Estimated scope (number of files changed, risk level)

**If something goes wrong:**
- If the spec writer cannot locate the relevant code, provide additional context about the module or system
- If multiple root causes are identified, ask the spec writer to rank them by likelihood
- If the bug spans multiple systems, request separate spec sections per system

---

## Step 2 — Spec Reviewer (@spec-reviewer)

> Review the spec for edge cases, completeness, and correctness.

**What to do:**
1. Tag `@spec-reviewer` and paste or reference the spec from Step 1
2. Ask it to evaluate:
   - Are all edge cases covered? (null states, race conditions, boundary values)
   - Does the fix introduce any new failure modes?
   - Is the rollback strategy viable?
   - Are multiplayer/replication implications addressed?
   - Is the performance impact acceptable for a per-frame system?

**What to expect:**
- A structured review with PASS / NEEDS WORK / BLOCKER ratings per section
- Specific questions or concerns that must be resolved before implementation
- Suggested additions to the spec if gaps are found

**If something goes wrong:**
- If the reviewer flags blockers, return to Step 1 with the feedback and regenerate
- If there is disagreement on approach, escalate to the tech lead with both perspectives
- Iterate between Step 1 and Step 2 until the spec receives a PASS on all sections

---

## Step 3 — Implement the Fix (Agent Mode)

> Switch to Agent Mode and implement the fix based on the approved spec.

**What to do:**
1. Switch to **Agent Mode** in Copilot Chat
2. Provide the approved spec as context
3. Instruct the agent to:
   - Make changes file-by-file, following the spec exactly
   - Follow UE5 conventions from `.github/copilot-instructions.md`
   - Use `find_references` before modifying any function signature
   - Keep changes minimal — do not refactor unrelated code
   - Add inline comments only where the fix is non-obvious

**What to expect:**
- Code changes matching the spec with no extraneous modifications
- Proper UPROPERTY/UFUNCTION macros on any new or changed members
- Replication setup if the fix touches replicated state

**If something goes wrong:**
- If the agent modifies files not in the spec, revert those changes and re-prompt with stricter scope
- If a compile error occurs, share the error output and ask the agent to fix it
- If the agent is uncertain about an API, have it use `find_references` to verify usage patterns

---

## Step 4 — Write Tests (@test-writer)

> Create tests that verify the fix and guard against regression.

**What to do:**
1. Tag `@test-writer` with the bug description and the files changed
2. Request tests that:
   - Reproduce the original bug (should fail without the fix)
   - Verify the fix under normal conditions
   - Cover edge cases identified in the spec review
   - Match existing test patterns in the project (Automation framework, Gauntlet, or custom)

**What to expect:**
- Test files placed in the correct directory following project conventions
- Tests that are deterministic and do not depend on frame timing unless explicitly testing async behavior
- Clear test names that describe the scenario: `Test_MagicProjectile_AppliesBurningDOT_WhenThornsBuffActive`

**If something goes wrong:**
- If no existing test patterns are found, ask the test writer to scaffold a minimal test harness
- If tests require game state setup that is complex, break into smaller focused tests
- If tests are flaky, check for tick-dependent logic and add explicit waits or mocks

---

## Step 5 — Build & Test (Iterate Until Green)

> Compile the project and run all tests. Iterate until everything passes.

**What to do:**
1. Build the project using the standard build command
2. Run the relevant test suite
3. If tests fail:
   - Read the failure output carefully
   - Determine if the failure is in new tests or existing tests
   - If new tests fail, return to Step 3 or Step 4 to adjust
   - If existing tests fail, the fix has a regression — return to Step 3
4. Repeat until all tests pass with zero warnings in changed files

**What to expect:**
- Clean build with no new warnings
- All new tests pass
- All existing tests continue to pass
- No memory leaks reported by static analysis

**If something goes wrong:**
- If build fails due to missing includes, check the header dependency chain
- If tests pass locally but fail in CI, check for platform-specific behavior
- If iteration count exceeds 3, pause and reassess the approach from Step 1

---

## Step 6 — Performance Check (@profiler)

> Verify the fix does not introduce performance regressions.

**What to do:**
1. Tag `@profiler` with the changed files and the nature of the fix
2. Ask it to check for:
   - New per-frame allocations (heap allocations in Tick, ReceiveDamage, etc.)
   - Unnecessary object copies (especially TArrays, FStrings, UStructs)
   - Added cost in hot paths (damage calculation, projectile updates)
   - Delegate binding/unbinding frequency
   - Cache-unfriendly access patterns

**What to expect:**
- A performance assessment with PASS / WARNING / FAIL per category
- Specific line-level callouts for any concerning patterns
- Suggested optimizations if warnings are found

**If something goes wrong:**
- If performance warnings are found, return to Step 3 and optimize
- If the fix fundamentally requires expensive operations, document the tradeoff in the spec
- For borderline cases, request a benchmark comparison (before/after)

---

## Step 7 — Source Control (Perforce MCP)

> Check out files and create a changelist via Perforce MCP.

**What to do:**
1. Use the Perforce MCP to check out all modified files
2. Create a new changelist with:
   - Description: `[GAME-XXXX] <brief fix description>`
   - Include all changed source files and test files
   - Exclude any scratch or temporary files
3. Verify the changelist contents match the spec's expected file list

**What to expect:**
- All modified files checked out and in the new changelist
- No unexpected files included
- Changelist ready for code review

**If something goes wrong:**
- If files are already checked out by another user, coordinate or use a branch
- If the Perforce MCP is unavailable, document the file list for manual checkout
- Verify no files were accidentally reverted during the process

---

## Step 8 — Update Ticket (Jira MCP)

> Update the Jira ticket with implementation notes.

**What to do:**
1. Use the Jira MCP to update the bug ticket with:
   - **Status**: Move to "In Review" (or equivalent)
   - **Implementation Notes**: Summary of the fix, files changed, test coverage
   - **Changelist**: Perforce CL number
   - **Risk Assessment**: Low / Medium / High with justification
   - **Test Plan**: List of tests added and what they verify
2. Add a comment tagging the reviewer

**What to expect:**
- Ticket updated with all relevant information
- Reviewer notified and can find everything they need in the ticket

**If something goes wrong:**
- If the Jira MCP is unavailable, prepare the update text and apply it manually
- If the ticket workflow requires additional fields, fill them based on the spec
- Ensure the ticket links to any related bugs discovered during the fix
