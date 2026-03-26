# Agent Shared Memory

This directory is the shared knowledge store for all Copilot agents working on this project. Every agent reads this memory before starting work and writes to it when they discover something important.

## Memory Architecture — 4 Layers

```
Layer 4: Session Memory    (ephemeral - lives only during agent session)
Layer 3: Shared Memory     (this directory - cross-agent knowledge)
Layer 2: Working Memory    (prompt files - per-bug context)
Layer 1: Static Memory     (copilot-instructions.md + agent.md)
```

### How Memory Flows

1. Agent starts -> reads Layer 1 (project conventions) + Layer 2 (bug context) + Layer 3 (shared knowledge)
2. Agent works -> builds Layer 4 (session knowledge) as it reads files, discovers patterns, hits errors
3. Agent finishes -> writes important Layer 4 discoveries to Layer 3 (shared memory)
4. Next agent starts -> reads the updated Layer 3, inherits predecessor's knowledge

### Why This Matters

Without shared memory, every agent starts from zero. Agent A discovers that "SActionComponent requires Physics module dependency" after 10 minutes of debugging a linker error. Agent B hits the same problem next week and spends 10 minutes learning the same thing. Multiply by 50 bugs and you've wasted 500 minutes on re-discovery.

With shared memory, Agent A writes the discovery once. Every future agent reads it in milliseconds.

## Directory Structure

```
.github/memory/
  README.md              <-- You are here
  _index.md              <-- Memory manifest (what's in here, when it was written)
  discoveries/           <-- Things agents learned about the codebase
  decisions/             <-- Architectural decisions and their rationale
  patterns/              <-- Reusable code patterns found in the project
  warnings/              <-- Things to avoid (known pitfalls, anti-patterns)
  dependencies/          <-- Cross-system and cross-bug dependencies
```

## Rules for Agents

### Reading Memory
- ALWAYS read `_index.md` first to see what's available
- Read entries relevant to your task (check subsystem tags)
- Don't read everything — respect context budget (max 5 memory files per session)
- If _index.md shows a relevant entry, read it. If not, skip the category.

### Writing Memory
- Write when you discover something that would help future agents
- Use the templates in each category directory
- Add your entry to `_index.md` with date, agent, and tags
- Keep entries focused — one discovery per file
- Include the "why" not just the "what"

### What to Write
- Unexpected dependencies between modules
- Build errors and their root causes
- Patterns that work well in this codebase
- Patterns that DON'T work (and why)
- Configuration quirks
- Performance characteristics discovered during profiling

### What NOT to Write
- Bug-specific implementation details (those belong in prompt files, Layer 2)
- Temporary workarounds (those belong in code comments)
- Personal preferences (those belong nowhere)
- Anything that changes every sprint (stale memory is worse than no memory)

## Memory Lifecycle

- Entries are created by agents or developers
- Entries have a `confidence` level: high (verified by build/test), medium (observed but not tested), low (hypothesis)
- Low-confidence entries are reviewed by humans quarterly
- Stale entries (referenced subsystem was refactored) should be marked `[STALE]` and removed in next cleanup
- Memory is version-controlled with the repo — you can see who wrote what and when
