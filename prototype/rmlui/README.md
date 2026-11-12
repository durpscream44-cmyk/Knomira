# Knomira RmlUi main-menu prototype

This is an **isolated evaluation target**, not a game UI migration yet. It deliberately leaves the existing Ingnomia/Noesis build, simulation, save system, and gameplay untouched.

## What it demonstrates

- A new RmlUi-based window with an original dark slate, brass, and parchment-inspired menu.
- Mouse input and button actions for New Game, Load Game, Settings, and Exit.
- A status message for unconnected actions, making it clear that no gameplay is wired up.
- No dependency on the proprietary NoesisGUI SDK or license.

## Requirements (Windows)

- Visual Studio 2022 with MSVC C++ components.
- CMake 3.24 or newer and Git on PATH.
- GLFW 3.3+ and FreeType installed for x64 MSVC. Easiest method: [vcpkg](https://github.com/microsoft/vcpkg) packages `glfw3` and `freetype`.
- Network access for CMake FetchContent to retrieve the pinned RmlUi 6.3 release.
- A GPU/driver supporting OpenGL 3.3.

Qt 6 is **not needed for this standalone prototype**. The actual game uses Qt 6.

## Example build with vcpkg

Open the **x64 Native Tools Command Prompt for VS 2022** and run commands from the repository root, replacing the vcpkg path as appropriate:

```bat
C:\path\to\vcpkg\vcpkg.exe install glfw3:x64-windows freetype:x64-windows
cmake -S prototype/rmlui -B build/rmlui-prototype -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build/rmlui-prototype --config Debug --target knomira_rmlui_prototype
build\rmlui-prototype\Debug\knomira_rmlui_prototype.exe
```

If `cmake` isn't on PATH, invoke the CMake executable by its full path. If Git/FetchContent is blocked, download RmlUi 6.3 separately and adjust the local CMake source reference.

## Install directly into an existing Knomira checkout

For the easiest setup, use the helper script committed alongside this prototype:

```powershell
.\prototype\rmlui\Install-Prototype.ps1
```

It downloads the **latest successful GitHub Actions build** into
`build\rmlui-prototype\desktop-test\` within the current Knomira checkout
and then launches the UI prototype. It requires [GitHub CLI](https://cli.github.com/)
(`gh`) and `gh auth login` once. To download without launching, add `-NoLaunch`.

The installer uses its own script directory to find the repository root, so it
does **not** need an absolute local development path. The script does not edit
game code or installed game files. If a previous test package exists, it is
renamed to `desktop-test.previous` to preserve it. Remove or rename that backup
before using the helper again.

## Windows GitHub Actions desktop test

The `Knomira RmlUi Prototype (Windows)` workflow builds the prototype and uploads
`knomira-rmlui-prototype-windows-debug` containing the executable, `main_menu.rml`,
and required vcpkg Debug DLLs. From the successful workflow run, download the
artifact ZIP, extract **all files to the same folder**, and launch
`knomira_rmlui_prototype.exe`.

Test all four buttons, window resizing, and clean exit. The first three buttons
only display placeholders: they are not connected to game creation, saves, or
settings yet. If Windows reports a missing MSVC runtime, install the matching
Visual C++ Redistributable or run on a machine with Visual Studio 2022 installed.

**Important:** A successful CI compile is not proof that the app opens or renders.
The main menu still needs a Windows desktop smoke test.

## Scope and next steps

1. Confirm the standalone prototype configures, compiles and launches on Windows.
2. Test input, font rendering, DPI scaling, resizing and clean exit.
3. Audit the actual Ingnomia Noesis-related input/rendering/controller components.
4. Adapt RmlUi to the game's **existing window/render context** in a second phase, preferably without making a second permanent window.
5. Migrate main menu commands first; leave gameplay panels until there's a tested adapter.

**Status:** Repository-only prototype. The code has not been compiled or launched in the assistant's environment; treat it as an unverified initial integration scaffold, not a completed migration.

**Licensing:** RmlUi is MIT-licensed; check notices and licenses for bundled/transitive dependencies and any assets included in releases.
