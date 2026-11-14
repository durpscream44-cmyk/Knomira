# Knomira — Codex development handoff

**Last reviewed:** 2026-11-14  
**Working branch for next playable-first ticket:** `feature/playable-config-startup-smoke` ([draft PR #12](https://github.com/durpscream44-cmyk/Knomira/pull/12)); stacked over PR #11 → #10 → #8. Do not treat any draft branch as merged.  
**Status:** UI shell and original IO/Config startup smoke tests pass in Windows CI; real `GameManager`/simulation **does not yet run inside the Noesis-free executable**. Keep the mission **playable -> stable -> optimized**.

This file is a durable "where to pick up" checkpoint for Codex or a local desktop developer. It records what was confirmed, what was only compiled, what is untested, and the next safe work units. Read the root [AGENTS.md](AGENTS.md), [prototype/rmlui/AGENTS.md](prototype/rmlui/AGENTS.md), and [Noesis-free migration plan](prototype/rmlui/NOESIS-FREE-PLAN.md) as well. **Inspect current code, branches, Actions and the local working tree again** rather than treating this document as proof of live functionality.

## 1. Verified status (as of this review)

| Area | Status | What was verified / not verified |
|---|---|---|
| Qt/RmlUi shell, menus, keyboard and Xbox input code | **CI pass** | Windows builds and automated input tests. Physical mouse/controller behavior still needs real-desktop confirmation, including binding click vs scrolling. |
| Original save-slot discovery | **CI pass** | The shell reads original Ingnomia save *metadata* without loading the game simulation. It must not write to player saves merely to browse them. |
| Original startup IO and Config execution | **CI pass (link + run)** | [Windows run 43580189380](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43580189380) passed linked tests that invoke original `IO::getDataFolder()`, `IO::createFolders()`, `IO::loadFile()`, `IO::loadOriginalConfig()`, and actual `Config::Config()` in a marked disposable temp directory; no normal saves touched. |
| Real C++ simulation source with Noesis removed from compilation | **CI pass, compile-only** | Latest [Windows run 43561481732](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43561481732) built `knomira_simulation_source_compile`, including `Game`, `GameManager`, `NewGameSettings`, `TechTree`, `PathFinder`, and `EventConnector`. This does **not** establish a fully linked simulation or playable world. |
| World generation, gnome AI in the RmlUi shell, actual map renderer, New Game/Load/Save | **Not integrated** | Core blockers for a playable game. |
| Legacy world mouse-click diagnostic fix | **Draft, not verified** | [PR #9](https://github.com/durpscream44-cmyk/Knomira/pull/9) is based on `feature/controls-menu-redesign`; no original-game desktop result. |
| Inventory search optimization, debug profiler, tick-conveyor standalone test | **Mixed: isolated CI passes** | [PR #7](https://github.com/durpscream44-cmyk/Knomira/pull/7), Windows run [43560407808](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43560407808). Conveyor tests pass but there are no live conveyors. Original engine integration, saves and UPS effects are unverified. |
| Instant inlet/outlet workshop logistics | **Design only** | Preferred over physical conveyor-belt animation; do **not** implement before basic stockpile/workshop gameplay works. |

### Branch/PR map — do not assume merged

- [PR #6](https://github.com/durpscream44-cmyk/Knomira/pull/6): `feature/controls-menu-redesign` -> `feature/remappable-controls` (**draft**). Controls UI, layout tests and an unresolved need for physical mouse verification.
- [PR #8](https://github.com/durpscream44-cmyk/Knomira/pull/8): `feature/playable-engine-compile-gate` -> `feature/controls-menu-redesign` (**draft; next playable integration path**).
- [PR #9](https://github.com/durpscream44-cmyk/Knomira/pull/9): `feature/map-mouse-routing-diagnostics` -> `feature/controls-menu-redesign` (**draft; separate legacy window diagnostics**).
- [PR #7](https://github.com/durpscream44-cmyk/Knomira/pull/7): `feature/tick-conveyor-prototype` -> `feature/controls-menu-redesign` (**draft; defer feature integration**).

These are **sibling branches** except for the older controls PR dependency. Neither #7 nor #9 is automatically included in #8. Rebase/cherry-pick only after selecting and testing the exact desired changes.

## 2. Top three blockers to playable

**A. Link and initialize the actual simulation (highest priority).**
`prototype/rmlui/qt_host.cpp` currently owns the Noesis-free Qt/RmlUi menu shell. It has no functioning `GameManager`-backed world. The original `src/game/gamemanager.cpp` controls lifecycle and initializes `EventConnector` and `NewGameSettings`; the existing `Game` has simulation threading and manager dependencies. The compile-only object target is the *start* of separating these from the Noesis-based root `src/CMakeLists.txt`, not the end. Audit constructors, ownership, required Config/DB/IO/sprite/sound dependencies, Qt signals and the main-thread/game-thread boundary before linking an executable.

**B. Render a real world beneath RmlUi.**
Preserve the original `src/gui/mainwindowrenderer.*` and the game's GLAD/OpenGL resources. Connect it to the existing Qt OpenGL context; render world -> RmlUi overlay -> swap. Do not create a second GL context or rewrite all rendering systems at once. Test resize, camera, zoom, z-level and OpenGL state restoration.

**C. Wire real gameplay commands and safe persistence.**
New Game must use the original settings/world generation. Load must validate the chosen slot, then deserialize actual state. Pause/resume must control the game thread. Game-world mouse clicks must reach the selection aggregator (not be swallowed by the UI), and an initial dig/stockpile/workshop command must result in an actual gnome job. Save/exit/relaunch/load must preserve worker state, jobs, items and structures.

## 3. Proposed next Codex tickets (small, testable, sequential)

### PLAY-01 — Linkable engine bootstrap smoke target

1. On `feature/playable-engine-compile-gate`, review `prototype/rmlui/CMakeLists.txt`, original `src/CMakeLists.txt`, `src/game/gamemanager.*`, `src/game/game.*`, `src/gui/eventconnector.*`, `src/base/config.*`, `src/base/db.*`, and `src/base/io.*`.
2. Extend the explicit Noesis-free source target **incrementally**, keeping the original legacy target untouched. Link an engine bootstrap/smoke executable (or equivalent real service target) rather than writing fake placeholders that look successful.
3. Verify safe Config/DB/resource initialization; no game save is created or overwritten just by starting the app. Keep any hidden data/bootstrap dependencies explicit.
4. CI acceptance: fresh Windows CMake build links the real target without Noesis PCH/NoesisSDK; smoke-test startup and cleanup succeed; existing shell/input tests still pass.
5. Document unresolved external Steam/OpenAL/renderer dependencies if linking the full engine is not yet possible. Mark **PARTIAL**, not PASS, if only source files compile.

### PLAY-02 — Real New Game/Load lifecycle

1. Expose original `NewGameSettings` through a minimal RmlUi New Game flow; use existing world generation and `GameManager`, not a dummy in-memory map.
2. Distinguish read-only save discovery from actual load. Require explicit user action before writing save data; test with copies of example saves, never the sole original.
3. Verify start -> pause -> resume -> stop and startup failure handling on the correct Qt thread; avoid zombie timers and dangling game pointers.
4. CI/manual acceptance: new world produces real tiles/gnomes, repeatable startup, no silent crashes; save/load verified before declaring it usable.

### PLAY-03 — World view and interaction

1. Reuse original world renderer with Qt/RmlUi viewport. Test render ordering and GL state.
2. Route world mouse press/move/release/wheel to `AggregatorSelection` when not over actual UI controls, preserving keyboard/controller navigation.
3. Test left-click selecting a tile, drag, right-click cancel, camera pan, zoom and z-level change, including high-DPI coordinates. An invisible full-screen UI panel must not block the world.
4. Make one normal designation (e.g. digging) produce an actual job, then observe a gnome fulfill it. This is the first persuasive gameplay milestone.

### PLAY-04 — Save/load and minimal colony HUD

Enable a small set of actual actions, not fake buttons: basic designations, stockpile, one workshop, inventory view, save and load. The first playable criterion is **create colony -> give job -> gnome works -> save -> quit -> reload -> continue**. Add longer-session simulation regression tests after that.

### INPUT-01 — Verify mouse failure, without broad speculative fixes

- The user reported a visible map where keyboard worked but mouse clicks did not, **but has not identified which executable**. The current independent `knomira_game_shell` has no playable map.
- Inspect [PR #9](https://github.com/durpscream44-cmyk/Knomira/pull/9) and [MAP_MOUSE_DEBUGGING.md on its branch](https://github.com/durpscream44-cmyk/Knomira/blob/feature/map-mouse-routing-diagnostics/MAP_MOUSE_DEBUGGING.md).
- In the legacy app `src/gui/mainwindow.cpp`, `mouseReleaseEvent` previously forwarded the last hover `m_mouseX/m_mouseY` to world selection. PR #9 uses the current Qt-local release position, clears a possibly stuck right-button state, and adds `KNOMIRA_MOUSE` traces. These are plausible improvements, **not a proven full fix**.
- Enable debug mode with Ctrl+O in the legacy world window and identify whether clicks reach `MainWindow`, pass `isOverGui`, reach `AggregatorSelection::onLeftClick`, then resolve to the intended tile. Inspect Noesis hit-test overlays if `overGui` is unexpectedly true.
- Test with an actual Noesis-capable legacy build if available; otherwise leave this branch as **BLOCKED: cannot reproduce in legacy runtime**. When RmlUi world integration exists, reproduce under the new viewport instead.
- Separately confirm Controls UI binding fields open capture on a single mouse click; passing a layout hit-test in CI is not sufficient.

## 4. What stays parked

- PR #7: any-material inventory octree early-exit, debug `KNOMIRA_PROFILE` timings, standalone tick transport. Rebase only selected proven low-risk improvements after actual simulation profiling and legacy game compilation. Confirm save/item/claim correctness.
- Instant inlet/outlet stockpile logistics: [DESIGN_INLET_OUTLET_LOGISTICS.md on its branch](https://github.com/durpscream44-cmyk/Knomira/blob/feature/tick-conveyor-prototype/DESIGN_INLET_OUTLET_LOGISTICS.md). No animated belts required; transactions must retain real item IDs, filters, atomic reservations and bounded transfer rate. Not a blocker to first playable colony.
- Preferred paths/military patrol sharing, workshop disruption alerts, large-population tuning, elaborate mod API, and Ages/Relics/legacy systems: defer until functioning game core; profile before changing simulation behavior. Military formations were explicitly removed from scope.

## 5. Local setup and verification notes (Windows)

Existing development machine was reported to have VS2022, CMake 3.30, Qt 6.12 `msvc2022_64`, Git and GitHub CLI. Verify actual paths; do not assume they exist on a fresh machine.

```powershell
cd C:\Users\durps\Knomira
git status --short --branch
git fetch origin
git switch feature/playable-engine-compile-gate
git pull --ff-only
# Do NOT overwrite any dirty worktree or overwrite the old game's saves.
# Inspect the exact CMake flags/CI workflow before configuring locally.
powershell -NoProfile -ExecutionPolicy Bypass -File .\prototype\rmlui\Test-GameShell.ps1
```

Use `-Install` only if authorized to replace the shell build, after verifying matching CI SHA. The test script's shell checks do not launch real colony simulation. The existing read-only kingdom catalog looks under `Documents\My Games\Ingnomia\save`; never delete or manipulate the user's original saves. Store logs and screenshots in the ignored build directory without credentials/other app contents.

Useful verification:
- [Engine gate CI run](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43561481732): completed **success**, including compile-only real engine object target.
- [Standalone transport CI run](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43560407808): completed **success**, but no live inventory transfer.
- Avoid saying map click, live world, or save/load works until tested **in the actual relevant executable**.

## 6. Handoff record template (update this file after each ticket)

- **Date and branch / PR / commit:** 
- **Ticket, tested behavior, code-path root cause:**
- **Files changed:**
- **Build commands + CI run, conclusion, exact failures:**
- **Manual test result:** PASS / PARTIAL / BLOCKED / NOT REPRODUCED
- **Save compatibility / test save used:**
- **What remains broken / what was not tested:**
- **Next single bounded coding task:**

When uncertain, leave a concrete blocker and reproduction command. **A compile-only build success is not a playable game.** No async tasks or work-in-progress promises: a ticket is complete only when the changes, tests, and outstanding blockers are recorded.

## 7. Latest PLAY-01B preparation: fail-closed test save-root isolation (2026-11-14)

**Work branch:** `feature/playable-safe-data-root`, [draft PR #11](https://github.com/durpscream44-cmyk/Knomira/pull/11), based on draft PR #10 (`feature/playable-db-bootstrap-smoke`), itself based on draft PR #8 (`feature/playable-engine-compile-gate`). None is automatically merged; check actual branch ancestry before cherry-picking.

**Why this matters:** `IO::getDataFolder()` in `src/base/io.cpp` normally returns `Documents/My Games/Ingnomia` on Windows. Original `Config::Config()` invokes `IO::createFolders()`, which can write to the configured data path. Before a linked `GameManager` smoke test, the real `IO` translation unit must be forced to use disposable test data or it risks touching existing player saves/settings.

**Implemented on PR #11:**
- `src/base/io_data_root.h`: one shared resolver called from the real `IO::getDataFolder()`.
- Without `KNOMIRA_ISOLATED_TEST_DATA_ROOT`, it returns the **same OS-specific player save folder as before**. An environment variable alone does not redirect normal gameplay.
- With that compile definition, `KNOMIRA_TEST_DATA_ROOT` is mandatory; it must refer to a real absolute directory **strictly inside the canonical system temp folder**, containing the `.knomira-isolated-test-root` marker. Missing, invalid, unmarked, or non-temp roots cause an immediate fatal failure, never a fallback to live saves.
- `knomira_simulation_source_compile` is a **smoke-only** target and is compiled with the isolation definition, so it can later provide objects for an isolated engine bootstrap. **Never attach this define to the shipping legacy game or normal `knomira_game_shell`.**
- `prototype/rmlui/io_data_root_test.cpp` is compiled twice: isolation mode tests rejection, temporary-file writes and root selection; production mode tests the unchanged default while the override environment variable is set. Windows CI targets are `knomira_io_data_root_test` and `knomira_io_data_root_production_test`.
- Original game data/schema and save formats remain unchanged. The new test does not load or alter player saves.

**Immediate next ticket: PLAY-01B linkable engine bootstrap** (not implemented): initialize `Config`, `DB::init()`, `DB::initStructs()`, `Strings::init()`, and then construct/destroy `GameManager` using the **test-compiled isolated IO source**, a deliberately created `QTemporaryDir` marker, and an explicit `KNOMIRA_TEST_DATA_ROOT`. Do not reuse any unguarded shipping `IO` implementation in the same executable. If the test root is missing or not validated, hard fail before constructing `Config`. Prefer `QApplication` with offscreen platform for legacy utility classes, and use orderly Qt thread quit/wait. GameManager still has extensive unresolved linker/aggregator/sprite/sound dependencies, so stop at the first tractable CI blocker and document it rather than faking lifecycle success.

**Verification — PASS:** [Noesis-free Windows CI run 43578230240](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43578230240) built both test targets and passed the `Verify isolated game data root cannot target player saves` step. The real engine sources also compiled. **Historical note (superseded):** the earlier test exercised only the shared resolver. PR #11 later linked the real `IO::getDataFolder()` and `IO::createFolders()` in a safe standalone test; PR #12 added the actual Config constructor. None of these tests link an actual `GameManager` yet.

**Additional lifecycle finding for PLAY-01B (source review only):** `src/game/gamemanager.cpp` assigns `Global::util = new Util(nullptr)` in the constructor and later replaces it via `new Util(m_game)` in both new-game and load paths, as well as a fresh `new Util(nullptr)` in `endCurrentGame()`. `Util` is not a QObject and has a trivial destructor; the displaced pointers are not explicitly freed in these paths. Audit `Util` and `Selection` ownership and shutdown on the real game thread before adding another manager lifecycle. Do not blindly `delete` objects during a running simulation; scope this as a separate measured, testable fix after smoke initialization.

**Acceptance checklist for Codex before declaring this ticket done:** Windows CI passes both root-mode tests and original source compile; no real save/settings path writes; PR #11 remains draft until review; the next linked engine startup test must assert exactly which paths were written inside the temporary root; only call a game playable after a world is generated, rendered, given a genuine job, and save/reload works.


## 8. Latest verified linked startup work: original IO + Config (2026-11-14)

**Dependency stack and PRs:** [#8 engine compile](https://github.com/durpscream44-cmyk/Knomira/pull/8) → [#10 packaged SQL/assets](https://github.com/durpscream44-cmyk/Knomira/pull/10) → [#11 real IO sandbox bootstrap](https://github.com/durpscream44-cmyk/Knomira/pull/11) → [#12 original Config runtime bootstrap](https://github.com/durpscream44-cmyk/Knomira/pull/12). All are **draft/unmerged** and must be merged/rebased in dependency order only after review.

- **PR #11 / PASS:** [Windows CI run 43579520278](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43579520278) compiled the engine and linked actual `IO::getDataFolder()` and `IO::createFolders()` into two runtime tests. The isolated mode creates mods/save/screenshots/settings/tmp and copies real bundled default settings into `QTemporaryDir`, preserves user-edited sandbox config across repeated folder setup, and tests production path parity without writing actual saves. These methods were *moved*, not functionally rewritten, to `src/base/io_data_root.cpp`. Root `src/CMakeLists.txt` uses `file(GLOB_RECURSE)` for sources, so **reconfigure CMake locally after new .cpp files are pulled**; the CI build uses fresh configure.
- **PR #12 / PASS:** [Windows CI run 43580189380](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43580189380) compiled the expanded source target, linked the **original** `Config::Config()` into `knomira_engine_config_bootstrap_test`, and passed the runtime check for `Config::valid()`, defaults, the packaged content path, actual JSON loading and temp-only files. To make linkable without pulling in all world/save symbols, moved original `IO::loadFile()` and `IO::loadOriginalConfig()` into `src/base/io_settings_file.cpp` and original `Config::set()` into `src/base/config_persistence.cpp`, leaving original method bodies/functionality intact. Both new sources are included in the real engine compile gate; the root build auto-discovers them on fresh configure.
- **No personal-save writes:** these linked smoke tests compile with `KNOMIRA_ISOLATED_TEST_DATA_ROOT`; they require an absolute marked temp sandbox. Production data root path and save file formats are unchanged. The normal `knomira_game_shell` does not enable the test override. The test writes only in its disposable temp directory.
- **Still not running:** original `DB::init()`, `DB::initStructs()`, `Strings::init()` and `GameManager` have not been linked/executed together with this Config test. There is no generated world, world renderer, playable colony or live save/reload in the Noesis-free shell. Do not describe the Config smoke as a playable game.

### Next single bounded task — PLAY-01C

1. On `feature/playable-config-startup-smoke`, inspect `src/base/db.cpp`, `src/base/global.cpp`, `src/gui/strings.cpp`, `src/main.cpp`, `src/game/gamemanager.cpp`. Original startup order from `src/main.cpp`: `Global::cfg = new Config` (verify valid) → `DB::init()` → `DB::initStructs()` → `S::gi().init()` → `GameManager` creation. `Config::set("CurrentVersion")` writes settings and should be skipped for the initial smoke unless its persistence is linked and sandbox-protected.
2. Build a **small linked real DB/Strings bootstrap** using the original `Config` and safe IO paths, then verify actual DB rows/cache and translation string lookup; avoid mock DB managers or writes outside marked `QTemporaryDir`. Report the first unresolved symbols and trace their real source ownership.
3. Only after this succeeds, attempt constructing/destroying `GameManager` and its actual `EventConnector`/aggregators in isolation. The constructor currently creates many GUI aggregator objects plus global Util/NewGameSettings, so expect broad dependencies; do not fake a usable game.
4. Thread teardown: old `src/main.cpp` uses `gameThread.terminate()` after `app.exec()`. The new shell must use orderly queued shutdown + `quit()`/`wait()`, with explicit QObject ownership and cleanup, instead of copying the force-termination behavior.
5. Audit `Global::util` and `Global::sel` ownership before repeated start/stop; they are assigned/replaced by raw pointers in GameManager, and leaked/dangling globals are possible. Verify with actual lifecycle tests rather than changing ownership blindly.
6. **Acceptance:** fresh Windows build links/runs original DB/Strings bootstrap using temporary files, verifies zero real-save writes, and existing UI/input tests pass. Update this handoff with exact CI SHA/run and blockers.

**Short Codex pickup instruction:** `git fetch origin; git switch feature/playable-config-startup-smoke`; read `AGENTS.md`, `CODEX_HANDOFF.md`, and PR #12; verify `git status` first and rerun CMake configure because of new `.cpp` files. Work on PLAY-01C only. Do not claim GameManager/world rendering is already integrated.

## PLAY-01B2 verified DB/Strings runtime milestone and branch reconciliation — 2026-11-14

**Canonical integration branch:** feature/playable-db-services-after-config, [draft PR #14](https://github.com/durpscream44-cmyk/Knomira/pull/14), based on draft PR #12 (feature/playable-config-startup-smoke), which in turn builds on PR #11 and PR #10/#8.

**PASS:** [Windows Noesis-free run 43587102028](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43587102028). All build and regression steps passed, including "Initialize original DB and Strings services in isolated runtime". The test links the genuine Config, DB::init(), DB::initStructs(), and Strings::init() implementations with the original staged game SQL. It confirms nonempty Items, Jobs and Translation tables; cached job and workshop lookups; an actual translated string lookup; and that no save was generated in the marked temporary test root. Existing shell, input, IO-root, Config constructor and SQL fixture tests also passed.

**Branch correction:** A parallel draft PR #13 performed similar DB startup tests but inherited a competing Config-JSON refactor. It was CLOSED AS SUPERSEDED by PR #14. Later Config changes accidentally added to PR #11 were backed out, restoring PR #11 to its original test-root and real IO::createFolders protection scope. PR #12 remains the single canonical Config startup extraction. Do not port io_config.cpp or the old PR #13's changed base into this branch; that would duplicate IO::loadFile and IO::loadOriginalConfig definitions.

**Next exact playable-first ticket:** Real GameManager startup/shutdown in a headless/offscreen sandboxed process. Audit EventConnector constructor and its aggregator dependencies, NewGameSettings, GameState and Util ownership before linking. Do not invoke the old src/main.cpp force-terminate shutdown logic. Separate the GUI overlay bridge from simulation startup only as needed; avoid fake manager stubs. Test actual Config -> DB -> Strings -> EventConnector/GameManager object construction and cleanup without creating a save. Then attach actual New Game/world renderer and real tile selection.

**Status boundaries:** CI proves a linked original data/translation service, not a linked GameManager, not moving gnomes, not a rendered map, and not playable save/load. Keep the draft PR chain in dependency order and test on Windows after each merge.


## PLAY-01B2 gameplay-data correctness audit — PR #15 (2026-11-14)

**Branch:** `feature/fix-job-cache-task-scope` ([draft PR #15](https://github.com/durpscream44-cmyk/Knomira/pull/15)), based on the canonical DB/Strings bootstrap [PR #14](https://github.com/durpscream44-cmyk/Knomira/pull/14). **This patch is a sibling follow-up, not yet merged.**

**Source bug discovered:** In `src/base/db.cpp`, `DB::initStructs()` loops over jobs but previously read ALL rows from `Jobs_Tasks` and ALL rows from `Jobs_SpriteID` for **each** job (missing an ID filter). The original `content/db/ingnomia.db.sql` has 54 jobs, 52 task rows, and 39 sprite rows. This erroneously yields 2,808 task attachments and 2,106 sprite attachments rather than 52 and 39. Each job's task list could contain completely unrelated actions, affecting real gameplay correctness and CPU/memory usage.

**Small fix committed:** Use the existing `DB::selectRows("Jobs_Tasks", job->ID)` and `DB::selectRows("Jobs_SpriteID", job->ID)` overloads. No data schema, gameplay design, inventory storage, or save format changes. Extended `prototype/rmlui/engine_services_bootstrap_test.cpp` to assert the **cached task and sprite counts for every job** equal the original database's rows scoped by ID; also assert global cached totals exactly match raw table counts.

**Verification — PASS:** [Windows Noesis-free run 43628614627](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43628614627) completed successfully. The `Compile real simulation source without Noesis or legacy PCH` step and `Initialize original DB and Strings services in isolated runtime` (including the new per-job task/sprite regression assertions) passed. Existing original Config, in-memory SQL, isolated-save-root and RmlUi checks also passed. This is still an isolated database-services test: original map, GameManager lifecycle, desktop map mouse input, and playable world remain untested.

**Next:** After successful job cache regression, return to PLAY-01C GameManager constructor/shutdown using the real Config/DB/Strings services from #14 (plus the job-cache fix from #15). Preserve base dependency sequence #8 -> #10 -> #11 -> #12 -> #14 -> #15 and explicitly recheck open PRs before cherry-picking. Do not merge superseded PR #13.


## PLAY-01C embark settings integration and combined-material regression (2026-11-14)

**Branch:** `feature/playable-newgame-embark-smoke`, [draft PR #18](https://github.com/durpscream44-cmyk/Knomira/pull/18), **based on PR #15** `feature/fix-job-cache-task-scope`. This is another small linked startup step, not a GameManager/world renderer.

**Newly identified original-game bug:** In `src/game/newgamesettings.cpp`, `NewGameSettings::setStartingItems()` previously read both `mat1` and `mat2` from `components[0]` in the `CombinedItem` case. This loses the true second material when an embark item is saved and reloaded. Changed `vmc2` to read `components[1]`. No save format or technology change.

**Runtime integration/test implementation:**
- Moved the original three `IO::saveFile` JSON overloads from large `src/base/io.cpp` to standalone `src/base/io_json_writer.cpp` with original behavior. This allows the **real** `NewGameSettings::save()` and preset methods to link without the world save serializer. `src/CMakeLists.txt` uses source globbing; **rerun CMake configure** when picking up this new source.
- Added `prototype/rmlui/newgame_settings_bootstrap_test.cpp`: real Config, DB::init, DB::initStructs, Strings::init and **real** `NewGameSettings` constructor with its Qt metaobject. Test uses the marked temporary `KNOMIRA_TEST_DATA_ROOT`, verifies positive world dimensions/gnome count, nonempty random name/seed and preset names, writes a Pickaxe composed of two distinct materials, checks serialized JSON and a **new actual NewGameSettings instance** after reload, and asserts there are no colony saves.
- Added `knomira_newgame_settings_bootstrap_test` to the standalone Qt/CMake build and Windows CI; added the extracted writer to the original simulation compile gate.
- This is not a player-visible New Game button, actual world generation, gnomes, renderer, or GameManager linkage. Do not claim playability.

**Verification — PASS:** Windows [Noesis-free run 43638918319](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43638918319) completed successfully. The exact new step `Initialize real NewGameSettings and verify two-material embark roundtrip` passed, along with real original engine-source compilation, existing Config/DB/Strings, isolated-root, RmlUi input/layout, SQL data/AI asset checks and runtime staging. The test executed against the original staged `content/db`, `content/JSON`, `content/ai`; no colony was created. PR #18 remains a reviewable **draft** and is not merged.

**Parallel PR reconciliation:** [PR #17](https://github.com/durpscream44-cmyk/Knomira/pull/17) is a **sibling** based on PR #15. It separately makes original `DB::init()` fail closed on missing/corrupt SQL and has passed its own Windows CI. PR #18 does **not** include PR #17. Integrate both after review (base stack #8 -> #10 -> #11 -> #12 -> #14 -> #15; then reconcile #17 and #18), without double-applying unrelated changes. Superseded #13 and #16 must remain closed.

**Next task after green:** Link the original `EventConnector` / `GameManager` bootstrap under an isolated temp root, but expect many aggregator and renderer dependencies. Prefer one real service at a time; audit Qt thread ownership and `Global::util` before repeated lifecycle tests. A successful embark-settings bootstrap does not replace a working colony.

