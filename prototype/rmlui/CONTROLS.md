# Knomira controls and keybindings

## Current build support

The Noesis-free shell now has **Settings / Controls**, with four category tabs (Menu, World, Panels, System) and two input-device
tabs (Keyboard, Xbox Controller). Each row shows one action and large, readable
Primary and Alternate assignments. The screen highlights which category/device
is selected, and clearly marks unfinished gameplay commands as planned.

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
2. Choose **Menu**, **World**, **Panels**, or **System** at the top.
3. Choose **Keyboard** or **Xbox Controller**.
4. Click the large **Primary** or **Alternate** binding field. One left mouse
   press opens the centered capture dialog; keyboard/Xbox selection also works.
5. Check the action name and existing binding shown in the dialog, then press
   your desired key/shortcut or controller button, trigger, or stick direction.
6. A conflicting assignment stays in the dialog and explains the problem.
7. Valid choices save immediately.
8. **Unbind This Slot** clears a binding, but each active menu action retains
   at least one keyboard and one controller binding.
9. **Restore Defaults** restores the factory preset and saves it.
10. Escape or **Cancel Change** dismisses a binding dialog without modifying it.

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

### Mouse interaction regression

RmlUi normally sends a click only when the pointer presses and releases over
the same element. The old control list forced focus scrolling during a click,
which could move the target out from under the mouse. Each binding field now
starts capture on **left mousedown**, while keyboard/Xbox activation retains
the normal click path. Focus scrolling is reserved for navigation by keyboard
and Xbox, and the pointer position is updated before each mouse release.

### Test matrix

- Open Settings. Confirm Menu + Keyboard are visibly selected and only four
  active menu actions are shown (not a wall of four inputs per action).
- Mouse: click every primary and alternate field; the **first click** must
  open the dialog, not just scroll the list. Confirm action/device/current value.
- Switch category and device with mouse and Xbox; highlighted tabs and fields
  must stay in sync.
- Navigate the long World list with Xbox: selected rows must scroll into view.
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
