# Phase 3 — Experimental RmlUi overlay inside the game window

## What this milestone is

The **same** MainWindow Qt QWindow / QOpenGLContext has an **opt-in**
path to draw a **passive** RmlUi diagnostic document after the original
world renderer and Noesis, but before the existing swapBuffers().

The test should show a small KNOMIRA / RmlUi panel in the upper-right
corner **inside the real game window**. It has no buttons or input
capture, so original controls continue down their existing paths.

This is not yet a new main menu, a Noesis-free game build, or
controller-driven gameplay.

## Build prerequisites

The actual game still needs its existing dependencies, including
NoesisGUI / NoesisApp licensing, Steam SDK, OpenAL, Qt, OpenGL and GLAD.
Building the separate Qt prototype does **not** satisfy those requirements.

When those dependencies work, enable the experimental path with CMake:

```powershell
cmake -S . -B build/ingnomia-rmlui -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.12.0/msvc2022_64" -DKNOMIRA_RMLUI_ENGINE_OVERLAY=ON
cmake --build build/ingnomia-rmlui --config Debug --target Ingnomia
```

Supply game-specific Noesis, Steam, OpenAL, and FreeType configuration
as you would for the existing game. RmlUi 6.3 is fetched if enabled.
The experimental compile switch is **OFF by default**.

At runtime, the panel requires a second explicit opt-in:

```powershell
$env:KNOMIRA_TEST_RMLUI_OVERLAY = "1"
# Launch the compiled Ingnomia.exe from its Debug output directory.
```

Removing that environment variable restores the unmodified Noesis UI.
The RML file is staged to Debug/content/rmlui/engine_overlay.rml.
The adapter logs a warning and disables itself if initialization fails.

## Build verification without Noesis

The new knomira_rmlui_engine_adapter_compile target in the prototype
CMake project compiles the **same** engine_overlay.cpp and RmlUi
OpenGL3 renderer with the game's glad/gl.h loader. This runs in the Qt
prototype CI without needing Noesis.

This is a **static library compile check**, not a standalone UI or
verification of linking/running the complete original game.

## Safety / performance contracts

- RmlUi reuses MainWindow's QOpenGLContext. It creates no window or GL context.
- The GL3 renderer uses the game's existing GLAD loader, not a second copy.
- The passive overlay receives no input, makes no game commands, and does
  not introduce simulation ticks or inventory scans.
- The game forwards resize information and shuts down RmlUi before
  the game GL context is destroyed.
- The overlay requires both the CMake build flag and runtime environment flag.
- The main game still requires Noesis for existing menus, saves and HUD panels.

## Acceptance criteria for next milestone

1. Windows CI compiles the Qt adapter and validates the installer syntax.
2. Full game with the CMake flag OFF behaves unchanged.
3. Full game with flag ON but no runtime environment variable behaves unchanged.
4. With both enabled, a small RmlUi panel appears over the game world/menu.
5. Resize, fullscreen and rendering remain healthy.
6. No new OpenGL errors, corruption, crashes or input capture.
7. Shutdown releases the overlay before the Qt GL context is destroyed.

Only after this validation should we wire menu actions to game commands
and begin migrating the old Noesis panels.
