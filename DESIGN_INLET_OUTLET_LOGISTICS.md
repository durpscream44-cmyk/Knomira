# Knomira — Inlet/Outlet Stockpile Logistics Network (preferred v1)

**Status:** proposed gameplay; not yet connected to the simulation.
**Goal:** reduce hauling work and pathfinding overhead with minimum changes to
existing items, jobs, stockpiles, and workshops.

## Player experience

At a midgame mechanical/industrial milestone, the player unlocks:

- **Logistics Inlet** adjacent to or assigned to a stockpile; exposes items
  that stockpile is allowed to provide under its existing item filters.
- **Logistics Outlet** attached to a workshop input, output, or stockpile.
- **Conduit/connection** between the nodes (or an explicit network assignment
  in the first prototype). Conduit geometry establishes connectivity only:
  there are NO moving items on conduit tiles and no per-tile conveyor tick.

Example:
`Ore Stockpile [Inlet] === Network === [Outlet] Smelter [Inlet] === Network === Finished Metal Stockpile`

When the smelter requires ore, the logistics network can fulfill its
ordinary job input request directly from a connected source stockpile
without assigning a gnome to haul it. A workshop may return output to
a connected, filtered and non-full stockpile. Disconnected buildings still
use the existing hauling system.

## Why this is easier than animated belts

Unlike a Factorio-style physical belt, there is no path, lane, direction,
intermediate item sprite, item collision, movement cooldown, junction,
splitter or tile-by-tile transport state to update. The network only needs:
1. topology/connectivity, recomputed when nodes/conduits change;
2. filtered source-item lookup in authoritative Inventory;
3. bounded request/transfer scheduling and atomic claiming;
4. target capacity and item-location correctness.

**Connectivity is cached** by network ID or connected component; it is not
recalculated for every item. A disconnected conduit prevents network
requests; it does not cause items to fall into a nonexistent transit queue.

## Two transfer semantics; pick one before live integration

**Recommended v1: instant on-tick item transfer to the workshop's real
input tile or output stockpile.**

- A connected job requests `(item type, material, count)`.
- Locate eligible *actual* item IDs in the linked stockpiles; preserve their
  filters, priorities, pull permissions and existing reservation state.
- Atomically reserve the IDs for the real craft job. Validate all required
  materials as a set before consuming a transfer allowance.
- Deliver the same IDs to an appropriate input location by using a safe,
  audited inventory move/stockpile update operation; alternatively, let the
  job consume a reserved input directly only if job and save/load semantics
  have been verified end-to-end.
- Update existing job delivery state so the job becomes READY via its
  current phase machine. Do **not** also create a `HaulToSite` subjob for
  those IDs. If the network cannot fulfill the job, fall back to normal
  hauling according to an explicit policy.
- Complete the move in a single game-thread transaction in one tick. There
  is no item "in transit" between save ticks, simplifying save/load.
- The destination still has to be accessible and have capacity, and
  completed output must be a real item in one real stockpile or workshop.

**Alternative: shared remote inventory consumption.** A workshop consumes
items in their original stockpile without changing tile location. This is
simpler visually but risks violating game code that expects ingredients to
arrive at `posItemInput` before crafting. Defer until tested, rather than
silently bypassing jobs, inventory history, or reservation cleanup.

## Throughput and balance

- **Basic network:** configurable item deliveries per N simulation ticks,
  initially suggested 1 item per 2 ticks per network or outlet. This makes
  transport instant per item but NOT infinite in volume.
- Optional network cap, power/axle cost, construction resources and storage
  throughput. Do not add expensive global scans on every tick.
- Work requests arrive as events (new job, produced output, new stock,
  released reservation, filter/capacity/topology change) and are queued.
- Rate limits are applied only at due ticks; a full destination sleeps until
  a capacity-changed event. Use a fair queue to avoid one workshop starving
  all others.
- Later upgrade: faster transfers or more outlets/throughput, without
  introducing physical item movement unless players actually want belts.

## Integrity and edge-case gates

- Exactly one authoritative item record and unique item ID throughout.
  Source stockpile access is **not a copy of its contents**.
- No double-claim with ordinary hauling, workshop jobs, trading, equipment,
  containers or other logistic outlets. Preserve atomic reservations.
- Stockpile allow/pull rules, material filters, priority, free capacity and
  blocked/disabled/suspended states are enforced before committing.
- Removing a conduit or destroying an inlet/outlet cancels uncommitted
  network requests; already committed transfer remains valid. No item
  stranded in a non-serialized intermediate state.
- Save network memberships, inlet/outlet config, throughput, and any durable
  pending requests with versioning. Do not save caches as authoritative.
  Restore a valid network graph after loading.
- If a source runs out or a destination fills, wake normal hauling when
  appropriate and report one actionable alert (no spam per item).
- Inventory queries must remain immediately accurate; UI may be updated
  using change notifications but never display stale inventory truth.

## Knomira source integration points (inspected)

- `src/game/jobmanager.cpp`: `processJobPhases` already atomically claims
  required item IDs, creates `HaulToSite` jobs, and marks a parent job READY
  after delivery counters reach the required total. Hook network delivery
  immediately before haul-subjob creation rather than replacing the job
  manager. Watch pending-claim retries.
- `src/game/workshop.cpp`: craft jobs already define `posItemInput` and
  `posItemOutput`.
- `src/game/inventory.cpp`: items have unique identities, claim indexes,
  stockpile and position indexes. `pickUpItem` / `putDownItem` alter item
  state; **do not invoke with a fake creature** or blindly teleport, because
  existing owner/stockpile/wealth hooks must be audited.
- `src/game/stockpile.cpp`: filters, priorities, capacities and reserved
  items must remain authoritative.
- `src/game/techtree.cpp`: technology level is derived from craft-component
  depth; a recipe-based midgame unlock is lower risk than inventing a new
  research manager.

## Minimal implementation sequence

1. Keep playable Noesis-free engine and New Game/Save/Load integration first.
2. Implement a small, simulation-only **LogisticsNetwork** connection graph
   storing network IDs and inlet/outlet endpoints. Verify connected/disconnected
   behavior without touching live inventory.
3. Add a transaction-safe one-item transfer adapter to Inventory and
   StockpileManager with deterministic tests (same ID, no duplication,
   claim ownership, filters and storage capacity). Do not use item destruction.
4. Connect one stockpile inlet to one workshop outlet and satisfy a single
   live craft input without generating a hauling job. Test empty inputs,
   full/blocked destinations, cancellation, ordinary hauling fallback,
   save/load during active crafting.
5. Implement player construction/designation and a midgame crafting recipe.
6. Benchmark CPU and game-day hauling throughput with 10/100/1000 transfers;
   then support multi-node networks and output-to-stockpile routing.

## Existing experimental belt queue

`src/base/tick_transport_line.h` and its standalone 20k-item test are
already present on this branch and passed the isolated Windows CI. **They are
NOT required for this simpler instant network** and do not move live game
items. Keep them as an optional transport-time/rate-limit experiment, but do
not integrate them ahead of getting the basic colony playable.

No performance gain has been measured, and this document is not evidence that
a functioning logistics network exists in the game.
