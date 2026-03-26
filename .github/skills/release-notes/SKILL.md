---
name: release-notes
description: Generate professional release notes from Jira tickets, Perforce changelists, and GitHub PRs. Creates audience-appropriate notes for different stakeholders (dev team, QA, producers, external). Use at sprint end, milestone, or before a build drop. WHEN: generate release notes, sprint summary, changelog, build notes, what shipped this sprint, milestone report, build drop notes, QA handoff notes.
---

# Release Notes Generator

Generates professional release notes from a combination of Jira tickets, Perforce changelists, and GitHub PRs. Different audiences need different levels of detail — this skill produces the right format for each.

## When to Use

- End of sprint: summarize what shipped
- Before a build drop to QA: list changes for testing focus
- Milestone completion: executive summary for producers
- Before certification submission: comprehensive change list for platform requirements
- After an agent completes a batch of fixes: summarize autonomous work

## What This Skill Does

### Step 1: Gather Change Data

From available sources (via MCP or provided data):

**Jira Tickets:**
- All tickets moved to "Done" or "Resolved" in the time period
- Group by: type (bug fix, feature, improvement, task)
- Extract: ticket ID, title, priority, component, assignee
- Note: which were human-fixed vs agent-fixed (@copilot)

**Perforce Changelists:**
- All CLs submitted in the time period
- Extract: CL number, description, author, file count
- Group by: module/directory
- Flag: large CLs (>20 files) for special mention

**GitHub PRs (if using GitHub flow):**
- Merged PRs in the time period
- Extract: PR number, title, author, labels
- Note: agent-created PRs (copilot/fix-* branches)

### Step 2: Generate Audience-Specific Notes

#### Format A: Dev Team Notes (Internal)
Technical, detailed, grouped by subsystem:
```
## Sprint 47 Release Notes (Dev Team)
### Build: 4.7.123 | Branch: main | Date: 2024-01-15

### Gameplay Systems (8 changes)
- [GAME-1234] Fixed dash ability wall clipping near corners (P2)
  - Files: SActionDash.cpp, TestDashAbility.cpp
  - Agent-fixed: Yes (@copilot, 22 min autonomous)
- [GAME-1240] Added cooldown visualization to action bar (Feature)
  - Files: SActionComponent.cpp, WActionBar.cpp, WActionBar.h
  - Author: @dev-name

### Character Movement (3 changes)
- [GAME-1238] Fixed jitter on slopes when sprinting (P2)
  ...

### AI (2 changes)
...

### Stats
- Total tickets resolved: 13
- Agent-fixed: 4 (31%)
- Average fix time (agent): 28 minutes
- Average fix time (human): 6.2 hours
- Build stability: 100% (0 broken builds this sprint)
```

#### Format B: QA Build Notes
Testing-focused, highlights what needs verification:
```
## QA Build Notes: 4.7.123

### MUST TEST (Bug Fixes - P1/P2)
1. [GAME-1234] Dash through walls near corners
   - Test: Dash at various corner angles (45, 90, 135 degrees)
   - Regression: Verify normal dash still works in open areas
   - Maps: TestMap_Corners, Level_01, Level_03 (has tight corridors)

2. [GAME-1238] Sprint jitter on slopes
   - Test: Sprint on all slope angles in TestMap_Terrain
   - Regression: Walk/jog on slopes should be unaffected

### SHOULD TEST (New Features)
3. [GAME-1240] Cooldown visualization on action bar
   - Test: Activate each ability, verify cooldown circle displays correctly
   - Edge case: Multiple abilities on cooldown simultaneously

### KNOWN ISSUES (Not Fixed This Build)
- [GAME-1250] Audio stutter in multiplayer (in progress)
- [GAME-1255] Minimap icons misaligned (scheduled next sprint)

### Test Environment
- Build: 4.7.123 from CL #45678
- Platform: PC (Windows 11), PS5 dev kit
- Config: Development build with logging enabled
```

#### Format C: Producer Summary
High-level, milestone-focused, metric-driven:
```
## Sprint 47 Summary

### Velocity
- Planned: 34 story points | Delivered: 31 story points (91%)
- Tickets resolved: 13 | Carried over: 3

### Highlights
- Dash ability wall clipping fixed (top player-reported bug)
- Cooldown visualization shipped (design milestone requirement)
- AI patrol system optimization (15% CPU reduction)

### Agent Productivity
- 4 of 13 tickets handled autonomously by @copilot
- Agent average turnaround: 28 minutes vs human 6.2 hours
- No agent-created regressions this sprint

### Risk Items
- Audio stutter in multiplayer still open (P1, needs audio engineer)
- Certification deadline in 3 sprints - 7 cert-blocking bugs remain

### Next Sprint Focus
- P1 audio stutter
- Networking replication bugs (3 tickets)
- Begin certification prep checklist
```

#### Format D: Certification Changelog
Formal, exhaustive, platform-requirement format:
```
## Change Log: Build 4.7.123

| CL | Ticket | Description | Risk | Files Changed |
|----|--------|-------------|------|---------------|
| 45672 | GAME-1234 | Fix collision sweep in dash ability | Low | 3 |
| 45675 | GAME-1238 | Fix character movement on slopes | Low | 2 |
| 45678 | GAME-1240 | Add cooldown UI visualization | Low | 4 |
...

### Compliance Notes
- No changes to networking replication code
- No changes to save/load system
- No changes to platform-specific code (PS5/Xbox/Switch)
- No changes to mtx/store functionality
- Memory profile: +0.2MB (new UI textures for cooldown display)
```

### Step 3: Highlight Agent vs Human Work

Always include a section showing:
- Which tickets were handled by @copilot vs human developers
- Average time-to-resolution for each
- Quality metrics (regressions introduced, PR review feedback)

This data builds the case for expanding agent usage over time.

## Output Customization

The skill adapts based on context:
- If given a date range: generates notes for that period
- If given specific tickets: generates notes for those tickets only
- If given a milestone name: groups by milestone criteria
- Default: generates all four formats (A, B, C, D)

## Integration with Workshop

- **Segment 5 (Rollout)**: Show how release notes auto-generate at sprint end
- **Pain Point 3.2**: Backlog rot - release notes provide audit trail of what actually shipped
- **Pain Point 5.2**: QA feedback loop - QA build notes tell testers exactly what to verify

Value proposition: "Your producer spends 2 hours every sprint compiling release notes from Jira queries and Slack messages. This skill generates four audience-specific versions in 30 seconds. And it includes agent productivity metrics that help you measure ROI."
