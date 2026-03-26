---
name: changelist-prep
description: Prepare and validate a Perforce changelist before submission. Checks for common mistakes, generates changelist descriptions, validates file list completeness, and catches debug code left in production files. Use before any P4 submit. WHEN: prepare changelist, validate submit, pre-submit check, P4 submit, changelist description, review changes before submit, check for debug code, missing files check.
---

# Changelist Prep Skill

Validates a Perforce changelist before submission and generates a professional changelist description. This is the pre-submit gate that catches the mistakes that break builds on Friday afternoon.

## When to Use

- Before submitting a changelist to Perforce
- When the autonomous agent has finished a fix and is preparing to submit/shelve
- During code review to validate changelist completeness
- When onboarding new developers to teach P4 submission best practices

## What This Skill Does

### Step 1: File List Validation

**Check for missing files:**
- New .h file without corresponding .cpp (or vice versa)
- New class without .generated.h being properly set up
- Modified Build.cs if new module dependencies were added
- Modified .uproject if new plugins were enabled
- New assets referenced in code but not in changelist

**Check for accidental includes:**
- .vs/ or .idea/ directory files
- Intermediate/ or Binaries/ directory files
- .pdb or .obj files
- Temporary test files or scratch files
- User-specific config files (*.user, *.suo)

**UE5-specific checks:**
- If a .h file in Public/ was modified, are dependent modules aware?
- If Build.cs was modified, do the module dependency changes make sense?
- If a .uasset was modified, is this a code-only changelist that shouldn't have assets?

### Step 2: Code Quality Checks

Scan all modified .cpp and .h files for:

**Debug Code Left In:**
- `UE_LOG(LogTemp, ...)` with temporary messages
- `GEngine->AddOnScreenDebugMessage()`
- `DrawDebugLine()`, `DrawDebugSphere()`, `DrawDebugBox()` without `#if ENABLE_DRAW_DEBUG`
- `check()` or `ensure()` with placeholder messages
- `// TODO`, `// HACK`, `// FIXME` without ticket references
- `#pragma message` left from debugging
- Hardcoded test values (magic numbers without named constants)
- `print()`, `printf()`, `FPlatformMisc::LowLevelOutputDebugString()`

**Common Mistakes:**
- Missing `#pragma once` in new headers
- Missing `GENERATED_BODY()` in new UCLASS/USTRUCT
- `.generated.h` not as last include
- Raw `new` instead of `NewObject<>()` or `CreateDefaultSubobject<>()`
- Missing `UPROPERTY()` on UObject pointers (GC won't track them)
- `FString` concatenation in hot path (should use `FStringBuilderBase`)
- Missing `const` on functions that don't modify state
- Missing `override` on virtual function overrides

### Step 3: Changelist Description Generation

Generate a professional CL description following studio conventions:

```
[GAME-1234] Fix dash ability wall clipping near corners

Summary:
- Replaced line trace with capsule sweep in SActionDash::ExecuteAction()
- Added corner geometry collision tests
- Sweep radius now matches character capsule dimensions

Testing:
- Unit test: TestDashAbility_CornerCollision (PASSED)
- Functional test: Dash at 45/90/135 degree corners (PASSED)
- Regression: Existing dash tests still pass
- Manual playtest: Verified in TestMap_Corners

Files Changed (3):
- Source/ActionRoguelike/Private/Actions/SActionDash.cpp (modified)
- Source/ActionRoguelike/Private/Tests/TestDashAbility.cpp (modified)
- Source/ActionRoguelike/Private/Tests/TestMaps/TestMap_Corners.umap (new)

Risk: Low - Change isolated to dash action sweep method
Reviewer: @senior-dev
```

### Step 4: Risk Assessment

Classify the changelist risk:
- **Low**: 1-3 files, single subsystem, has test coverage
- **Medium**: 4-10 files, crosses 2 subsystems, needs reviewer attention
- **High**: 10+ files, crosses 3+ subsystems, touches core/shared code
- **Critical**: Modifies engine code, changes save format, affects networking replication

For Medium+ risk, recommend:
- Specific reviewer(s) based on affected subsystems
- Whether a shelve-and-test-first approach is needed
- Whether to submit during low-traffic hours

## Output Format

```
## Changelist Validation: CL #12345

### File Check
- [PASS] All source file pairs complete (.h + .cpp)
- [PASS] Build.cs unchanged (no new module dependencies)
- [WARN] New test map added - verify it's in correct content directory
- [PASS] No accidental binary/intermediate files

### Code Quality
- [WARN] SActionDash.cpp:89 - DrawDebugLine() found without #if ENABLE_DRAW_DEBUG guard
- [PASS] No temporary UE_LOG statements
- [PASS] All new UPROPERTY pointers properly marked
- [INFO] 2 TODO comments found - both reference GAME-1234

### Generated Description
[formatted CL description as above]

### Risk Assessment
- **Risk Level**: Low
- **Files**: 3 changed
- **Subsystems**: Action System only
- **Recommendation**: Safe to submit during normal hours

### Pre-Submit Checklist
- [ ] Build locally (Debug + Development)
- [ ] Run affected tests
- [ ] Remove debug visualization (line 89)
- [ ] Verify test map is in correct content folder
```

## Integration with Workshop

- **Segment 3, Step 7**: After agent checks out files via Perforce MCP, invoke changelist-prep to validate before submit
- **Pain Point 4.1**: Broken builds block everyone - this is the pre-submit gate
- **Pain Point 2.4**: Shelving abuse - proper CL descriptions prevent anonymous shelves

Value proposition: "This catches the Friday afternoon submit that breaks Monday morning's build. Every changelist gets validated before it touches the depot."
