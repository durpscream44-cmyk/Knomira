# Proposal: Preferred Routes and Workshop Logistics Links

**Status:** DESIGN ONLY — deferred until the Noesis-free executable can generate,
display, save and reload playable colonies. This is not implemented gameplay.

## Player goal

Allow players to mark preferred travel tiles using the **same designation selection
workflow already used for farms, stockpiles, rooms and no-pass areas**. The
player should not need a new standalone painting/waypoint editor.
**The primary performance goal is reusing an already solved, validated route
between recurring logistics endpoints** (e.g. stockpile and workshop), rather
than having every gnome independently recompute that same route for every trip.
Gnomes headed in a compatible direction should favor these routes when doing so
is sensible, but must never abandon their real job target to obey a route.

Allow workshops to prefer source stockpiles for input materials and preferred
destination stockpiles for finished products. Connections may include other
workshops for production chains. This is about where hauling jobs get items
and deliver outputs; it must not cause each worker to carry extra items during
unrelated jobs.

## Two independent features

### A. Preferred-path tile designations (reuse existing selection)

**Reuse, don't rebuild the farm/stockpile painting UI:**

- The existing `src/base/selection.cpp` handles `CreateStockpile`,
  `CreateFarm`, `CreatePasture`, `CreateGrove` and `CreateNoPass`.
  `Selection::setAction()`, `leftClick()`, `updateSelection()` and
  `onSecondClick()` already provide a two-click area/rectangle designation,
  per-tile validation, selection previews, and final dispatch. The
  `src/gui/aggregatorselection.cpp` maps mouse movement to world tiles and
  sends selection previews to the renderer.
- Introduce a `CreatePreferredPath` designation through this established
  action pipeline. Reuse the existing floor/walkability checks, action preview
  feedback, camera/z-level behavior, and rectangle selection conventions.
  Add erase/remove designation support through the same UI pattern.
- The initial interaction should match farm and stockpile area selection:
  **choose Preferred Path -> first corner -> second corner**. Each selected,
  valid tile becomes part of a soft travel network. Players can repeat the
  action to build thin corridors, larger walkways, junctions and branches.
  Freehand paint brushes can be added later only if useful; no need to invent
  a separate editor for v1.
- The tile zone storage and routing policy need a dedicated data owner (e.g.
  `PreferredPathManager`) rather than abusing Stockpile/Farm records. A path
  designation **must not** reserve tiles for storage, farming or work, and
  may overlap other designations wherever walkability permits.
- Start with one colony-wide painted preferred network; named sub-networks or
  workshop-scoped preferences can be introduced if benchmarks/gameplay show
  clear value. Multiple disconnected areas are allowed and must not imply
  false connections between them.
- A route may span multiple z-levels only through actual legal connections
  such as stairs and ramps; simply marking tiles on different levels does
  not create a connection.
- When the world UI is replaced with RmlUi, preserve the existing game-world
  selection logic; replace only the Noesis UI command surface.
- Mark route as **Preferred**, not **Mandatory**. A route must be optional
  whenever endpoints are not on/near it, it is blocked, dangerous, too slow or
  creates an unacceptable detour.
- Routing should consider the cost of reaching the corridor, traversal cost,
  leaving it and reaching the actual destination.
- Suggested *minimum viable heuristic*: prefer a corridor only if it shares a
  sensible segment with the ordinary route; define a tunable maximum detour
  percentage, and always preserve hazard and no-pass restrictions.
- Route tiles aren't guaranteed to remain passable: mining, construction,
  doors, water, enemies, z-level changes and designated hazards must invalidate
  affected segments promptly.
- Avoid assigning every gnome the same narrow corridor during congestion. Do
  not solve every crowded tile with expensive per-gnome re-routing.
- The overlay must show the route and whether it is active/blocked. Players
  should be able to edit, disable and delete it.

### B. Workshop logistics links

