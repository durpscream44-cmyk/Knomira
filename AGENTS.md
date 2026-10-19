# Ingnomia Functional-Completion Codex Instructions

## Mission
Your job is to turn this Ingnomia fork into a dependable, functionally complete single-player colony simulator before adding major new features. Preserve the existing game and architecture wherever practical. Prioritize correctness, save/load integrity, reachable player-facing features, and reproducible builds over refactors or feature expansion.

## Definition of done
A release candidate must reliably support: new-game/world generation; colony setup; mining/chopping/hauling; stockpiles and construction; workshops/production; farming and animal care; food/water survival; pathfinding and job recovery; combat and deaths; trading/events that are exposed in the UI; repeated save/load during active simulation; and multi-season play without game-stopping corruption, deadlock, or crashes.

A visible feature must either work end-to-end or be clearly disabled/removed until implemented. Do not keep dead controls that pretend to work.

## Hard rules
1. Do not add multiplayer, caravans, expedition systems, or new dungeon systems during the completion pass.
2. Do not rewrite the engine or migrate frameworks.
3. Do not clear TODOs mechanically. Prove reachability/player impact first.
4. Make the smallest coherent change that fixes the verified problem.
5. Do not remove gameplay merely to make compilation/tests pass unless the ticket explicitly authorizes disabling an unfinished UI feature.
6. Preserve save compatibility when reasonably possible. If a save-format change is unavoidable, version it and document migration behavior.
7. Never silently discard invalid state on load. Log enough context to diagnose it and repair state deterministically when safe.
8. Treat item location, item claim/ownership, job reservation, equipment, container, stockpile, and carried-item state as integrity-critical.
9. Prefer deterministic tests. Any RNG-dependent test must use a fixed seed.
10. Do not perform broad formatting or unrelated refactors in a bug-fix commit.
11. Never commit generated binaries, local dependency folders, credentials, SDK secrets, or proprietary license files.
12. Before changing behavior, identify callers and explain why the code path is active.

## Required workflow for every ticket
### A. Investigate
- Read the ticket and relevant code.
- Search all callers/references before modifying a stub/TODO.
- Reproduce the problem where possible.
- Record whether the issue is CONFIRMED, NOT REPRODUCED, or DORMANT/UNREACHABLE.

### B. Plan
Before editing, state:
- root cause or current hypothesis;
- files expected to change;
- tests/reproduction that will prove the fix;
- save-compatibility implications.

### C. Implement
- Keep changes scoped to the ticket.
- Add assertions/invariants where they catch corruption near its source.
- Add automated tests when logic can be isolated; otherwise add a precise manual regression test.
- Update docs only when behavior/build instructions changed.

### D. Verify
At minimum:
- configure/build the affected target;
- run available automated tests;
- run the ticket's reproduction/regression steps;
- inspect logs for new warnings/errors;
- if save/load is involved, test both pre-save and post-load behavior.

### E. Report
Return:
1. status: PASS / PARTIAL / BLOCKED / NOT A BUG / DORMANT;
2. root cause;
3. exact files changed;
4. concise diff summary;
5. commands/tests run and results;
6. remaining risks;
7. recommended next ticket.

Do not claim a runtime fix based only on successful compilation.

## Branch/commit policy
Use one branch per ticket when practical: `completion/TXX-short-name`. Keep commits reviewable. Suggested commit prefix: `TXX:`. Do not combine multiple tickets unless one is a prerequisite and the report explicitly says so.

## Source-specific warnings from the audit
These are leads to verify, not permission to blindly edit them:

- Current CMake appears to require Qt6/C++23 while README/build instructions contain older Qt5-era guidance.
- `src/game/inventory.cpp` load/index reconstruction and cleanup paths deserve priority. Audit ranges around 313-439 and 1728-1768, plus job/death cleanup callers in `src/game/jobmanager.cpp` around 1184-1220 and `src/game/gnome.cpp` around 837-850.
- `src/base/io.cpp` around 605-615 contains a `loadConfig()` path that appears not to apply loaded values; determine intended semantics first.
- `src/game/farmingmanager.cpp` around 642-687 contains unfinished farm/grove/pasture priority accessors/setters used by `src/gui/aggregatoragri.cpp` around 266, 298, and 385.
- `src/gui/xaml/neighborsmodel.cpp` around 398-411 exposes Spy/Raid/Sabotage handlers that appear to be debug-only stubs; `src/game/neighbormanager.cpp` around 336 also marks an ambassador path as stub-like.
- `src/game/militarymanager.cpp` contains several incomplete squad/target/uniform hooks. Determine what is actually reachable before deciding to implement or hide controls.
- `src/base/pathfinder.cpp` has an empty `cancelRequest()` but no active caller was found in the static audit. Do not prioritize without evidence.
- `src/game/creaturefactory.cpp` has a random-monster factory returning null but no active caller was found. Treat as dormant unless runtime/code search proves otherwise.
- `src/game/canwork.cpp` contains no-op spell animation/cast functions while effect completion exists. Determine whether magic is player-visible/reachable before spending time on it.
- `DESIGN_ITEM_SYSTEM.md` is not a reliable implementation-status tracker. Validate the live code instead.

## Scope decisions
For the first completion release:
- Core colony simulation: REQUIRED.
- Save/load correctness: REQUIRED.
- Farming/workshops/stockpiles/construction: REQUIRED.
- Basic combat and death cleanup: REQUIRED.
- Existing market/trader loop if visible: REQUIRED.
- Advanced diplomacy actions: OPTIONAL; disable honestly if unfinished.
- Advanced squad targeting/uniform automation: OPTIONAL unless required by existing normal gameplay.
- Magic: OPTIONAL unless clearly exposed as a normal progression path.
- Multiplayer/caravans/new dungeon expedition gameplay: OUT OF SCOPE.

## Completion gate
Do not label the project functionally complete until a fresh checkout can be built using written instructions and a clean-install smoke test plus a long-session test have passed. Save/load must be tested during active jobs, carried items, agriculture, combat/death, and trader/event activity.
