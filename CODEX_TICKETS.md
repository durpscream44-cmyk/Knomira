# Ingnomia Functional-Completion Ticket Queue

Run these approximately in order. A ticket may be marked DORMANT/NOT A BUG after evidence; that is a valid result.

## T01 — Reproducible Windows build
**Priority:** P0

**Goal:** Produce a clean, repeatable Windows developer build without gameplay changes.

**Tasks:** Inspect `CMakeLists.txt`, CMake modules, CI workflows, submodules, content-fetch steps, Qt/Noesis/SFML/Steam dependencies, and compiler requirements. Compare them with README instructions. Build from a clean checkout. Create/update `BUILDING.md` with exact prerequisites and commands. Do not weaken compiler checks or delete features merely to compile.

**Acceptance:** A clean checkout can be configured, compiled, and launched by following `BUILDING.md`; required proprietary/external dependencies are clearly separated from repository content; all deviations from upstream docs are recorded.

## T02 — Baseline smoke-test harness/checklist
**Priority:** P0

**Goal:** Establish a known pre-fix baseline.

**Tasks:** Create `docs/SMOKE_TEST.md` covering new game, world generation, mining, hauling, stockpile, basic construction, workshop production, agriculture, save/load, combat/death, and market/trader if reachable. Capture reproducible failures and logs. Add lightweight automated startup/logic tests only where practical.

**Acceptance:** Another developer can follow the checklist and distinguish existing failures from regressions introduced later.

## T03 — Inventory save/load ownership-index reproduction
**Priority:** P0/P1

**Goal:** Prove or disprove the static ownership-index risk before modifying it.

**Focus:** `src/game/inventory.cpp` around load/add-object/index reconstruction and owner cleanup; relevant `ItemLocation`/`ItemClaim` code.

**Test scenarios:** Save while a gnome carries an item; save with an item claimed by a job; reload then cancel that job; save/reload then kill/remove the carrier through a safe debug/test path; verify items remain discoverable and claims release.

**Acceptance:** Automated or deterministic reproduction demonstrates current behavior. If no bug exists, document why indexes are reconstructed elsewhere and close as NOT A BUG.

## T04 — Fix inventory ownership reconstruction and invariants
**Priority:** P0/P1
**Dependency:** T03 confirmed problem.

**Goal:** Ensure location/claim owner indexes match item state after load and cleanup.

**Tasks:** Implement the narrowest correct reconstruction path. Expand `Inventory::sanityCheck()` to validate bidirectional ownership/index consistency, carried items, job claims, position indexes, equipment/container/stockpile references where architecture supports it. Prefer a single rebuild/validation function over duplicated index mutation.

**Acceptance:** All T03 scenarios pass before and after save/load; sanitizer detects deliberately corrupted test state; no duplicate/orphan ownership entries are introduced.

## T05 — Job cancellation/reservation cleanup
**Priority:** P1

**Goal:** Ensure canceled/invalid jobs return all reservations and resources.

**Focus:** `src/game/jobmanager.cpp`, including `giveBackJob()` and TODO concerning stockpile reservation; inventory claim cleanup.

**Scenarios:** Cancel hauling, construction, workshop, and stockpile-related jobs at multiple stages; remove destination/source while job is active; repeat after save/load.

**Acceptance:** No ingredient/item remains permanently reserved; replacement jobs can claim resources; no ghost job references remain.

## T06 — Death/removal item cleanup
**Priority:** P1

**Goal:** Make creature death/removal deterministic and recoverable.

**Focus:** `src/game/gnome.cpp` death cleanup, equipment/carried items, inventory owner indexes, active jobs.

**Scenarios:** Death while idle, carrying, equipped, performing a job, and after reload.

**Acceptance:** Carried/equipped items end in valid world/container states according to design; jobs/claims are released; no dangling owner IDs remain; save/reload after death succeeds.

## T07 — Save configuration semantics
**Priority:** P1

**Goal:** Resolve `IO::loadConfig()` ambiguity.

**Focus:** `src/base/io.cpp` around save/load config paths and all writers/readers of per-save configuration.

**Tasks:** Identify exactly what `config.json` stores and which values are expected to be per-save versus global. If per-save values are intended, implement restoration with validation/defaults. If obsolete, remove dead serialization expectations safely and document the behavior.

**Acceptance:** Every persisted field has a defined owner and round-trips correctly, or obsolete fields are deliberately retired without breaking existing saves.

## T08 — Agriculture priority system
**Priority:** P1

**Goal:** Make farm, grove, and pasture priorities truthful and functional.

**Focus:** `src/game/farmingmanager.cpp` around 642-687 and `src/gui/aggregatoragri.cpp` callers around 266/298/385.

**Tasks:** Verify UI reachability. Correct wrong farm/grove references. Implement getters/setters against authoritative data and verify scheduler behavior, or disable/remove controls if priority is not part of intended gameplay.

**Acceptance:** UI displays actual priority; changes persist and affect work selection as designed; farm/grove/pasture use the correct collections; save/load preserves values if applicable.

## T09 — Construction/stockpile/workshop end-to-end regression
**Priority:** P1

**Goal:** Validate the core economic loop after state-integrity fixes.

**Tasks:** Test mine/haul/store/build/deconstruct/craft/cancel flows. Specifically look for phantom constructed items, orphaned materials, inaccessible items, stale stockpile membership, and duplicate reservations. Use `DESIGN_ITEM_SYSTEM.md` only as a risk list, not implementation truth.

