# Example Changelist Validation Output

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
