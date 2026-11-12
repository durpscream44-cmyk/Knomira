# Knomira: migration from NoesisGUI to a playable RmlUi game

## Why a direct replacement does not compile yet

The original Ingnomia executable still creates a NoesisGUI view from
`src/gui/mainwindow.cpp`. The root CMake file requires the Noesis SDK,
NoesisApp and license setup. `src/pch.h` includes NoesisPCH, while
`src/CMakeLists.txt` globs **all** C++ files, including the old
`src/gui/xaml/*.cpp` models. Removing `find_package(Noesis)`
alone cannot produce a working executable.

The GL world renderer is a separate `MainWindowRenderer` object.
It can be preserved and moved to the new shell after its dependencies
are cleaned up. The Qt-hosted RmlUi prototype already verifies that
a Qt `QWindow` and `QOpenGLContext` support the UI.

## Milestones (in dependency order)

### 1. Independent Noesis-free executable — in progress

`knomira_game_shell` builds from the prototype CMake project. It uses
RmlUi / Qt / OpenGL, and preserves keyboard and Xbox menu controls.
It scans original Ingnomia save slots **read-only** and shows real
kingdom metadata, without trying to deserialize game state.

Success gate: Windows CI compiles and packages the app; user confirms
mouse, keyboard, Xbox and saved-kingdom browsing on a desktop.

### 2. Make the game core build without a Noesis precompiled header

Create a distinct `knomira_simulation` CMake library from
`src/base`, `src/game` and the Qt-only `src/gui/aggregator*.cpp`
and `eventconnector.cpp` as they are proven to compile, excluding
`src/gui/xaml` and Noesis-bound sources. Replace incidental PCH
assumptions with explicit includes, and keep any required Steam /
OpenAL dependencies behind small service abstractions.

The complete old game remains buildable throughout migration. Do not
blindly remove all legacy files; split source lists by ownership.

Success gate: engine C++ and Qt signal paths compile, independently of
Noesis, before attempting world generation.

### 3. Link the real engine lifecycle

Connect Qt app bootstrap to existing Config, DB, NewGameSettings,
GameManager and EventConnector, retaining the original game-thread
ownership and signals. Guard all UI actions against game state and
dispatch commands to the correct thread using queued Qt connections.

New Game must expose the original embark settings rather than
launch world generation with arbitrary/default values. Load Game
must pass a selected, validated save slot to GameManager and must
not alter the save during browsing.

Success gate: creation / load / save / pause with error reporting and
thread-safe transition to gameplay.

### 4. Render the real world underneath RmlUi

Attach the existing MainWindowRenderer to the new shell's existing
Qt OpenGL context. Continue using the existing game GLAD 4.3 loader,
not the separate prototype GLAD or a second GLFW context. Render
world first, then RmlUi, then swap. Preserve GPU tile update,
camera, selection and sprite atlas state.

Success gate: an actual generated settlement displays, camera moves,
zoom and z-level navigation work, and the UI does not intercept
unhandled world clicks. Confirm GL state across fullscreen/resizes.

### 5. Replace Noesis menus and the gameplay HUD incrementally

Port save/load screens and new-game settings first. Then inventory,
stockpiles, building, farming, workshops, creatures and combat.
Wire each page into the existing Qt aggregator signals with
change-driven updates; do not poll passive item inventories each frame.
Use cached, accurate inventory totals.

Gamepad: keep XInput menu navigation already tested; add game-world
controls separately, with explicit input focus and mouse/keyboard
fallbacks.

Success gate: a colony can be created, saved, resumed and meaningfully
managed with RmlUi alone.

### 6. Remove Noesis for good

After the new executable works, remove the old Noesis-only XAML
sources, NoesisPCH and license/toolchain requirements from the
new game build. Leave the legacy implementation on its own branch
until regression tests and long-running colony saves pass.

## Immediate validation commands (from Knomira checkout)

Switch to the `feature/noesis-free-shell` branch, then use GitHub
Actions `Knomira Noesis-Free Game Shell (Windows)` for a packaged
binary. Once the workflow passes:

```powershell
git fetch origin
git switch --track origin/feature/noesis-free-shell
powershell -NoProfile -ExecutionPolicy Bypass -File .\prototype\rmlui\Install-Prototype.ps1 -GameShell
```

If the branch exists locally, use `git switch feature/noesis-free-shell`.
The installer uses a **separate output folder** so it cannot overwrite
the earlier working Qt/Xbox executable.

**Important:** The new shell is not yet a playable colony simulation.
It verifies UI and save discovery while we separate the engine code.
