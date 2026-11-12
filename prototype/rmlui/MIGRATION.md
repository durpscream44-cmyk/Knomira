# Knomira UI migration: audited integration points

## Current verified state
The existing main target still requires `find_package(Noesis REQUIRED)`, `NoesisApp`, and a Noesis license header. The isolated RmlUi prototype is **not yet wired into the game**.

## Actual architecture found in the Ingnomia source
- `src/gui/mainwindow.h/.cpp`: Qt `QWindow` with a `QOpenGLContext`. Noesis input routing, hit testing, XAML component registration, view lifetime, UI resizing and paint compositing live here. Rendering is triggered through Qt update events.
- `src/gui/mainwindowrenderer.h/.cpp`: world renderer using OpenGL. Preserve world rendering, camera movement, framebuffer behavior, and selection while changing UI.
- `src/gui/eventconnector.h/.cpp`: gameplay-event dispatch and commands such as starting/loading a world. Prefer an adapter to this instead of directly coupling RmlUi widgets to simulation internals.
- `src/gui/aggregator*.h/.cpp`: Qt-backed gameplay data aggregation; useful future interface for read-only UI models, but individual Noesis types must be replaced deliberately.
- `src/gui/xaml/`: Noesis-specific models, proxies, and XAML screen controllers. Do not assume these can compile without the Noesis SDK just because the window was replaced.
- `content/xaml/`: original XAML layouts and Noesis themes. Treat as behavioral/UI reference only; convert screens to original RML/RCSS and preserve asset licensing.
- `src/CMakeLists.txt`: recursively adds all .cpp/.h files; a full migration must explicitly exclude legacy Noesis sources or move them behind a separately configured build option. Merely removing find_package(Noesis) won't work.

## Safe next milestone
1. **Validate prototype in CI on Windows**. Fix dependency/build errors before changing game files. Test launch interactively on an actual desktop; CI compilation is insufficient for rendering/input success.
2. Prototype the RmlUi + Qt `QOpenGLContext` adapter using the existing world window, not the temporary GLFW window. Process Qt key/text/mouse/wheel events through RmlUi input translation. Preserve existing gameplay hotkeys and camera controls when the UI does not consume input.
3. Use a dedicated UI renderer that draws over the current world framebuffer. Ensure OpenGL state is restored, viewport/scaling reflects Qt `devicePixelRatio()`, and compositor order remains world then UI then `swapBuffers()`.
4. Introduce a minimal command interface for `new_game`, `load_game`, `settings`, `quit` through the existing `EventConnector` and menu flow. UI implementation should not modify simulation logic or save formats.
5. After this proves out, **isolate legacy Noesis files** in CMake, migrate one screen at a time, and only then remove the Noesis dependency.
6. Run menu -> create world -> pause -> save -> load -> resume smoke tests.

## Rendering and performance guidance
- UI should not trigger extra simulation ticks. Distinguish simulation updates, UI layout updates, and rendering requests.
- Avoid full UI model rebuilds every frame; update observable values only when changed. Inventory shows immediately accurate values, with cached totals and indexed queries rather than full-world scans.
- Keep input, render and gameplay changes incremental and testable.
- Do not embed absolute paths to fonts in production; prototype uses system fonts only as a convenience.
- The UI port is not finished merely because an RmlUi splash menu displays: actual game execution still depends on Noesis code in numerous compilation units.

## Acceptance criteria for the prototype
- Windows MSVC x64 Debug build succeeds with RmlUi, GLFW, and FreeType.
- Opens a resizable GUI window with four legible buttons.
- Mouse clicks change status for the non-wired buttons; Exit closes the window cleanly.
- No Noesis license, Noesis SDK, Qt, Steam, or original game files needed for the isolated build.
- CI and desktop smoke test evidence recorded in the pull request.

## Technical caveats
The standalone sample's GLFW/OpenGL3 backend is a quick validation tool, **not** the desired final game backend. RmlUi's backend API documentation expressly recommends customizing the backend when integrating into an advanced application.