- Workshop UI: **Input stockpiles** (preferred item sources), **Output
  stockpiles** (preferred output destinations), optional **Next workshop**
  (production chain destination). Support per-input/item filters and multiple
  ranked candidates.
- The hauling/job scheduler chooses a valid stockpile/item and destination.
  When possible, movement combines a short approach path, a **shared validated
  route segment**, and a short exit/arrival path. The normal pathfinder
  remains available for initial segment creation and fallback.
- Links control logistics choices; corridors influence physical travel. The
  player can use either without needing the other.
- Do not permit item duplication, incorrect reservations, or workers hauling
  arbitrary items while completing unrelated jobs. Use existing claim/haul
  ownership and atomically verify the source and sink before job assignment.
- Use fallback stockpiles (and clearly report why) when a preferred one is
  inaccessible, full, filtered out, reserved, destroyed, or lacks inputs.
- Build production chains incrementally: stockpile -> smelter -> metal
  stockpile -> forge, never infinite workshop transfer cycles.

## Reusable workshop routes — explicit behavior

A defined route is a cached **shared travel segment**, not an instruction that
forces every gnome to start from the exact same tile. Example:

`Ore Stockpile -> validated trunk route -> Smelter -> Output Stockpile`.

- On route creation or first use, validate the corridor against current
  walkability, stair/ramp transitions, access/no-pass rules and hazards. If
  the player draws rough waypoints, solve the legal walkable segments between
  those waypoints once. The route is marked invalid if no safe legal connection
  exists.
- Store reusable segment(s) independently of the requesting gnome, keyed by
  endpoints, traversal permissions, and a navigation/terrain version. All
  compatible haul jobs may reuse them.
- For each gnome, only the **approach to the reusable segment** and the **final
  approach to the actual pickup/dropoff tile** may require new local searches.
  A gnome standing directly on the route can often join with negligible
  search work. Do not promise zero pathfinding for every haul.
- Before reusing a segment, validate route revision and required movement
  constraints. When map topology or access permissions change, invalidate
  affected segments **once per route/version**, rather than running a
  complete path search for every gnome that attempts to use it.
- Compare estimated total travel cost (approach + cached route + arrival)
  with ordinary travel, with a configurable detour threshold. Do not choose
  a route that is blocked, unsafe, much longer, or incompatible with the job.
  A route can be preferred for the inbound supply trip but not a gnome's
  unrelated return journey.
- When a route is invalidated, gnomes already traveling must safely switch
  to fallback A* at the next relevant decision point. Avoid retry storms:
  a failed route remains inactive until something relevant changes or a
  bounded revalidation is requested.

## Workshop route monitoring and player alerts

A workshop's **Logistics / Routes** panel shows both the selected source and
destination links and the status of each reusable travel route:

| Route status | UI and behavior |
|---|---|
| Available | The preferred segment is valid and may be reused |
| Blocked | A construction, door/access rule, obstacle, or dangerous tile broke the segment; normal pathfinding takes over |
| Unreachable | No legal connection remains to the linked stockpile or workshop; hauling jobs using that link cannot proceed until another destination or access path is found |
| Detour fallback | A normal path is available but the designated corridor would take too long |
| Awaiting validation | Newly created or invalidated path is being checked |

- Include a short **reason**, affected source and destination, and where
  possible the first problem tile / z-level with a **Show on Map** action.
- Send one actionable workshop alert when a previously usable link becomes
  blocked or unreachable; group repeat alerts instead of sending one per
  gnome or job. Notify again only on meaningful state change, and report
  restoration when the route becomes usable.
- Distinguish **route blocked but fallback available** (warning) from
  **workshop cannot receive materials / deliver output at all** (problem).
  These must integrate with the planned workshop status panel alongside
  missing required inputs, output storage full and missing assigned worker.
- If a preferred stockpile is unreachable, obey the workshop's fallback
  rules (e.g. alternative source/destination). Never quietly lose reserved
  items, duplicate transport jobs, or silently mark goods delivered.
