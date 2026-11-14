# Map mouse click diagnostic checklist

This is for the **legacy Ingnomia map window**, not the separate Noesis-free
RmlUi menu/saves shell. The RmlUi shell cannot display an actual playable
world yet, so do not misdiagnose its lack of world selection as this bug.

## Small source-level fix

In `src/gui/mainwindow.cpp`, Qt 6 mouse press/move/release/wheel now read
`QMouseEvent::position()` / `QWheelEvent::position()`, which are already
widget-local, rather than converting global coordinates back to local.
`mouseReleaseEvent` always refreshes `m_mouseX` and `m_mouseY` before
dispatching to the selection aggregator. This is relevant when a click
occurs without an intervening hover/move event, and on scaled displays.

On right-click release, the world-button state is cleared even when
the pointer moved over a UI element.

## Debug traces

When `Global::debugMode` is on (the legacy window toggles it with
**Ctrl+O**), the following `qDebug()` messages explain where clicks go:

- `KNOMIRA_MOUSE press`: button, widget-local position, whether the
  legacy Noesis view exists and whether GUI hit-testing owns that point
- `KNOMIRA_MOUSE release`: same, with whether the click started on the
  world and whether a drag was recognized
- `KNOMIRA_MOUSE world left dispatched`: input reached world click dispatch
- `KNOMIRA_MOUSE aggregator left`: queued signal reached
  `AggregatorSelection`, with the selected world tile and action status

Observe a single click in the center of the map, then one on an obvious
visible UI element.

### Interpretation

1. No press/release logs: the Qt game window isn't receiving mouse input;
   investigate overlay, focus, event filters and window geometry.
2. Press/release present but `overGui true` on a map tile: the Noesis
   hit-test or an invisible/fullscreen UI element is intercepting world
   input. Inspect actual `isOverGui()` hit targets before changing behavior.
3. `overGui false`, but release has `leftStartedOnWorld false`: the mouse
   down wasn't captured for world input, or focus/GUI ownership changed.
4. `world left dispatched`, but no `aggregator left`: check queued signal
   connections, processing thread and lifecycle for
   `AggregatorSelection`.
5. `aggregator left` with unexpected cursor: investigate
   `calcCursor()`, renderer camera parameters, viewport scaling and
   world-to-screen projection.
6. `aggregator left` with correct tile/action but no resulting work:
   investigate `Selection::leftClick()` and the job command's tile validity.

## Test cases

- Click a tile without first moving the pointer; it selects the tile
- Move pointer and click a different tile; release uses the newest coordinate
- Press a map tile, release over UI; no accidental world job
- Press and release over map; click is forwarded exactly once
- Drag over map; camera drag remains a drag, not a tile click
- Use right-click to cancel a selection, move to GUI, release, then
  right-click again; no stuck button state
- Zoom / select across different DPI and multiple monitors
- Keyboard movement and mouse wheel still operate
- Retest mouse/controller capture in the **separate RmlUi shell** to
  ensure input routing has not regressed elsewhere

## Limitations

The original game's executable still requires Noesis at this point, and
the CI Noesis-free shell doesn't compile `mainwindow.cpp`. Therefore this
legacy mouse behavior is **not build-tested or desktop-tested** in CI.
The log is opt-in. Do not claim it is fixed until a real map-window test
confirms click events and world selection behave correctly.
