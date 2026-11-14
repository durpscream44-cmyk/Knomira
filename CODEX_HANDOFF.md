# Knomira — Codex development handoff

**Last reviewed:** 2026-11-14 (updated with verified DB/bootstrap milestone)  
**Authoritative working branch for the next playable-first ticket:** `feature/playable-engine-compile-gate` (draft [PR #8](https://github.com/durpscream44-cmyk/Knomira/pull/8)).  
**Status:** UI shell builds; real simulation **does not yet run inside the Noesis-free executable**. Keep the mission **playable -> stable -> optimized**.

This file is a durable "where to pick up" checkpoint for Codex or a local desktop developer. It records what was confirmed, what was only compiled, what is untested, and the next safe work units. Read the root [AGENTS.md](AGENTS.md), [prototype/rmlui/AGENTS.md](prototype/rmlui/AGENTS.md), and [Noesis-free migration plan](prototype/rmlui/NOESIS-FREE-PLAN.md) as well. **Inspect current code, branches, Actions and the local working tree again** rather than treating this document as proof of live functionality.

## 1. Verified status (as of this review)

| Area | Status | What was verified / not verified |
|---|---|---|
| Qt/RmlUi shell, menus, keyboard and Xbox input code | **CI pass** | Windows builds and automated input tests. Physical mouse/controller behavior still needs real-desktop confirmation, including binding click vs scrolling. |
| Original save-slot discovery | **CI pass** | The shell reads original Ingnomia save *metadata* without loading the game simulation. It must not write to player saves merely to browse them. |
| Real C++ simulation source with Noesis removed from compilation | **CI pass, compile-only** | [Windows run 43575420551](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43575420551) compiled `Game`, `GameManager`, `NewGameSettings`, `TechTree`, `PathFinder`, `EventConnector`, and original `Config`, `DB`, `Global` sources as objects without the Noesis PCH. **Not yet linked or run as a simulation.** |
| Real game SQL bootstrap and runtime asset staging | **CI pass, isolated** | [PR #10](https://github.com/durpscream44-cmyk/Knomira/pull/10) packages original `content/db/ingnomia.db.sql` at `<appDir>/content/db/` and replays actual SQL in an in-memory SQLite smoke executable; verified in Windows run 43575420551. No original save/config was opened or written by that test. |
| World generation, gnome AI in the RmlUi shell, actual map renderer, New Game/Load/Save | **Not integrated** | Core blockers for a playable game. |
| Legacy world mouse-click diagnostic fix | **Draft, not verified** | [PR #9](https://github.com/durpscream44-cmyk/Knomira/pull/9) is based on `feature/controls-menu-redesign`; no original-game desktop result. |
| Inventory search optimization, debug profiler, tick-conveyor standalone test | **Mixed: isolated CI passes** | [PR #7](https://github.com/durpscream44-cmyk/Knomira/pull/7), Windows run [43560407808](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43560407808). Conveyor tests pass but there are no live conveyors. Original engine integration, saves and UPS effects are unverified. |
| Instant inlet/outlet workshop logistics | **Design only** | Preferred over physical conveyor-belt animation; do **not** implement before basic stockpile/workshop gameplay works. |

### Branch/PR map — do not assume merged

- [PR #6](https://github.com/durpscream44-cmyk/Knomira/pull/6): `feature/controls-menu-redesign` -> `feature/remappable-controls` (**draft**). Controls UI, layout tests and an unresolved need for physical mouse verification.
- [PR #8](https://github.com/durpscream44-cmyk/Knomira/pull/8): `feature/playable-engine-compile-gate` -> `feature/controls-menu-redesign` (**draft; next playable integration path**).
- [PR #10](https://github.com/durpscream44-cmyk/Knomira/pull/10): `feature/playable-db-bootstrap-smoke` -> `feature/playable-engine-compile-gate` (**draft; database/data packaging and Global source compile pass**). Integrate after review; it is a child of #8, not the main branch.
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

**Completed sub-milestone, PR #10 / CI PASS:** Original `Config`, `DB`, and `Global` translation units compile without Noesis/PCH; the real checked-in SQLite SQL is staged beside the shell and passes an in-memory reload+table-population test. `src/base/global.cpp` had an orphaned `Noesis::Key` map definition no longer declared in `global.h`; PR #10 removes that stale line. **This remains an independent database executable, not an actual `GameManager` bootstrap.**

**Next sub-milestone (PLAY-01B, unimplemented):** link an engine service/bootstrap target that safely initializes the actual `Global::cfg`, `DB::init()` and `DB::initStructs()`, then constructs/destructs `GameManager` with correct Qt object ownership. Do this in an isolated test data directory so `Config::Config()` calling `IO::createFolders()` cannot create or change the user's normal saves/settings. Audit `Global::reset()`, `EventConnector` aggregator dependencies, and moc/Qt signals. The original `src/main.cpp` currently calls `gameThread.terminate()` on shutdown; new bootstrap must instead use orderly thread quit/wait and object disposal. Preserve original lifecycle semantics; do not silently skip initialization by creating fake managers.



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
- [Engine plus game DB bootstrap CI](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43575420551): completed **success**, including original Config/DB/Global source compilation, original in-memory game SQL replay, and packaged DB staging. **No GameManager link, runtime world or save/load tested.**
- [Earlier engine gate CI run](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43561481732): completed **success**, compile-only game lifecycle target.
- [Standalone transport CI run](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43560407808): completed **success**, but no live inventory transfer.
- Avoid saying map click, live world, or save/load works until tested **in the actual relevant executable**.

## 6. Latest bounded ticket result

- **Date / branch:** 2026-11-14, `feature/playable-db-bootstrap-smoke`, draft PR #10
- **Ticket:** PLAY-01A — game database prerequisite and compile expansion
- **Changed paths:** `src/base/config.cpp`, `src/base/db.cpp`, `src/base/global.cpp`, `prototype/rmlui/engine_database_smoke.cpp`, `prototype/rmlui/CMakeLists.txt`, `.github/workflows/noesis-free-shell.yml`.
- **Verification:** Windows Noesis-free run [43575420551](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43575420551) **PASS**: original source object compile, database fixture execution against the staged original SQL, current shell and existing UI/save/input tests, installer syntax. No desktop gameplay was attempted.
- **Save risk:** SQL test only writes to `:memory:`; it does not construct the user-data-writing `Config` class or touch player saves. DB data staged in the packaged build only.
- **Outstanding:** GameManager and full simulation *not linked or started*; DB::init's real startup and config paths not runtime tested, no world renderer, no save/load gameplay; no manual desktop mouse validation.
- **Next single task:** PLAY-01B above; investigate all real dependency symbols, initialize Config/DB into a disposable test directory, link GameManager, and exercise safe shutdown; mark PARTIAL until that code runs.

## 7. Handoff record template (update this file after each ticket)

- **Date and branch / PR / commit:** 
- **Ticket, tested behavior, code-path root cause:**
- **Files changed:**
- **Build commands + CI run, conclusion, exact failures:**
- **Manual test result:** PASS / PARTIAL / BLOCKED / NOT REPRODUCED
- **Save compatibility / test save used:**
- **What remains broken / what was not tested:**
- **Next single bounded coding task:**

When uncertain, leave a concrete blocker and reproduction command. **A compile-only build success is not a playable game.** No async tasks or work-in-progress promises: a ticket is complete only when the changes, tests, and outstanding blockers are recorded.
