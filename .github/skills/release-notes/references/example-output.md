# Example Release Notes Output

Realistic examples of the release-notes skill generating audience-specific notes for a UE5 game project sprint.

---

## Example 1: Producer Summary (Format C)

## Sprint 52 Summary — Project Titan

### Velocity
- Planned: 38 story points | Delivered: 35 story points (92%)
- Tickets resolved: 17 | Carried over: 3
- Sprint goal: "Stabilize combat system for internal playtest" — **ACHIEVED**

### Highlights
- **Combat hit registration rewrite** — resolved top 3 player-reported bugs from last playtest. Hit detection now uses server-authoritative sweep with client-side prediction. QA confirmed 0 missed hits in 4 hours of testing.
- **Enemy AI flanking behavior shipped** — design milestone requirement for M3 demo. AI squads now coordinate flanking maneuvers using EQS scoring. Playtest feedback: "enemies feel much smarter."
- **Memory optimization pass on Level_03** — reduced peak memory from 4.8GB to 3.9GB by converting 200+ static meshes to instanced static meshes. Brings us under the 4GB PS5 budget.
- **Cooldown visualization on HUD** — new radial cooldown indicator on ability bar. Art team approved final visual.

### Agent Productivity
- 5 of 17 tickets handled autonomously by @copilot (29%)
- Tickets handled by agent:
  - [TITAN-2045] Fix null pointer in AInventoryComponent::DropItem (P2, 18 min)
  - [TITAN-2051] Add missing UPROPERTY on cached weapon reference (P3, 12 min)
  - [TITAN-2058] Fix TArray reallocation in damage calculation loop (P2, 34 min)
  - [TITAN-2062] Update copyright headers across 47 source files (Task, 8 min)
  - [TITAN-2070] Add unit test for ammo deduction edge case (Task, 25 min)
- Agent average turnaround: 19 minutes vs human average 5.8 hours
- Agent-created regressions: 0
- All agent PRs passed code review on first submission

### Risk Items
- **[P1] Multiplayer desync on ability activation** — still open, networking team investigating. Blocks multiplayer playtest scheduled for Sprint 53.
- **Certification deadline in 4 sprints** — 9 cert-blocking bugs remain (down from 14 last sprint)
- **Audio middleware upgrade** — Wwise 2024.1 upgrade deferred to Sprint 53. Risk: new Wwise version required for PS5 certification.

### Next Sprint Focus
1. P1 multiplayer desync (top priority, assigned to networking lead)
2. Wwise 2024.1 upgrade (audio engineer + build team)
3. Begin certification pre-check: TRC/XR compliance audit
4. Performance pass on Level_04 (currently 8ms over GPU budget)

### Build Info
- Build: 5.2.891 | Branch: main | CL: #52340
- Platforms validated: PC (Win11), PS5 devkit
- Automated test pass rate: 97% (2 flaky tests under investigation)

---

## Example 2: QA Build Notes (Format B)

## QA Build Notes: 5.2.891 — Sprint 52 Drop

**Build Date**: 2024-06-14  
**Branch**: main @ CL #52340  
**Platforms**: PC (Windows 11 Development), PS5 Development Kit  
**Config**: Development build, logging enabled, stat overlays available

---

### MUST TEST — Bug Fixes (P1/P2)

#### 1. [TITAN-2038] Hit registration misses at close range
- **What changed**: Rewrote melee hit detection to use server-authoritative capsule sweep instead of client-only line trace. Added 50ms reconciliation window for latency compensation.
- **Test steps**:
  1. Enter TestMap_Combat, spawn 3 AI enemies
  2. Attack each enemy at point-blank range (<1m) with sword — expect 100% hit rate
  3. Attack while strafing left/right — expect hits to register within 1 swing
  4. Attack while both player and enemy are moving — verify no phantom misses
- **Regression areas**: Ranged weapon hit detection (should be unaffected — different code path). Verify ranged headshots still register correctly on TestMap_Shooting.
- **Maps**: TestMap_Combat, TestMap_Shooting, Level_02 (arena encounter)

