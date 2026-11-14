# Squad Leader AI and Formations — design specification

**Status: DESIGN ONLY; NOT IMPLEMENTED.** This extends
[Preferred Routes, Workshop Logistics and Military Patrols](DESIGN_PREFERRED_ROUTES_AND_LOGISTICS.md).
Do not prioritize ahead of Noesis-free playable engine, correct saves, basic
military combat and initial patrol orders.

## Player-facing goal

The player assigns a squad to a reusable cached patrol circuit, chooses a
formation, and optionally selects a leader. The squad shares patrol planning
and broad threat coordination instead of every member independently solving
the same long-distance route or repeating identical threat-candidate searches.

Default formation: **Single file**. Player can select:
- **Single file:** 1-column trailing formation; arbitrary member count.
- **2×2:** first four soldiers form a compact two-column square; extra members
  extend in two-column ranks behind. Use "2-wide" internally for larger squads.
- **3×3:** first nine soldiers form a three-column square; extra members
  extend in three-column ranks behind. Use "3-wide" internally for larger squads.

A leader is normally in the front rank. For even-width ranks, choose a
deterministic front-left/front-right leader anchor and rotate offsets by
travel heading. Squad size can be 1, 2, 3, 4, 5, 9, 12 or much larger without
silently excluding members or placing two gnomes on the same tile.

## One squad plan, lightweight member autonomy

**SquadController** (new; implementation name tentative) owns:
- squad ID, leader ID, selected formation (width 1/2/3), spacing and policy;
- patrol circuit/route ID, version, current segment and moving heading;
- current squad order/alert/threat candidates and reevaluation schedule;
- re-formation state and once-per-transition status notifications.

**Shared navigation:**
- Leader or squad controller requests the long-range path through the
  existing world/pathfinder services *once per route segment/version*,
  reusing the proposed cached patrol segments.
- Followers select dynamic world positions by rotating predefined slot
  offsets with the leader's facing/route direction and by tracking a short
  history of leader movement. Follow-through history is generally preferable
  for corners; a direct offset at a sharp bend may send a soldier into a wall.
- Each follower has small local steering/short path correction, occupancy
  avoidance, and a bounded catch-up plan. Do not run a fresh global A* search
  every simulation tick for every soldier.
- The shared route is not a single giant rigid collision body; soldiers still
  have independent positions, needs, movement cooldowns and injuries.
- Followers too far from the squad request an individual safe join path
  at a rate-limited interval, then return to cheap formation following.

**Shared broad threat assessment:**
- Use an existing/introduced spatial index to gather potential enemies in
  the **squad's swept footprint plus relevant detection range** on an
  event-driven or staggered schedule; don't make every soldier perform
  the same large-area candidate enumeration.
- The leader/controller evaluates squad orders and alert/target priorities,
  then broadcasts potential threats and tactics. Followers may relay sightings
  back to squad even if the leader cannot see the enemy.
- Individual soldiers MUST retain their own line of sight, valid attack range,
  cover/weapon checks, immediate self-defense, injury/flee decisions and
  reaction to close-range dangers. Don't let the leader's sight alone decide
  what a follower is allowed to detect or attack.
- Periodic broad checks can run less frequently while calmly patrolling,
  but immediate local/contact events and active combat run at appropriate
  higher priority. Measure detection latency; never reduce responsiveness
  just to improve a CPU benchmark.
- Squad-wide assignments should avoid all members choosing the same target
  or reevaluating every tactical option every tick.

## Formation adaptation and tactical safety

- Width 2/3 is a **preferred** formation, not a guarantee. A 1-tile doorway,
  stairs, ramps, blocked tile, cliff edge, friendly obstruction or hazard
  triggers temporary **single-file compression** or a narrower configuration,
  then safe re-expansion in wider space.
- Do not require a 3-wide movement footprint to traverse every tile the
  leader can traverse. Either check route corridor clearance before
  entering or adapt formation width dynamically.
- Keep members in their own legal, unoccupied tiles. If a preferred slot
  becomes blocked, the member stops, takes a bounded local detour, or
  switches to a following queue. No teleport, overlap, repeated pathfinding
  storm, or deadlock.
- Formation direction is relative to current movement vector/route heading,
  not a fixed compass direction. For turning, use leader path history and
  rank spacing to prevent followers from shortcutting through walls.
- Combat overrides patrol formation as required by the situation. A squad
  may hold, spread, pursue, defend, flee or regroup according to its orders;
  it should not march in a square while being attacked simply because patrol
  formation is active.
