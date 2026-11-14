# Proposal: Preferred Routes and Workshop Logistics Links

**Status:** DESIGN ONLY — deferred until the Noesis-free executable can generate,
display, save and reload playable colonies. This is not implemented gameplay.

## Player goal

Allow players to designate suggested corridors by selecting tiles or waypoints.
Gnomes headed in a compatible direction should favor these routes when doing so
is sensible, but must never abandon their real job target to obey a route.

Allow workshops to prefer source stockpiles for input materials and preferred
destination stockpiles for finished products. Connections may include other
workshops for production chains. This is about where hauling jobs get items
and deliver outputs; it must not cause each worker to carry extra items during
unrelated jobs.

## Two independent features

### A. Soft path corridors

- Player selects a corridor by dragging across passable tiles or specifying
  connected waypoints; a named route may span multiple z-levels.
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
  The actual pathfinder still computes a safe path to the selected endpoint.
- Links control logistics choices; corridors influence physical travel. The
  player can use either without needing the other.
- Do not permit item duplication, incorrect reservations, or workers hauling
  arbitrary items while completing unrelated jobs. Use existing claim/haul
  ownership and atomically verify the source and sink before job assignment.
- Use fallback stockpiles (and clearly report why) when a preferred one is
  inaccessible, full, filtered out, reserved, destroyed, or lacks inputs.
- Build production chains incrementally: stockpile -> smelter -> metal
  stockpile -> forge, never infinite workshop transfer cycles.

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

1. Suggested corridor is used for compatible destinations and ignored for
   unrelated/long-detour destinations.
2. Blocked or hazardous corridor falls back to a safe ordinary path and
   does not trap a gnome in repeated retry loops.
3. Stockpile A supplies workshop B and workshop output goes to C; item filters,
   reservations and output capacity are respected.
4. Save/load preserves route definitions and logistics links; removed buildings
   or tiles are resolved without corrupting jobs.
5. With routes disabled, existing movement behavior is unchanged.
6. Benchmark path searches, total A* nodes expanded, pathfinding CPU p95, haul
   throughput, distance walked, and idle/stuck gnomes for small and large
   colonies. Compare baseline, corridors-only, links-only and both enabled.
   Favor net improvement under representative production chains.

## Development priority

**Post-playable milestone.** First finish Noesis-free engine linking,
world rendering, new game, save/load and a minimally usable colony HUD.
A workshop-specific choice of input/output stockpiles can ship before
route corridor editing, since it is simpler and often useful even without
pathfinding speed gains.
