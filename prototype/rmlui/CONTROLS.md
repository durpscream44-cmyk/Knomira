# Knomira controls and keybindings

## Current build support

The Noesis-free shell now has **Settings / Controls**, with a searchable-by-scrolling
action list covering menu actions plus reserved controls for the future game.

Each action stores two keyboard shortcuts and two controller assignments. Click
a binding to capture a replacement key or Xbox input. It supports D-pad, face
buttons, bumpers, triggers, thumb-stick directions, and stick clicks. Controller
triggers and stick directions use drift-resistant thresholds.

- **Menu (active)**: Previous, Next, Select, Back. These are dispatched now.
- **World (planned)**: camera movement, zoom, z-levels, rotation, pause, time
  speed, select tile, cancel tool.
- **Panels (planned)**: construction, inventory, stockpiles, workshops, farms.
- **System (planned)**: save and load shortcuts.

Planned actions are **editable and saved** but not yet connected to the game
simulation. This ensures users can design their preferred layout now without
claiming unfinished gameplay functions work.

### How to customize

1. Open **Settings / Controls** from the main menu.
2. Click a keyboard slot K1/K2 or controller slot Pad1/Pad2.
3. Press the desired key/shortcut or controller button, trigger, stick direction.
4. The game checks for duplicate assignments within the same context.
5. Valid choices save immediately.
6. Use **Clear binding** to unassign a secondary input (primary cannot be the
   last remaining keyboard/controller input for an active menu action).
7. **Restore Default Controls** restores the factory preset and saves it.
8. Escape cancels a keyboard rebind; use **Cancel** for controller rebinding.

For safety, **Tab / Shift+Tab** still navigate between controls and **Escape**
acts as an emergency Back shortcut even if custom mappings change.
Mouse clicks on the UI always work, independently of custom shortcuts.

### Persistence

The shell uses `QStandardPaths::AppConfigLocation` to save a Knomira-specific,
versioned `controls.json` atomically. On Windows, the location is usually
the user's roaming application configuration under a Knomira directory.
The exact path depends on Windows and Qt settings; do not hard-code it.

The profile stores action IDs and two input slots per device, not
engine objects or proprietary UI types. Invalid/malformed JSON falls back
to safe defaults. Conflicts are rejected within each context so menu keys
can also be used later for world controls.

### Test matrix

- Remap Previous to F3; verify Up no longer triggers Previous and F3 does.
- Remap Previous Xbox input from D-pad Up to X; verify it triggers.
- Attempt to bind the same key/Xbox input to two menu actions; ensure conflict.
- Relaunch the shell: remapped bindings should persist.
- Clear a secondary binding and restore all defaults.
- Exit confirmation should still default to Cancel, respond to mouse/keyboard
  and Xbox, and never close on an accidental first click.
- Original Qt-hosted prototype should remain unaffected.
- World and panel binding edits should persist but not invoke gameplay yet.

**Next step:** replace the remaining Noesis-only command paths, then attach
the saved world/panel action IDs to engine state and input routing. When
tile selection and world camera are integrated, add explicit mouse action
bindings and context-sensitive world focus. Never send a world command
when an RmlUi input or modal owns focus.
