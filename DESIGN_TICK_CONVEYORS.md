# Knomira — Mid-Game Conveyor / Tick Logistics Design

**State:** transport-core implementation and standalone tests; **not live gameplay**.
**Branch:** `feature/tick-conveyor-prototype`, based on the RmlUi integration work.
This is a design for a later in-game system. No tech-tree unlock, recipe, belt
construction, live inventory transfer, or conveyor RmlUi controls are wired yet.

## Player-facing feature

The player unlocks **Basic Mechanical Conveyors** at a mid-level industrial
milestone (gears/axles and access to workshop automation). Belts are
one-way transport links along tiles; ideally they can be powered by the
existing machinery/axle network, but the powering scheme is **provisional**
until the actual relevant game infrastructure is compiled and inspected.
They move materials between specified input/output stockpiles or workshops
and reduce hauling assignments.

Design example:
`Ore Stockpile -> Conveyor -> Smelter -> Conveyor -> Finished Metal Stockpile`

**Midgame balance:** a belt has an up-front construction cost, requires
a physically connected route, cannot teleport materials through disconnected
areas, has capacity/throughput limits, and cannot send items into full or
filtered destinations. Workers still haul across areas without connections
and can temporarily take over when a line is unavailable.

**Later upgrade option:** high-tier "rapid transfer" or fantasy/arcane
mechanism can use the same queue with one-tick travel at a limited
input/output rate; it is deliberately not a basic belt feature.

## What Knomira actually has now (prototype)

`src/base/tick_transport_line.h` provides a standalone C++17 class,
`Knomira::Logistics::TickTransportLine`, with:
- distance-dependent arrival tick or one-tick fast travel;
- minimum ticks between accepted items and delivered items;
- limited in-flight capacity;
- stable item IDs, plus duplicate-in-flight rejection;
- strict take/deliver callbacks for future authoritative inventory integration;
- explicit destination-ready notification rather than repeated failed
  destination queries each tick;
- route operational enable/disable and recoverable evacuation;
- delivery processing only when due, not per-item/per-tile movement.

`prototype/rmlui/tick_transport_line_test.cpp` tests ordinary transfer,
multiple items, blocked output, strict throughput after unblocking,
source refusal, no double delivery, evacuation/recovery, near-instant
limited-rate transfer, tick overflow, and a 20,000-item stress scenario.

This code is intentionally **not connected to the old game loop**.
The existing legacy gameplay build still depends on Noesis and the RmlUi
shell is not yet connected to the original simulation. Avoid claiming
conveyors are already usable in the game.

## Why scheduled transfers rather than moving sprites every tick

Factorio's developers documented joining belt sections, replacing items
with lower-cost line data, and using relative distance representations,
so updates need not visit every item on each line every tick.

Knomira's v1 should be simpler: treat a straight connected run as an
**event-scheduled transfer queue**. A queued item retains its identity
and an arrival tick. No moving entities, per-tile collision or full
pathfinding are needed inside the conveyor once endpoints are connected.
The renderer can interpolate belt effects cosmetically, but gameplay
doesn't depend on individual sprite positions mid-route.

This is not physically accurate Factorio two-lane belt simulation.
That's intentional: minimal implementation and low CPU overhead matter
more here than reproducing high-precision belt physics.

Developer references:
- https://www.factorio.com/blog/post/fff-176
- https://factorio.com/blog/post/fff-82
- https://wiki.factorio.com/Transport_belts/Physics

## Real inventory integration — safety first

The future inventory adapter must guarantee these authoritative states:
`AtSource -> InTransit(route,itemID) -> AtDestination` or
`InTransit -> RecoveryStorage`.

- Picking an item for the line must atomically claim it and remove it from
  haulable source capacity, without destroying its unique ID.
- While traveling, it cannot be separately hauled, sold, destroyed, equipped,
  counted twice, or sourced as a workshop ingredient.
- On successful arrival, atomically confirm destination access, filters and
  capacity; only then release the item's transit claim into that destination.
- When destination is full, line pauses. Resume from a *capacity-changed*
  event rather than retrying costly checks every tick.
- On belt disconnection or destruction, pause and safely recover every
  item or preserve its in-transit state; never delete or silently duplicate.
- On save/load, serialize each in-flight item's ID, route ID, arrival tick,
  line config and destination metadata. Never create another item identity.
  If a route no longer exists, recover from a dedicated overflow store.
- On workshop cancellation, item reservations and pending haul jobs must
  never compete with the conveyor line for the same material.
- If the route is not connected, reject enqueue before taking the source item.
  Distinct Z-levels require a valid lift/connection, not implied teleportation.
- Route topology is validated when built/changed, not for every item.

Do not call `Inventory::destroyObject` when enqueuing an item. That would
remove its identity and create opportunities for duplication on delivery.
An additive transit location/status in the authoritative inventory is needed.

## Tick and throughput rules

Suggested initial tuning **only**:
- Basic conveyor: `lengthInTiles=route length`,
  `ticksPerTile=4`, `ticksBetweenItems=2`.
- In-flight capacity: bounded by constructed belt length/segments.
- No storage: in-flight items do not automatically fill a stockpile until
  arrival. Blocked outputs must not overflow.
- Late game: `nearInstant=true`, still rate-limited and capacity-constrained.

The transport core's `Process(tick)` can be called from one future
`ConveyorManager::onTick()` per active line; it performs constant work
when no item is due and skips full destinations. At high scale, a global
next-due priority queue could wake lines only when needed.

Tick-based transfers reduce *movement simulation overhead*. They won't
automatically reduce inventory/recipe lookup costs unless the job scheduler
stops producing ordinary haul jobs for items covered by active conveyors.
Benchmark both CPU cost and the amount of gnome labor actually freed.

## Minimal implementation plan

1. **Done:** Isolated queue, deterministic tests and Windows CI target.
2. Restore Noesis-free actual simulation/new game/save/load so live items
   can be tested without changing the legacy production build.
3. Add a small `ConveyorManager` and explicit source/destination
   integration with the existing `Inventory`, `StockpileManager` and
   `WorkshopManager` interfaces. First enable only *one connected straight
   line between one source and one destination*; no splitters or crossings.
4. Add a midgame unlock and construction recipe. The inspected
   `src/game/techtree.cpp` computes item levels from the dependency graph
   (`BaseItems` and `Crafts_Components`) and writes levels to `Items`.
   **That is not itself proof of a player-facing research unlock system.**
   The low-change route is a mechanical-conveyor crafting recipe that depends
   on mid-level gears/axles/metals, placing it naturally after those materials.
   An explicit research button/gate would be a separate progression feature.
   Precise materials and recipe IDs remain to be confirmed against the DB.
5. Add construction/designation and a throughput/status screen in RmlUi,
   recycling the existing tile selection pattern rather than new editors.
6. Test capacity, filters, route breakage, power-off, parallel hauling jobs,
   save/load, multiple items, and exact inventory ID/accounting.
7. Benchmark 0, 10, 100 and 1,000 connected lines against worker hauling
   and previously defined large-colony benchmarks. Retain only net wins.

**Priority constraint:** The present RmlUi shell is not yet a playable
colony simulation. Building a giant belt network now would delay the
more important engine-migration milestones. Keep this as an isolated,
tested piece for integration once simulation services are available.
