# Qt/OpenGL RmlUi hosting — integration proof of concept

## Why this exists

Knomira's **game window is a Qt 6 QWindow with a manually owned QOpenGLContext**,
while the working RmlUi menu prototype used a **separate GLFW window**. The
`knomira_rmlui_qt_host` executable is a minimal bridge between those
architectures: it draws our existing menu through RmlUi's GL3 renderer in a Qt
window, with Qt native input forwarded into RmlUi.

This test **does not load game simulation, save files, original Noesis UI, Steam,
OpenAL, or world rendering**. It is not yet the in-game replacement UI.

## Build on Windows (existing Qt setup)

Use the x64 Native Tools Command Prompt for Visual Studio 2022, from the
Knomira repository root. Requirements: Qt6 MSVC2022 x64, CMake, Git, and
vcpkg-installed `glfw3:x64-windows` and `freetype:x64-windows`.
GLFW is needed by the *original standalone target's build configuration*
but isn't used by this Qt window.

```bat
cmake -S prototype/rmlui -B build/rmlui-prototype-qt -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_PREFIX_PATH=C:/Qt/6.12.0/msvc2022_64 -DKNOMIRA_BUILD_QT_HOST=ON
cmake --build build/rmlui-prototype-qt --config Debug --target knomira_rmlui_qt_host
build\rmlui-prototype-qt\Debug\knomira_rmlui_qt_host.exe
```

The Windows CI workflow `.github/workflows/rmlui-qt-host.yml` also builds the
target against Qt 6.8.3 and uploads a packaged artifact containing Qt runtime
dependencies. This is a compilation/package check, **not** a graphical test.

## Acceptance tests

- Build produces `knomira_rmlui_qt_host.exe` with the menu RML file adjacent.
- Qt opens a resizable OpenGL window on desktop.
- Buttons work: first three update status and Exit closes the window.
- Mouse hit testing still works at 100% and 150% Windows DPI scaling.
- Keyboard navigation, resizing, clipboard, and close/reopen are checked.
- No Noesis SDK or license is required by this *test target*.
- Existing game CMake and gameplay code remain unchanged.

## Next after successful tests: actual MainWindow integration

1. Reuse the *game's existing* Qt window and OpenGL context rather than creating
   a second one. Prefer extracting this adapter into a small header/source pair
   owned by MainWindow.
2. Evaluate GLAD symbol/header compatibility: the game already has GLAD 4.3,
   while the standalone RmlUi GL3 renderer includes a different generated GLAD
   loader. Do **not** blindly compile both in the same executable until the
   loader strategy is resolved.
3. Preserve world render order, Qt event routing, camera movement, and GUI
   consume/propagate semantics. On resize and DPI changes, update RmlUi's
   viewport/scale in framebuffer pixels.
4. Map menu commands to EventConnector only when a valid screen flow exists.
   New Game in particular requires the original world-creation UI to migrate,
   not just a direct call to onStartNewGame.
5. Explicitly separate Noesis-only sources in CMake; current
   `src/CMakeLists.txt` globs all C++ sources so just removing the link library
   will not compile. Plan screen/controller replacement incrementally.
6. Only remove NoesisGUI after the required gameplay HUD and save/load screens
   have tested equivalents.

## Performance contract

Do not introduce simulation ticks from UI rendering or input. Resource totals
remain authoritative and accurate; update UI read-models based on changes
instead of scanning passive item records on every frame.
