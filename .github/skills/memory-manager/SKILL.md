---
name: memory-manager
description: Manage the shared agent memory system. Read, write, search, and maintain the cross-agent knowledge store. Use to discover what previous agents have learned, write new discoveries, search for relevant context, or clean up stale entries. WHEN: read agent memory, write discovery, search memory, what do agents know, shared knowledge, cross-agent context, memory index, find previous knowledge, agent learned, persist knowledge, institutional knowledge.
---

# Memory Manager Skill

Manages the shared agent memory system at `.github/memory/`. This skill is the interface between agents and the team's institutional knowledge.

## When to Use

- **Before starting work**: Search memory for relevant context about the subsystems you'll touch
- **After discovering something**: Write a new memory entry to help future agents
- **During review**: Check if a proposed change contradicts known patterns or warnings
- **During maintenance**: Clean up stale or low-confidence entries

## Operations

### 1. Search Memory

Given a task description or set of files, find relevant memory entries:

**Process:**
1. Read `_index.md` to get the full entry list
2. Match tags against the subsystems involved in the current task
3. Return relevant entries sorted by: warnings first (safety), then discoveries (context), then patterns (guidance), then decisions (constraints)

**Example Query**: "I'm modifying SActionDash.cpp to fix wall clipping"
**Relevant Entries**:
- [WARNING] tick-in-actions.md (tags: action-system) — Don't add Tick
- [DISCOVERY] dash-sweep-vs-linetrace.md (tags: action-system, collision) — Use capsule sweep
- [DISCOVERY] action-system-physics-dep.md (tags: action-system, build) — Physics module dependency
- [PATTERN] create-new-action.md (tags: action-system) — Action pattern reference
- [DECISION] action-system-pattern.md (tags: action-system) — All abilities use USAction
- [DEPENDENCY] action-to-character-movement.md (tags: action-system, movement) — Cast to custom movement component

### 2. Write Discovery

When an agent discovers something new:

**Template:**
```markdown
# Discovery: [Clear Title]

**Discovered by**: [agent name or "manual"]
**Date**: [YYYY-MM-DD]
**Confidence**: [HIGH/MEDIUM/LOW]
**Tags**: [comma-separated subsystem tags]

## What Was Discovered
[1-2 paragraphs explaining what was found]

## Context
[How this was discovered — what task, what error, what investigation]

## Recommendation
[What to do about it — specific, actionable advice]

## How Future Agents Should Use This
[When to apply this knowledge, what to check]
```

After writing the entry file, update `_index.md` with the new row.

### 3. Write Warning

For dangerous patterns or pitfalls:

**Template:**
```markdown
# Warning: [Clear Title]

**Severity**: [CRITICAL/HIGH/MEDIUM]
**Author**: [who discovered this]
**Date**: [YYYY-MM-DD]
**Tags**: [comma-separated]

## The Rule
[One sentence: what to never do, or always do]

## Why This Is Critical
[Numbered list of consequences]

## What to Do Instead
[Code examples of the correct approach]

## How Future Agents Should Use This
[When this applies, how to check]
```

### 4. Write Pattern

For reusable code patterns:

**Template:**
```markdown
# Pattern: [What This Creates/Does]

**Author**: [who wrote it]
**Date**: [YYYY-MM-DD]
**Tags**: [comma-separated]

## When to Use
[Conditions where this pattern applies]

## Step-by-Step
[Detailed implementation guide with code]

## Checklist
[Verification items]
```

### 5. Maintenance — Clean Up Stale Entries

Periodically review entries:
- Check if tagged subsystems still exist
- Verify HIGH confidence entries are still accurate
- Remove entries about refactored/deleted systems
- Mark stale entries with `[STALE]` prefix in _index.md

## Memory Propagation Flow

This is how memory flows between agents:

```
Agent A (GAME-1234)              Shared Memory              Agent B (GAME-1235)
     |                               |                            |
     |-- 1. Reads _index.md -------->|                            |
     |-- 2. Reads relevant entries -->|                            |
     |                               |                            |
     |   [Works on the bug]          |                            |
     |                               |                            |
     |-- 3. Discovers Physics dep -->|                            |
     |-- 4. Writes discovery ------->|  discoveries/              |
     |-- 5. Updates _index.md ------>|  action-system-physics-    |
     |                               |  dep.md                    |
     |                               |                            |
     |                               |<-- 6. Reads _index.md -----|
     |                               |<-- 7. Reads discovery -----|
     |                               |                            |
     |                               |    Agent B now knows about |
     |                               |    the Physics dependency  |
     |                               |    without rediscovering it|
```

## Context Budget for Memory

To prevent memory from overwhelming an agent's context:
- Maximum 5 memory files read per session
- Prioritize: warnings > discoveries > patterns > decisions
- If >5 relevant entries exist, read the 5 most recently updated
- Total memory context should not exceed 20% of the agent's context budget
- The remaining 80% is for: instructions (10%), working memory/prompt file (20%), and actual code (50%)

## Integration with Workshop

- **Segment 2**: Introduce shared memory as "institutional knowledge for agents"
- **Segment 3**: Show the agent reading memory before starting the GAME-1234 fix
- **Segment 3B**: Show the autonomous agent WRITING to memory after discovering something
- **Segment 5**: Explain memory as a long-term investment — gets more valuable over time

**Workshop Talk Track:**
"Think of shared memory as your team's wiki, but for agents. When a senior developer discovers a quirk in the codebase, they tell colleagues in Slack or maybe update Confluence. That knowledge lives in one person's head or in a page nobody can find. Shared memory is structured, searchable, version-controlled, and automatically read by every agent. Over time, it becomes your team's most valuable asset — institutional knowledge that can never leave the company."