**Acceptance:** A scripted/manual colony workflow can repeatedly construct/deconstruct and produce goods across save/load without item loss/duplication/stuck reservations.

## T10 — Pathfinding and stuck-job recovery
**Priority:** P1

**Goal:** Prevent ordinary map changes from permanently wedging workers/jobs.

**Tasks:** Test doors, stairs/ramps, newly blocked paths, removed targets, changed designations, and unreachable jobs. Determine whether `PathFinder::cancelRequest()` is actually needed. Do not implement dormant APIs without a caller.

**Acceptance:** Actors abandon/requeue invalid work safely; the job system continues progressing; no unbounded stale path requests are observed in tested scenarios.

## T11 — Combat and hostile-encounter completion pass
**Priority:** P1

**Goal:** Ensure existing normal combat can start, resolve, and clean up safely.

**Tasks:** Identify the currently reachable hostile-spawn/event route. Test targeting, damage, incapacitation/death, drops, job interruption, and save/load during/after combat. Do not revive `createRandomMonster()` solely because it is a stub if nothing uses it.

**Acceptance:** At least one normal player-facing hostile encounter works end-to-end without corrupting colony state.

## T12 — Military UI reachability and scope decision
**Priority:** P1/P2

**Goal:** Stop incomplete military features from presenting as functional.

**Focus:** `src/game/militarymanager.cpp` stubs and all UI callers.

**Tasks:** Map every visible military control to its implementation. For required base-game actions, implement the smallest reliable behavior. For advanced unreachable/incomplete targeting/uniform features, disable/hide them with an explanatory tooltip/text rather than inventing a large new subsystem.

**Acceptance:** Every visible military action has a tested effect; no normal UI control silently no-ops.

## T13 — Diplomacy/neighbor dead-control cleanup
**Priority:** P2

**Goal:** Make neighbor/diplomacy UI honest.

**Focus:** `src/gui/xaml/neighborsmodel.cpp` Spy/Raid/Sabotage handlers; ambassador path in `src/game/neighbormanager.cpp`; corresponding XAML/UI.

**Tasks:** Verify visibility/reachability. If these actions lack a complete simulation path, disable/hide them for the completion release. Do not build a new diplomacy game unless existing design/data clearly supports a bounded implementation.

**Acceptance:** No visible diplomacy button only prints debug text or otherwise pretends to execute an action.

## T14 — Trader/market reliability
**Priority:** P1

**Goal:** Verify the existing trade loop as part of the finished base game.

**Tasks:** Test market stall creation/removal, trader arrival/departure, trading, cancellation, save/load while trader is present, and destruction/reconfiguration of market structures. Check historical crash-prone transitions but rely on current reproduction, not old issue text alone.

**Acceptance:** Trader lifecycle and trade transaction complete without crashes, orphaned references, or save corruption; if a nonessential subfeature cannot be completed, it is clearly disabled.

## T15 — Magic/spell reachability decision
**Priority:** P2

**Goal:** Decide whether magic belongs in the completion release.

**Focus:** `src/game/canwork.cpp` cast/cast-animation no-ops and effect completion path; UI/content references.

**Tasks:** Determine whether a normal player can reach/use magic. If yes, trace one spell end-to-end and finish the minimum execution path required for correctness. If no, classify as dormant/future and ensure UI/content does not falsely advertise it.

**Acceptance:** Either one documented, tested player-facing magic path works correctly or magic is explicitly out of scope and inaccessible for this release.

## T16 — UI dead-control sweep
**Priority:** P1/P2

**Goal:** Ensure all ordinary player-visible controls have meaningful behavior.

**Tasks:** Walk every major screen/menu. Trace buttons/commands to handlers. Flag debug-only, empty, null-return, or stub handlers. Fix small bounded cases; disable/hide larger unfinished systems. Do not equate every null/empty function with a bug unless reachable.

**Acceptance:** A manual UI sweep finds no ordinary control that silently does nothing.

## T17 — Save/load stress matrix
**Priority:** P0 release gate

**Goal:** Validate persistence under active simulation.

**Matrix:** active hauling; active construction; active workshop job; growing crops; animals; equipped/carried items; job claims; combat/death; trader present; paused/unpaused; multiple consecutive saves/reloads.

**Acceptance:** State before save and after load is semantically equivalent; no invisible/orphaned/duplicated items, stuck claims, dead jobs, invalid owner IDs, or unloadable saves.

## T18 — Long-session stability and release candidate
**Priority:** P0 release gate

**Goal:** Decide whether the game is functionally complete enough to release.

**Tasks:** Start from a clean new colony and play/simulate through several in-game seasons. Grow population/economy, use agriculture/workshops/trade, experience combat, save/reload repeatedly, and inspect logs/performance. Fix only release-blocking regressions discovered here; file nonblocking polish separately.

**Acceptance:** No crash, unloadable save, runaway degradation, permanent colony-wide AI stall, or core-loop blocker. Produce `docs/RELEASE_TEST_REPORT.md` with build hash, test environment, scenarios, failures, and final PASS/FAIL recommendation.

# After T18
Only after the release gate passes should the project open separate feature epics for dungeons/expeditions, overworld caravans, and host-authoritative co-op multiplayer.