#### 2. [TITAN-2041] Player gets stuck in dash state after hitting sloped geometry
- **What changed**: Added slope-angle check in `SActionDash::OnDashComplete`. If character is on slope > 45°, force-exit dash state and apply small upward impulse to prevent embedding.
- **Test steps**:
  1. Load TestMap_Terrain, find the steep ramp section (marker "SLOPE_TEST")
  2. Dash into the ramp at various angles (head-on, 45°, glancing)
  3. Dash off the top of the ramp into open air — should land cleanly
  4. Dash into the V-shaped crevice at marker "CREVICE_TEST" — should not get stuck
- **Regression areas**: Normal dash on flat ground, dash through doorways, dash cancel into attack
- **Maps**: TestMap_Terrain, Level_01 (outdoor area with hills), Level_03 (indoor ramps)

#### 3. [TITAN-2058] Damage calculation overflow when hitting large enemy groups
- **What changed**: Pre-allocated damage result array with `Reserve(MaxTargetsPerSwing)` and added bounds check. Previously, the TArray would reallocate during the damage loop, and in rare cases with 8+ targets, memory corruption caused damage values of 0 or MAX_INT.
- **Test steps**:
  1. TestMap_Combat: spawn 10 enemies in tight cluster
  2. Use AoE attack (heavy slam) — all enemies should take correct damage (verify HP bars)
  3. Repeat 20 times to check consistency
- **Regression areas**: Single-target damage, damage-over-time effects, shield damage reduction
- **Maps**: TestMap_Combat

---

### SHOULD TEST — New Features

#### 4. [TITAN-2044] Enemy AI flanking behavior
- **What changed**: New EQS query `EQS_FlankPosition` scores positions behind the player. AI squads of 3+ will send one member to flank while others suppress.
- **Test steps**:
  1. TestMap_AI: trigger squad encounter (3 enemies)
  2. Take cover behind wall — observe: 2 enemies should suppress (shoot at cover), 1 should move to flank position
  3. Verify flanking enemy doesn't path through walls or get stuck
  4. Kill flanking enemy — another should attempt to flank after 5s delay
- **Edge cases**: Player moving rapidly (AI should re-evaluate flank position), player in open area (AI should still attempt flanking from behind), single enemy remaining (should not try to flank alone)
- **Maps**: TestMap_AI, Level_02 (arena with cover objects)

#### 5. [TITAN-2060] Cooldown visualization on ability bar HUD
- **What changed**: New radial fill widget on each ability slot. Shows remaining cooldown as a clockwise sweep + seconds remaining text.
- **Test steps**:
  1. Activate each ability (Dash, Shield, Heavy Attack, Ultimate)
  2. Verify cooldown circle appears and counts down correctly
  3. Verify text shows seconds remaining (rounds to nearest integer)
  4. Activate multiple abilities rapidly — all should show independent cooldowns
  5. Die and respawn — cooldowns should reset
- **Edge cases**: Cooldown reduction buff active (circle should animate faster), ability interrupted mid-cast (cooldown should or should not apply per design doc)
- **Maps**: Any gameplay map

---

### KNOWN ISSUES — Not Fixed This Build

| Ticket | Priority | Description | Status |
|--------|----------|-------------|--------|
| TITAN-2033 | P1 | Multiplayer desync on ability activation | In progress — networking team |
| TITAN-2055 | P2 | Audio stutter when >20 enemies on screen | Blocked on Wwise upgrade |
| TITAN-2063 | P3 | Minimap icons offset by ~10px after respawn | Scheduled Sprint 53 |
| TITAN-2067 | P3 | LOD pop-in visible on Level_04 mountain range | Art team adjusting LOD distances |

---

### Agent-Created Changes in This Build

The following fixes were implemented autonomously by @copilot. They passed automated tests and human code review, but please include them in your test coverage:

| Ticket | Description | Agent Time | Reviewer |
|--------|-------------|------------|----------|
| TITAN-2045 | Null pointer fix in DropItem | 18 min | @senior-dev |
| TITAN-2051 | Missing UPROPERTY on weapon ref | 12 min | @lead-dev |
| TITAN-2058 | TArray realloc in damage calc | 34 min | @senior-dev |

---

### Build Artifacts
- **PC**: `\\build-server\drops\Titan_5.2.891_PC_Dev.zip`
- **PS5**: `\\build-server\drops\Titan_5.2.891_PS5_Dev.pkg`
- **Symbols**: `\\build-server\symbols\5.2.891\`
- **Crash reporter**: Enabled, reports go to #titan-crashes Slack channel
