---
name: test-gap-finder
description: Analyze code changes and identify missing test coverage. Maps modified functions to existing tests, finds untested code paths, and suggests specific test cases to write. Use after implementing a fix or feature to ensure adequate test coverage. WHEN: find missing tests, test coverage gaps, what tests are needed, untested code, test analysis, coverage check, suggest test cases, verify test completeness.
---

# Test Gap Finder Skill

Analyzes code changes (a diff, a set of modified files, or a feature description) and identifies what test coverage is missing. This skill bridges the gap between "I wrote the fix" and "I'm confident it works."

## When to Use

- After implementing a bug fix, before creating a PR
- After the autonomous agent implements changes, to verify test completeness
- During code review to check if tests cover the actual changes
- When prioritizing which tests to write for an under-tested area
- Sprint planning to estimate testing effort for upcoming work

## What This Skill Does

### Step 1: Map Changes to Testable Behaviors

For each modified function/method:
- What behavior changed? (new branch, modified calculation, different return value)
- What are the inputs? (parameters, object state, external dependencies)
- What are the outputs? (return value, side effects, events fired, state changes)
- What are the edge cases? (null inputs, empty collections, boundary values, overflow)

### Step 2: Inventory Existing Tests

Search the codebase for existing test coverage:
- Find test files that reference the modified classes/functions
- Check test naming conventions to match tests to production code:
  - `Test*.cpp` files
  - `*Test.cpp` files
  - `*Spec.cpp` files
  - UE5 Automation tests: `IMPLEMENT_SIMPLE_AUTOMATION_TEST`
  - UE5 Functional tests in `Tests/` directories
- Note which behaviors are already tested vs untested

### Step 3: Identify Coverage Gaps

For each modified behavior, classify:

**Covered**: Existing test explicitly tests this behavior
- Cite the test name and file
- Note if the test needs updating (e.g., expected values changed)

**Partially Covered**: Test exists but doesn't cover the specific change
- Cite the test and explain what's missing
- Suggest additional test cases to add to existing test

**Not Covered**: No test exists for this behavior
- Flag as a gap
- Suggest specific test cases with inputs and expected outputs

**Untestable (as written)**: Code structure prevents easy testing
- Identify the coupling or dependency issue
- Suggest refactoring to make it testable (e.g., dependency injection)

### Step 4: Generate Test Suggestions

For each gap, provide a concrete test case specification:

```
Test Case: DashAbility_StopsAtCornerWall
Category: Functional
Priority: HIGH (directly tests the bug fix)
Setup:
  - Spawn character at position (100, 100, 0)
  - Place wall corner at (200, 200, 0) with 90-degree angle
  - Set dash distance to 300 units
Action:
  - Activate dash ability toward the corner
Expected:
  - Character position is NOT beyond the wall
  - Character position is at or near the wall surface
  - Dash ability reports completion (not cancellation)
  - No collision penetration detected
Why:
  - This is the exact scenario from GAME-1234
  - Without this test, the bug could regress
```

### Step 5: Prioritize Test Cases

Rank suggested tests by:

1. **Critical**: Tests the exact bug fix / feature change (must have)
2. **High**: Tests edge cases of the change (should have)
3. **Medium**: Tests related behaviors that might be affected (nice to have)
4. **Low**: Tests pre-existing untested behavior discovered during analysis (backlog)

## Output Format

```
## Test Gap Analysis

### Summary
- Functions modified: 4
- Existing tests found: 2
- Coverage gaps identified: 5
- Test cases suggested: 7

### Coverage Map

| Function | File | Existing Test | Status |
|----------|------|--------------|--------|
| SActionDash::ExecuteAction | SActionDash.cpp | TestDashAbility.cpp | PARTIAL - doesn't test corner collision |
| SActionDash::CalculateSweep | SActionDash.cpp | None | GAP |
| SCharacter::CanActivateAction | SCharacter.cpp | TestCharacterActions.cpp | COVERED |
| SActionComponent::StartAction | SActionComponent.cpp | TestActionComponent.cpp | COVERED |

### Suggested Test Cases

#### [CRITICAL] Test: DashAbility_StopsAtCornerWall
[full test case spec as above]

#### [CRITICAL] Test: DashAbility_SweepUsesCharacterCapsule
[full test case spec]

#### [HIGH] Test: DashAbility_CornerAngles_45_90_135
[full test case spec]

#### [HIGH] Test: DashAbility_StopsAtThinWall
[full test case spec]

#### [MEDIUM] Test: DashAbility_SlideAlongWallSurface
[full test case spec]

### Existing Tests to Update

#### TestDashAbility::TestBasicDash
- Current: Tests dash travels full distance in open space
- Update needed: Add assertion that dash respects collision in corner scenario
- File: Source/ActionRoguelike/Private/Tests/TestDashAbility.cpp:45

### Testing Effort Estimate
- Critical tests: ~30 minutes to implement
- High priority tests: ~45 minutes to implement
- All suggested tests: ~2 hours total
- Recommendation: Implement Critical + High before merge, Medium in follow-up
```

## UE5 Testing Patterns

When suggesting tests, use UE5 testing conventions:

**Automation Test (unit-level):**
```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDashCornerCollisionTest,
    "ActionRoguelike.Actions.Dash.CornerCollision",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDashCornerCollisionTest::RunTest(const FString& Parameters)
{
    // Setup, action, assertion pattern
    return true;
}
```

**Functional Test (requires game world):**
```cpp
UCLASS()
class AFunctionalTest_DashCorner : public AFunctionalTest
{
    // UE5 functional test with actual level geometry
};
```

## Integration with Workshop

- **Segment 3, Step 5**: After implementation, invoke test-gap-finder to see what tests are needed before asking @test-writer to create them
- **Segment 4**: Alongside @test agent to show comprehensive testing workflow
- **Pain Point 4.3**: Flaky tests - focused test suggestions reduce flakiness vs broad integration tests

Value proposition: "Stop guessing what tests to write. The test gap finder maps your exact changes to specific test cases, prioritized by importance. No more 'we'll add tests later.'"