- Rest, sleep, injury, loss of path access, alarms and manual player orders
  supersede normal patrol following. Assign an eligible replacement leader
  deterministically if the leader dies, becomes unreachable, is incapacitated
  or leaves the squad. Prevent per-tick leader election churn.
- No squad should require its commander physically alive/present for
  individual soldiers to avoid hazards or defend themselves.
- Larger formations can split into subgroups/fireteams later only if profiling
  or gameplay shows a real need. Start with one squad controller.

## UI and persistence

- Military -> Squads -> Patrol: route picker using existing painted patrol
  designations, **Formation** selector: Single file (default), 2×2, 3×3;
  leader selection (default auto), status/alert, rally and re-form options.
- Map preview shows the leader, designated route, a ghost formation footprint
  at the cursor, and a small compression/fallback indication for narrow gaps.
- Save squad patrol ID, leader preference, formation, member rank order,
  spacing, behavior and interruption state in a versioned schema.
  Cached paths are disposable derived data and should be rebuilt lazily on
  load; don't persist stale terrain geometry as authoritative truth.
- When editing/removing a route or squad, validate references and emit a
  single actionable notification (route interrupted, leader changed, unable
  to re-form); don't spam one message per gnome.

## Existing Knomira integration points

Verified on `feature/controls-menu-redesign`:
- `src/game/militarymanager.h`: `Squad` stores ID, name, member gnome IDs
  and target priorities; it has serialization.
- `src/game/militarymanager.cpp`: `MilitaryManager::onTick()` currently
  starts a timer and performs no squad AI work; patrol/leader/formation
  controllers do **not** already exist.
- `src/game/gnome.cpp`: gnomes use individual behavior-tree conditions for
  combat and needs; do not bypass this for formation orders.
- `src/game/creature.cpp`: `Creature::moveOnPath()` advances an individual
  current path and checks walkability and door access. It is an appropriate
  movement safety boundary, not a place to blindly teleport followers.
- `src/base/pathfinder.cpp`: asynchronous A* search and grouping/caching
  behavior already exist; reuse the shared navigator rather than writing
  a second pathfinder inside MilitaryManager.

## Tests and performance gates

1. Squad size 1/2/4/5/9/12/30/100 with formation width 1/2/3: all soldiers
   receive stable slots; changes preserve member assignments and don't
   overlap tiles.
2. The 2×2 and 3×3 previews match actual occupancy in open terrain. The
   2-wide/3-wide pattern extends in ranks for larger squads; turning
   preserves follower order as practical.
3. Patrol a narrow gate, corner, ramp and stairs: compress to single-file
   and restore formation without deadlock or duplicate searches.
4. Leader dies/disconnects mid-patrol: deterministic replacement, remaining
   squad continues or safely pauses with one alert, no crashes.
5. Enemy spotted by a follower outside leader sight: report propagates and
   soldier self-defense/LOS works. No squad-wide shared vision that permits
   shots through walls or suppresses emergency detection.
6. Combat, alarms, sleeping and injury interrupt patrol; resumption reaches
   a legal checkpoint without repeatedly restarting the route.
7. Save/load and patrol-route modification preserve formation and squad
   membership; reconstruct derived cache safely.
8. Benchmark 10/50/200 patrolling soldiers and 1/5/20 squads against per-gnome
   independent patrol paths. Measure global A* requests, explored nodes,
   wall-clock pathfinding time, total simulation CPU, memory, maximum/average
   patrol spacing, threat-detection latency, stuck/blocked soldiers, and
   completed patrol laps. A pathfinding-only improvement accompanied by
   excessive follower-AI overhead is **not** a net performance gain.
9. Provide a compatibility toggle/benchmark comparison with independent
   movement and threat checks to catch regressions.

## Implementation sequence

1. After playable engine and basic squad orders: store formation selection
   and deterministic leader/member rank order.
2. Support a single-file shared patrol circuit and follower path history.
3. Add 2-wide/3-wide formation offsets and narrow-corridor compression.
4. Add squad-level broad-phase threat candidate checks while retaining
   individual local reactions.
5. Add deterministic large-patrol benchmarking and only then optimize
   scheduling, shared route reuse, or fireteams based on measured bottlenecks.

This is a **performance-oriented design hypothesis**, not a claim that the
existing military AI already supports it or that it saves a fixed CPU percent.
