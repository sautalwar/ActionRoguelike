---
name: build-analyzer
description: Analyze Unreal Build Tool (UBT) output to identify slow compilation units, suggest PCH optimizations, and diagnose build failures. Use when build times are slow, builds fail with cryptic errors, or when optimizing compile times. WHEN: build is slow, analyze build log, optimize compilation, PCH tuning, include analysis, translation unit bottleneck, UBT errors, link errors, build optimization.
---

# UE5 Build Analyzer

Analyzes Unreal Build Tool (UBT) build output to diagnose failures and optimize compile times. This is the skill game developers invoke when their 1-line C++ change triggers a 10-minute rebuild.

## When to Use

- Build failed and error messages are cryptic
- Build succeeded but took too long
- Want to identify which translation units are slowest
- Need PCH (precompiled header) optimization suggestions
- Investigating include chain bloat
- After an agent makes code changes and the build has issues

## What This Skill Does

### 1. Build Failure Diagnosis

When given build output containing errors:

**Step 1: Classify the Error**
- Compilation error (syntax, type mismatch, missing include)
- Linker error (unresolved symbol, duplicate symbol, missing library)
- UBT error (module dependency, plugin configuration, Build.cs issue)
- Resource error (missing asset reference, cooking failure)

**Step 2: Identify Root Cause**
For each error:
- Parse the exact error code and message
- Identify the source file and line number
- Check if it's a cascading error (fix the first one, others may resolve)
- Look for common UE5 patterns:
  - `GENERATED_BODY()` missing or in wrong position
  - Forward declaration needed instead of full include
  - Module not listed in Build.cs dependencies
  - `UFUNCTION()` / `UPROPERTY()` macro issues
  - Missing `#include "ClassName.generated.h"` (must be last include)

**Step 3: Suggest Fix**
Provide the exact code change needed, following UE5 conventions:
- Include the file path and line number
- Show before/after code
- Explain WHY this fixes it (not just what to change)

### 2. Build Time Analysis

When given build timing output or when builds are slow:

**Step 1: Parse Timing Data**
Look for UBT timing output patterns:
- `Total time in XYZ actions: N.Ns`
- `Compiling X actions, Y in parallel`
- Individual translation unit compile times
- Link time vs compile time breakdown

**Step 2: Identify Bottlenecks**
Rank translation units by compile time. Common culprits in UE5:
- Files that include `Engine.h` or other monolithic headers
- Files with heavy template instantiation
- Generated code files (`.generated.cpp`)
- Files that transitively include Slate/UMG headers in non-UI modules

**Step 3: Optimization Recommendations**

Provide specific, actionable suggestions:

**Include Optimization:**
- Replace `#include "Engine.h"` with specific headers needed
- Use forward declarations in headers, full includes in .cpp only
- Move includes from .h to .cpp where possible
- Identify headers included but not used

**PCH Tuning:**
- Suggest candidates for SharedPCH based on frequency analysis
- Identify headers that should be in the PCH (included by >60% of module files)
- Flag headers that should NOT be in PCH (rarely used, large)

**Module Structure:**
- Identify modules that are too large (>50 translation units)
- Suggest module splits based on include dependency clusters
- Flag circular module dependencies

**Build Configuration:**
- Recommend Unity Build settings for the project
- Suggest Incredibuild/distributed compilation if available
- Recommend NVMe SSD for build output directories
- Suggest `-DisableUnity` for specific modules during iteration

### 3. Incremental Build Optimization

For developers complaining about incremental build times:

**Check:**
- Is Live Coding enabled? (can cause full rebuilds)
- Are there unnecessary header changes triggering cascading recompiles?
- Is the Derived Data Cache (DDC) configured and warm?
- Are intermediate files on SSD or spinning disk?

**Suggest:**
- Header/implementation split to reduce recompile cascading
- Use `PIMPL` pattern for frequently-changed implementation details
- Configure DDC for shared server in team environments
- Use `-NoLiveCoding` during automated builds

## Output Format

Always structure output as:

### Build Analysis Summary
- **Status**: [PASSED/FAILED]
- **Total Time**: X minutes Y seconds
- **Errors**: N compilation, M linker
- **Warnings**: N (highlight any that indicate real problems)

### Issues Found (if any)
For each issue:
- **File**: path/to/file.cpp:line
- **Error**: exact error message
- **Root Cause**: plain English explanation
- **Fix**: exact code change

### Optimization Recommendations (if build succeeded)
Ranked by impact:
1. [HIGH] specific recommendation with estimated time savings
2. [MEDIUM] specific recommendation
3. [LOW] nice-to-have improvement

### Metrics
- Slowest translation units (top 5)
- Total include depth (average)
- Estimated time savings if recommendations applied

## UE5-Specific Knowledge

### Common UBT Error Patterns
- `error C2039: 'XXX' is not a member of 'UYY'` -> Usually a missing include or the class changed modules
- `error LNK2019: unresolved external symbol` -> Module dependency missing in Build.cs
- `LogInit: Warning: Module 'XXX' not found` -> Plugin not enabled or module name wrong in .uproject
- `error: expected ';' after class` -> Missing semicolon after UCLASS declaration, or GENERATED_BODY() issue

### Build.cs Patterns
When suggesting Build.cs changes:
- PublicDependencyModuleNames for APIs used in public headers
- PrivateDependencyModuleNames for APIs used only in .cpp files
- Never add Engine-level modules to game modules unnecessarily

## Integration with Workshop

This skill is demonstrated in:
- **Segment 3, Step 4**: After the agent implements a fix and builds, invoke this skill if the build fails to diagnose
- **Segment 4**: Alongside @modernize agent to show build optimization
- **Pain Point 1.2**: Build times kill inner loop (8-15 min for 1-line change)

Value proposition: "Instead of reading 500 lines of build output and googling error codes, the build analyzer tells you exactly what broke and how to fix it. For slow builds, it tells you which files are bottlenecks and what to do about it."
