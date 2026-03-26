---
name: bug-triage
description: Read a Jira ticket and produce structured bug triage including severity assessment, affected subsystems, estimated complexity, and recommended assignee. Use when a new bug comes in and needs triage, or when the autonomous agent needs to assess a ticket before starting work. WHEN: triage bug, assess ticket, severity check, estimate complexity, classify bug, new Jira ticket, prioritize bug, assign bug, bug assessment.
---

# Bug Triage Skill

Reads a Jira bug ticket (via MCP or pasted content) and produces a structured triage assessment. This replaces the manual triage meeting where 5 engineers spend 30 minutes classifying 20 tickets.

## When to Use

- New bug ticket arrives and needs classification
- Autonomous agent needs to assess a ticket before starting work
- Sprint planning needs complexity estimates for the backlog
- QA-to-dev handoff needs enriched ticket context
- Batch triage of stale backlog tickets

## What This Skill Does

### Step 1: Read the Ticket

Extract from the Jira ticket:
- Title and description
- Reproduction steps (if provided)
- Reporter and environment details
- Screenshots or video links (note them, can't view them)
- Linked issues / related tickets
- Custom fields (platform, build version, area)

### Step 2: Classify the Bug

**Severity Assessment** (based on actual impact, not reporter's opinion):
- **P0 - Crash/Blocker**: Game crashes, data loss, blocks other work, certification blocker
- **P1 - Major**: Core gameplay broken, no workaround, affects >50% of players
- **P2 - Significant**: Feature doesn't work correctly, workaround exists, affects specific scenarios
- **P3 - Minor**: Cosmetic, edge case, polish item, has simple workaround
- **P4 - Trivial**: Typo, minor visual glitch, nice-to-have fix

**Bug Category:**
- Gameplay logic (abilities, AI, physics, game rules)
- Rendering/Visual (shaders, materials, particles, UI)
- Performance (frame drops, memory leaks, loading times)
- Networking/Multiplayer (replication, lag, desync)
- Audio (missing sounds, volume, spatial audio)
- Platform-specific (console cert, platform API, input)
- Build/Infrastructure (compilation, packaging, cooking)
- Crash/Stability (null pointer, assertion, memory corruption)

### Step 3: Identify Affected Subsystems

Based on the bug description and codebase knowledge:
- List specific UE5 modules affected (e.g., "Action System, Character Movement")
- List likely source files (e.g., "SActionDash.cpp, SCharacterMovement.cpp")
- Identify if this crosses module boundaries (higher complexity)
- Flag if this affects shared/core systems (higher risk)

### Step 4: Estimate Complexity

**T-shirt sizing:**
- **XS (< 1 hour)**: Single file, obvious fix, no test changes needed
- **S (1-4 hours)**: 2-3 files, clear approach, existing test patterns to follow
- **M (4-8 hours)**: Multiple files, needs investigation, new tests needed, may need spec
- **L (1-3 days)**: Cross-module, needs design discussion, significant test coverage needed
- **XL (3-5 days)**: Architectural change, multiple subsystems, needs spec + review + extensive testing

**Complexity Factors:**
- Thread safety implications (+1 size)
- Replication/networking involved (+1 size)
- Affects save game data (+1 size for migration)
- Platform-specific behavior (+1 size per platform)
- No existing test coverage for area (+1 size)

### Step 5: Recommend Assignment

Based on severity and complexity:

| Severity | Complexity | Recommendation |
|----------|-----------|----------------|
| P0-P1 | Any | Human developer (senior for L/XL) |
| P2 | XS-S | @copilot (autonomous agent) |
| P2 | M | Human with agent assist (human-in-the-loop) |
| P2 | L-XL | Human developer with spec from @spec-writer |
| P3-P4 | XS-M | @copilot (autonomous agent) |
| P3-P4 | L-XL | Backlog (schedule in future sprint) |

### Step 6: Generate Triage Output

## Output Format

```
## Bug Triage: [TICKET-ID]

### Classification
- **Severity**: P2 - Significant
- **Category**: Gameplay Logic
- **Complexity**: S (1-4 hours)
- **Risk**: Low (isolated to Action System)

### Affected Subsystems
- Primary: Action System (SActionDash, SActionComponent)
- Secondary: Character Movement (collision detection)
- Files likely involved:
  - Source/ActionRoguelike/Private/Actions/SActionDash.cpp
  - Source/ActionRoguelike/Private/SCharacter.cpp

### Root Cause Hypothesis
Based on the description "dash passes through walls near corners":
- Likely: Collision sweep during dash doesn't account for corner geometry
- The dash action probably uses a linear sweep that can skip past thin wall corners
- Need to verify: Does SActionDash use a capsule sweep or ray trace?

### Recommended Approach
1. Verify the collision sweep method in SActionDash::ExecuteAction()
2. If using line trace, switch to capsule sweep matching character capsule
3. Add corner-case test with thin wall geometry
4. Test with various dash distances and angles

### Assignment Recommendation
- **Assign to**: @copilot (autonomous agent)
- **Reason**: P2 severity, S complexity, isolated subsystem, clear approach
- **Human review**: Required on PR (gameplay-affecting change)

### Related Context
- No linked issues found
- Similar pattern: GAME-1100 (melee attack through walls) - fixed via sweep upgrade
- Area has existing test coverage: TestDashAbility.cpp

### Estimated Agent Work
- Spec generation: ~2 minutes
- Implementation: ~5 minutes (1-2 files)
- Test creation: ~3 minutes
- Build + test cycle: ~10 minutes
- Total: ~20 minutes autonomous
```

## Integration with Workshop

This skill is demonstrated in:
- **Segment 3, Step 1**: After reading the Jira ticket, invoke bug-triage to classify it
- **Segment 3B**: The autonomous agent invokes this skill automatically before starting work
- **Pain Point 5.2**: QA-to-Dev feedback loop (9 days for 2h fix) - triage eliminates the sprint planning delay

Value proposition: "Your 30-minute triage meeting with 5 engineers? This skill produces the same output in 30 seconds. Engineers can review the triage async instead of sitting in a meeting."

## Guardrails

- Never auto-assign P0/P1 bugs to agents - these need human judgment
- Always flag if the bug might affect save game data or platform certification
- If complexity estimate is L or XL, recommend spec-first approach (never let an agent start coding without a spec on complex bugs)
- If there are >3 related/linked issues, flag potential systemic problem - may need architectural review, not just a bug fix
