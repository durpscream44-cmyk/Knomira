# Squad Patrol Performance — shared navigation and selective AI optimization

**Status: DESIGN ONLY; NOT IMPLEMENTED.** Extends
[Preferred Routes, Workshop Logistics and Military Patrols](DESIGN_PREFERRED_ROUTES_AND_LOGISTICS.md).
Do not prioritize ahead of the playable Noesis-free engine, save/load reliability,
and working basic military commands.

## Scope

Optimize patrols by **removing redundant work**, not by adding formation
simulation. There are **no formations** in this design: no single-file,
2×2, 3×3, follower slots, leader-relative offsets, keeping rank spacing,
formation squeezing, or constant regrouping calculations.

Squad membership remains useful for assigning patrol circuits, attack/defend
rules, alerts and selected orders. Gnomes move individually and retain normal
collision, path-access, combat, needs and survival behavior. A squad leader,
if supported as an optional command role, is not a moving anchor that followers
must track. An elected controller/leader can coordinate orders; the shared
navigation cache must also work without a living or reachable leader.

## Cached squad patrol navigation

- Players designate patrol tiles using the existing farm/stockpile-style
  selection controls, then set ordered patrol checkpoints. Refer to the parent
  document for how the circuit is generated and painted.
- Solve and validate a patrol segment **once** for each terrain/permissions
  revision, then reuse the segment for any compatible squad member (or squad).
  A shared route is a read-only sequence of legal tiles, not a gnome-specific
  reservation or physical formation.
- Each soldier keeps only minimal per-agent state: route ID, segment/checkpoint,
  direction, current position, and an interruption flag. A soldier joining
  from elsewhere may require a limited local search to reach the route.
- Keep using the existing `PathFinder` and its batching/reuse logic. Do not
  create an independent military A* implementation.
- Invalidate only affected cached segments when tiles, ramps, doors, access
  rules or patrol zones change. Deduplicate revalidation instead of launching
  one full pathfinding job per soldier. Respect hazards and movement access.
- If the shared path becomes invalid, reuse a new safe replacement if one
  exists; otherwise use ordinary per-gnome movement or suspend that patrol
  with one squad-level warning. Combat and emergencies always override
  scheduled movement.
- Soldiers may naturally spread out, walk in clumps or follow different parts
  of the circuit. Do not add CPU work to enforce a visual pattern.

## Threat-detection optimization (only if benchmarked)

- Existing individual AI must retain **immediate local threat awareness**,
  line-of-sight/range checks, self-defense, survival and target validity.
  Do not make the squad blind outside the leader's vision.
- If profiling reveals repeated expensive candidate-enumeration work, run one
  **squad-level broad-phase nearby-enemy query** using an efficient spatial
  index, then share potential targets with squad members; each individual
  performs only necessary local checks.
- A gnome can report a threat that the leader does not personally see.
  Nearby attacks, alarm events and ongoing combat should never wait for the
  next low-frequency squad scan. Preserve target-priority behavior.
- Coalesce redundant target notifications and squad-order reevaluations.
  Avoid adding a new full-squad tick scan if the existing per-creature code
  already handles the relevant case cheaply.
- Treat shared perception as an **optional optimization**, not a dependency
  of the initial patrol feature. Implement only when profiling demonstrates
  net savings and acceptable detection latency.

## Existing code boundaries

As inspected on `feature/controls-menu-redesign`:
- `src/game/militarymanager.h/.cpp`: squad membership, target priorities,
  persistence; `MilitaryManager::onTick()` currently does not run patrol AI.
- `src/base/pathfinder.cpp`: queued asynchronous A* and grouping of searches
  sharing endpoints. New cache must complement, not duplicate, these.
- `src/game/creature.cpp`: `Creature::moveOnPath()` checks walkability and door
  access per soldier and must continue to prevent illegal moves.
- `src/game/gnome.cpp`: existing individual behavior-tree needs and combat
  conditions remain authoritative for each soldier.
- Reuse original world selection/area designations, not a second drawing UI.

## Acceptance tests and performance gate

1. Patrol 10, 50 and 200 gnomes sharing a loop; record pathfinder requests,
   A* nodes expanded, total pathfinding CPU and **whole-simulation** CPU,
   memory, patrol laps, congestion, combat response time and stuck gnomes.
2. Compare baseline individual patrol paths, cached patrol paths, and
   optional shared threat-candidate detection. Only ship optimization if
   whole-simulation performance improves without significant regressions.
3. A blocked route should invalidate shared cache once per affected revision,
   safely replan/fallback, and send at most one actionable squad alert.
4. A soldier detached for combat, injury, sleep or an emergency order must
   still respond correctly and rejoin its assigned circuit when appropriate.
5. A missing leader, changing squad membership, and save/reload cannot strand
   soldiers or invalidate unrelated patrols.
6. A follower seeing an enemy outside the leader's vision still responds
   promptly; no shared-vision shortcuts permit attacks without individual LOS.
7. With shared patrol optimization disabled, behavior returns to the
   existing independent navigation without additional overhead.

## Implementation order

1. Finish the playable core and basic military patrol orders.
2. Add versioned shared patrol segment caching and route-invalidated alerts.
3. Benchmark at scale; fix measured cache or movement bottlenecks.
4. *Only if needed:* add shared broad-phase threat candidate queries.
5. Keep formations explicitly **out of scope** unless the player later asks
   for them as a separate, performance-justified feature.