- Route topology changes should trigger **event-driven** invalidation and
  revalidation (affected tile, door/permission, building/storage changes).
  Avoid checking every route on every simulation tick.

## How painted tiles turn into reusable routes

- A designation marks **acceptable/preferred tiles**, not a sequence that
  magically eliminates A*. For repeated workshop/stockpile trips, identify
  connected painted corridors/junctions and cache safe traversable segments
  through them. Short approaches from a gnome to the corridor and from the
  corridor to the actual item/job destination may still need a search.
- Cached routes are **derived from** the designation network and actual
  walkable terrain; don't store hardcoded instructions tied to an individual
  worker. Invalidate the affected cache when the selection, doors, stairs,
  structures, walkability or access rules change.
- Maintain the original default A* when a painted route is blocked, unsafe,
  disconnected, a large detour, or does not apply to the worker's target.
  The existing `src/base/pathfinder.cpp` already batches some searches,
  so benchmark before claiming reductions in CPU load.
- A workshop linked to a preferred path should show a disruption alert when
  a cached route becomes blocked/unreachable, and a distinct warning when
  a fallback is available. Suppress repeated identical alerts.

## Potential pathfinding optimization — hypothesis, not a guarantee

The current `src/base/pathfinder.cpp` already supports fast trivial paths,
RegionMap reachability checks, queued A* searches and batching of requests
sharing a start or goal (including reversed searches). `PathFinderThread`
reuses an explored field across grouped goals. Player-defined routes do not
automatically make these searches cheaper; forcing waypoints may create MORE
searches and longer paths.

A real optimization would require one or more of:
- Reusable path segments between fixed hubs (workshops, stockpiles, stairways,
  main intersections), with reachability/path versioning.
- Hierarchical navigation: local approach -> shared corridor graph -> local
  destination, falling back to ordinary A* when the shared route isn't helpful.
- Reuse of multi-goal/flow-field computations for workers headed to a common
  destination, with invalidation when topology or access permissions change.
- Scheduling and measuring costly path requests, not adding per-tick checks
  to every gnome.

Do not introduce these techniques until a representative large-colony
benchmark shows they reduce CPU time and improve or preserve completed work.

## Acceptance tests

1. Preferred Path uses the existing zone selection interaction and preview:
   select first corner, second corner, then inspect designated valid tiles.
   Erasing/extending one area does not change neighboring farm or stockpile
   designation ownership. Multiple zones can form branching corridors.
2. Suggested corridor is used for compatible destinations and ignored for
   unrelated/long-detour destinations.
3. Repeated stockpile-to-workshop hauling reuses a validated shared route
   without running a full A* search for each identical trunk segment.
4. Blocked or hazardous corridor falls back to a safe ordinary path and
   does not trap a gnome in repeated retry loops.
5. Stockpile A supplies workshop B and workshop output goes to C; item
   filters, reservations and output capacity are respected.
6. When the player closes a passage or removes a stockpile, linked workshops
   receive a single actionable blocked/unreachable alert; the map highlights
   the affected route. Alert spam is suppressed and recovery is reported.
7. Blocked route with valid ordinary alternative remains productive and is
   labeled **fallback active**; no valid route stops affected hauling safely.
8. Save/load preserves route definitions and logistics links; removed buildings
   or tiles are resolved without corrupting jobs.
9. With routes disabled, existing movement behavior is unchanged.
10. Benchmark path searches, total A* nodes expanded, pathfinding CPU p95, haul
   throughput, distance walked, and idle/stuck gnomes for small and large
   colonies. Compare baseline, corridors-only, links-only and both enabled.
   Favor net improvement under representative production chains.

## Development priority

**Post-playable milestone.** First finish Noesis-free engine linking,
world rendering, new game, save/load and a minimally usable colony HUD.
A workshop-specific choice of input/output stockpiles can ship before
route corridor editing, since it is simpler and often useful even without
pathfinding speed gains.
