# Knomira — Codex development handoff

**Last reviewed:** 2026-11-14  
**Authoritative working branch for the next playable-first ticket:** `feature/playable-engine-compile-gate` (draft [PR #8](https://github.com/durpscream44-cmyk/Knomira/pull/8)).  
**Status:** UI shell builds; real simulation **does not yet run inside the Noesis-free executable**. Keep the mission **playable -> stable -> optimized**.

This file is a durable "where to pick up" checkpoint for Codex or a local desktop developer. It records what was confirmed, what was only compiled, what is untested, and the next safe work units. Read the root [AGENTS.md](AGENTS.md), [prototype/rmlui/AGENTS.md](prototype/rmlui/AGENTS.md), and [Noesis-free migration plan](prototype/rmlui/NOESIS-FREE-PLAN.md) as well. **Inspect current code, branches, Actions and the local working tree again** rather than treating this document as proof of live functionality.

## 1. Verified status (as of this review)

| Area | Status | What was verified / not verified |
|---|---|---|
| Qt/RmlUi shell, menus, keyboard and Xbox input code | **CI pass** | Windows builds and automated input tests. Physical mouse/controller behavior still needs real-desktop confirmation, including binding click vs scrolling. |
| Original save-slot discovery | **CI pass** | The shell reads original Ingnomia save *metadata* without loading the game simulation. It must not write to player saves merely to browse them. |
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

**Verification — PASS:** [Noesis-free Windows CI run 43578230240](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43578230240) built both test targets and passed the `Verify isolated game data root cannot target player saves` step. The real engine sources also compiled. The runtime tests exercised the shared data-folder resolver only; `IO::getDataFolder()` itself has not been linked into an actual GameManager process.

**Additional lifecycle finding for PLAY-01B (source review only):** `src/game/gamemanager.cpp` assigns `Global::util = new Util(nullptr)` in the constructor and later replaces it via `new Util(m_game)` in both new-game and load paths, as well as a fresh `new Util(nullptr)` in `endCurrentGame()`. `Util` is not a QObject and has a trivial destructor; the displaced pointers are not explicitly freed in these paths. Audit `Util` and `Selection` ownership and shutdown on the real game thread before adding another manager lifecycle. Do not blindly `delete` objects during a running simulation; scope this as a separate measured, testable fix after smoke initialization.

**Acceptance checklist for Codex before declaring this ticket done:** Windows CI passes both root-mode tests and original source compile; no real save/settings path writes; PR #11 remains draft until review; the next linked engine startup test must assert exactly which paths were written inside the temporary root; only call a game playable after a world is generated, rendered, given a genuine job, and save/reload works.


## 8. Integration reconciliation — one canonical Config bootstrap (2026-11-14)

PR #11 is limited to the tested fail-closed data-root/real IO::createFolders protection. Its later, overlapping Config refactor has been backed out of this branch. That extraction remains recoverable in Git history but should NOT be merged together with the following canonical implementation.

- [PR #12](https://github.com/durpscream44-cmyk/Knomira/pull/12) (branch feature/playable-config-startup-smoke, based on #11) is the **canonical, already-CI-tested** Config constructor bootstrap. It moves two JSON load functions into io_settings_file.cpp and Config persistence into config_persistence.cpp. It has a passed Windows Config test.
- [PR #14](https://github.com/durpscream44-cmyk/Knomira/pull/14) (branch feature/playable-db-services-after-config, based on #12) is the **preferred next** real DB::init / DB::initStructs / Strings::init service integration check. Use its Windows CI run to confirm the database service test; it creates no world and uses the marked test root.
- [PR #13](https://github.com/durpscream44-cmyk/Knomira/pull/13) was a separately verified DB/Strings prototype on the competing, now-backed-out Config extraction. It is **superseded for integration** by #14; do not merge it onto #11/#12.
- Merge/test dependency order: #8 -> #11 (after its base #10 as appropriate) -> #12 -> #14. Note #11 actually bases on #10; see PR ancestry. Review all branch heads before merging.

Next after a green PR #14: link genuine GameManager/EventConnector/aggregator dependencies under the temporary data-root guard; verify shutdown and no writes outside sandbox. Do not use stub managers to claim success.
