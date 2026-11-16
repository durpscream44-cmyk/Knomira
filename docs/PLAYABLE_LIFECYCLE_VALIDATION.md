# PLAY-01D / PLAY-01E validation — 2026-11-15

## Scope and status

- PLAY-01D **PASS**: real GameManager/EventConnector/15-aggregator startup and
  cooperative shutdown ran locally and in Windows CI at `8d5f67b` (PR #21).
- PLAY-01E **local runtime PASS; Windows build/asset-guard PASS** at `f982056`
  (PR #22, based on #21): real colony, timers, pause/speed, save/reload and restored
  tree-job completion locally. The full colony runtime is not run by asset-free CI.
- The RmlUi frontend remains a menu shell. These tests do not establish playable
  UI, general save corruption recovery, Release compatibility or long-run stability.

## Confirmed causes and changes

PLAY-01D's previous source gate did not link the actual manager graph. The new
test-only static library compiles the original base/game/gfx and aggregator
sources, with real FastNoise, exprtk and OpenAL. It uses a Qt-only PCH; the
existing selected-source no-PCH gate remains intact. The test owns a marked
QTemporaryDir before Config starts, and the library's IO is compiled with the
existing mandatory test-root safeguard.

Shutdown reproduced a runtime failure/hang. Util lacked an owner; QObject
globals outlived their objects; audio objects were destroyed without their
context current; device cleanup queried a closed OpenAL handle. Explicit Util
ownership, cleared globals and ordered audio cleanup fix those paths.
AggregatorSettings needed its own Global include to compile independently.

PLAY-01E generated the original terrain and settlers, then reproduced the
QList insertion assertion in MilitaryManager::init/addSquad. Entity IDs are not
list indexes. Appending new/loaded squads preserves IDs and saved priority order.
The same two-line fix existed in the original dirty checkout and was selectively
ported. No other uncommitted viewer/simulation changes were imported.

## Exact changed files

PLAY-01D code/build/documentation:

- `.github/workflows/noesis-free-shell.yml`
- `prototype/rmlui/CMakeLists.txt`
- `prototype/rmlui/engine_lifecycle.cmake` (new)
- `prototype/rmlui/engine_lifecycle_test.cpp` (new)
- `src/game/gamemanager.cpp`
- `src/game/gamemanager.h`
- `src/gui/aggregatorsettings.cpp`
- `src/gui/aggregatorsound.cpp`
- `src/gui/openalwrapper.cpp`
- `BUILDING.md`
- `CURRENT_CHECKPOINT.md` (subsequent documentation commit)

PLAY-01E code/build/documentation:

- `.github/workflows/noesis-free-shell.yml`
- `prototype/rmlui/CMakeLists.txt` (CMake 3.25 minimum for imported OpenAL target)
- `prototype/rmlui/engine_lifecycle.cmake`
- `prototype/rmlui/colony_lifecycle_test.cpp` (new)
- `src/game/militarymanager.cpp`
- `BUILDING.md`
- `CURRENT_CHECKPOINT.md` and this report (subsequent documentation commit)

## Verification

Toolchain: Windows, MSVC 19.44, Qt 6.12.0, CMake 3.30, existing local RmlUi and
vcpkg dependencies. The build directory is `build/lifecycle` in the isolated
worktree. `BUILDING.md` gives configure/runtime environment commands.

1. Configured standalone `prototype/rmlui` with GAME_SHELL and QT_HOST enabled.
2. Built Debug targets: `knomira_engine_lifecycle_test`,
   `knomira_colony_lifecycle_test`, `knomira_game_shell`, the original simulation
   source/header gates, `knomira_rmlui_qt_host`, engine adapter, standalone
   prototype and all regression executables.
3. `ctest --test-dir build/lifecycle -C Debug --output-on-failure`: **11/11 PASS**
   with authorized local assets; final run 31.02 seconds. Covers Config, DB
   rollback/job cache, two-material embark, both data-root variants, save
   metadata catalog, input bindings, RmlUi geometry, manager and colony lifecycles.
4. Asset-free colony invocation: **Skipped**, exit 77. Explicitly invalid asset
   paths fail; no substitute sprites or simulation are generated. Windows CI
   builds the colony executable and checks the missing-asset contract.
5. The colony test generated 532,480 tiles and one settler, advanced real timer
   ticks, verified pause, restored the pending order and reordered squads, then
   let the settler fell the tree and create wood. Both save slots existed and
   the first slot's SHA256 file map was unchanged after load and after save 2.
6. Original audio was staged only under the ignored build directory for the
   final run. Audio buffers loaded and teardown succeeded using OpenAL Soft's
   null output; audible playback was not tested.
7. `git -c core.whitespace=cr-at-eol diff --check`: PASS. No generated binaries,
   tilesheets, audio, SDK content or credentials were staged in Git.

PLAY-01D Windows CI at the exact code SHA passed:
[Noesis-free shell and lifecycle](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43766361570),
[Qt host](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43766361653),
[RmlUi prototype](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43766361619).

PLAY-01E Windows CI at `f982056470e3fbf5d046d2582f278b6c4d4856e7` also passed:
[Noesis-free shell, real manager lifecycle and colony asset guard](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43768110225),
[Qt host](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43768110220),
[RmlUi prototype](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43768110231).
The checkpoint/report commits after those code SHAs change documentation only.

## Preservation and remaining limits

The original checkout remains on `feature/controls-menu-redesign`; its 53
pre-existing modified/untracked status entries remain present. Work occurred in
`C:/Users/durps/Knomira/work/play01d-real-lifecycle`, first on the PLAY-01D branch,
then on `completion/play01e-colony-roundtrip`. No reset, stash, cleanup or
whole-checkout merge was used. Git author identity was supplied per command
from the existing history; global Git configuration was not changed.

Tests never opened `Documents/My Games/Ingnomia`. All settings and colony saves
used marked temporary directories. Production IO paths and save schemas are
unchanged. Successful test roots are disposable; failure roots may remain for
diagnosis. Assets were read from the existing native-viewer build.

Fresh roots still log missing optional userpresets/military settings. Existing
missing-return warnings in Util::borderPos and StockpileManager::getJob remain.
The original simulation internally reseeds some RNG paths, so fixed input seeds
do not imply identical generated populations across runs; assertions compare
the same colony before and after persistence. The colony smoke runs on one Qt
application thread; active simulation with a separate rendering thread is not
yet covered. The legacy Noesis main function
still has its old terminate path. No original user save or long session was tested.

Next: PLAY-02A, connect the RmlUi session and original world renderer, with
actual desktop verification and protected integration-test data roots.
