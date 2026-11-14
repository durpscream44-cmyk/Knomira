# Knomira — Colony Simulation Performance Research

**Date:** November 13, 2026
**Status:** Research, source-level audit and proposed experiments; **not measured performance results**.
**Scope:** Large gnome/animal populations, items, jobs, workshops, hauling,
stockpiles, combat patrols and world simulation. Do not postpone a basic
playable Noesis-free game for extensive rewrites. No military formations.

## Evidence boundaries

- Primary: published statements from Factorio developers; Tarn Adams'
  Dwarf Fortress interview; official Klei release notes; Songs of Syx
  developer forum; KeeperRL open-source game logic.
- Comparative illustration: an older **decompiled RimWorld alpha** shows
  Normal/Rare/Long tick categories and priority work-giver structure; not
  evidence of every current RimWorld release.
- Knomira: direct code reading on `feature/controls-menu-redesign`. Hotspot
  labels below are **hypotheses** until instrumented and run under load.
- Borrow architectural **ideas**, independently implement them for Knomira.
  Do not copy proprietary RimWorld, Factorio, Dwarf Fortress or Klei code.
  Check licensing before adapting KeeperRL code. Respect Ingnomia/Knomira's
  existing open-source license and attribution obligations.

## Lessons from comparable games

### Dwarf Fortress — decouple immediate actions from expensive assignment

In a 2019 first-person interview, Tarn Adams described multiple simulation
cadences: job assignments and applications roughly every hundred ticks on
staggered schedules; stockpiles and location updates on other staggered
frequencies; active creature actions each tick; and optimizations to skip
unchanged portions of the world. He explicitly called out large item counts
(50,000+ boulders) as a problem.

**Adapt:** Keep exact movement, hazards, combat and inventory mutations;
schedule idle decision reevaluation, job selection, low-frequency needs,
stockpile candidate refreshes, and periodic world work separately.
Wake deferred work immediately on events that matter. Do not blindly postpone
urgent work just because a timer hasn't elapsed.

Source:
https://www.gamedeveloper.com/design/q-a-dissecting-the-development-of-i-dwarf-fortress-i-with-creator-tarn-adams

### RimWorld — categorized ticking and filtered work candidates

A publicly accessible decompile of **older RimWorld alpha** `TickManager`
separates objects into Normal, Rare, Long and Never update categories;
`JobGiver_Work` shows prioritized work-giver dispatch. These are structural
examples, **not a claim about exact code in RimWorld 1.6/2026**.

**Adapt:** Explicit update categories (Immediate, Periodic, Dirty/Event, Never)
and filtered job indexes by skill/type/area. Stagger periodic checks by stable
entity ID. Ensure accumulated time is accounted for correctly if needs or
growth integrate over skipped ticks.

Sources:
https://github.com/josh-m/RW-Decompile/blob/master/Verse/TickManager.cs
https://github.com/josh-m/RW-Decompile/blob/master/RimWorld/JobGiver_Work.cs

### Oxygen Not Included — optimize chore evaluation and navigation contention

Klei's November 2024 update notes describe precondition evaluation suitable
for some multithreading, reductions in pathfinding lock contention,
NavGrid update speed/allocation changes, and duplicants prioritizing next
errand lookup after previous task completion.

**Adapt:** Filter obviously invalid job candidates before expensive reachability
or item checks; measure lock waiting and allocation counts; make a newly idle
gnome find work promptly. Introduce multithreaded read-only checks only when
state ownership/determinism are proven, not as the first optimization.

Source:
https://kleiforums.com/game-updates/oni-alpha/640445-r2418/

### Factorio — path caches, failed-search caches, fair budgets, sleeping work

Factorio's developers documented reusing *middle sections* of a previously
computed path by calculating only local joins, and caching "no route" results.
They also documented bounded pathfinding work per tick and a general
strategy of putting inactive entities to sleep and grouping identical
calculations. The current public API documents explicit path-cache and
per-tick work-budget settings.

**Adapt:** Versioned shared path segments for frequent stockpile-to-workshop
routes and squad patrols; cached disconnected-region failures; local join
searches; queueing and priority budgets if pathfinder saturation is measured.
Wake inactive/unchanged workshops on input, order, capacity, reservation,
access, construction or time events. Keep path safety checks and fallback.

Sources:
https://www.factorio.com/blog/post/fff-121
https://www.factorio.com/blog/post/fff-117
https://www.factorio.com/blog/post/fff-148
https://lua-api.factorio.com/latest/concepts/PathFinderMapSettings.html

### Songs of Syx — multi-resolution navigation

The Songs of Syx developer explained in a public forum reply that navigation
uses multiple grid levels, including 8x8, 16x16 and 32x32 tiles; this reduced
memory/processing while accepting some route-quality tradeoffs and update
complexity. This is a developer account, not inspected source code.

**Adapt later:** Only if large-colony benchmarks show current RegionMap /
A* cannot meet goals, add hierarchical routing via connected areas and
cached portal-to-portal segments, with correct revalidation after terrain
changes. Don't introduce expensive multi-level path logic before that gate.

Source:
https://steamcommunity.com/app/1162750/discussions/3/3419934814481362952/

### KeeperRL — designated areas produce and retire work

The GPL-licensed KeeperRL `Collective::updateGuardTasks` creates missing
guard-zone tasks, deletes tasks for no-longer-designated positions, and
raises a warning when no eligible fighters can perform guard jobs.

**Adapt:** Painted patrol and logistics designations should own persistent
task/route definitions that are invalidated by events, rather than inviting
individual gnomes to scan all tiles to rediscover the same obligation.

Source:
https://github.com/miki151/keeperrl/blob/master/collective.cpp

## Knomira source audit: specific candidate bottlenecks

Audited on `feature/controls-menu-redesign`, not dynamically profiled.

| Location | Observed code pattern | Experiment, not blind rewrite |
|---|---|---|
| `src/game/game.cpp: Game::loop` | Calls grass, plants, creatures, gnomes, jobs, stockpiles, farming, workshops, other managers, water and pathfinding on every active simulation loop; emits multiple UI signals | Add subsystem timers/counters, change-driven UI signals, and scheduled noncritical work |
| `src/game/jobmanager.cpp: JobManager::processJobPhases` | Visits every registered job; PENDING jobs attempt atomic item claims; unsuccessful attempts remain PENDING for next tick | Track only dirty/pending IDs due for recheck; react to inventory/claim changes; maintain bounded fallback retry |
| `src/game/jobmanager.cpp: JobManager::getJob` | Per-gnome skill/priority/candidate iteration and resource/reachability checks | Filter by indexes and available candidates first, then expensive checks; audit queue consistency |
| `src/game/inventory.cpp` | Already indexes by item ID, type/material, position and octree; updates many indexes on creation/destruction | Preserve exact counts and ownership. Profile repeated type/material/spatial queries and maintain indexes incrementally |
| `src/game/stockpile.cpp: Stockpile::onTick` | Candidate refresh when filter changes or candidate list is empty/stale; `getJob` consumes candidate list | Keep refresh event-driven; wake when inventory/capacity/filter changes, avoid repeated full candidate rebuilds |
| `src/game/creaturemanager.cpp: CreatureManager::onTick` | Iterates all creatures, start index reset each tick; creature-at-position queries scan the full creature list | Distinguish urgent vs idle creature AI; add occupancy/spatial index only if these scans are hot |
| `src/game/gnome.cpp: Gnome::onTick` | Gnome behavior tree executes in ordinary tick, along with needs, state checks, movement | Profile tree cost; avoid reselecting jobs unnecessarily; retain immediate hazards, combat and needs-state transitions |
| `src/base/pathfinder.cpp: PathFinder::findPaths` | Groups some shared endpoints; launches std::async tasks and waits for all futures in the game loop; result callback scans outstanding jobs | Measure task count, waits, lock contention and result matching. Consider bounded worker pool and keyed completion matching |
| `src/game/workshopmanager.cpp: WorkshopManager::onTick` | Iterates every workshop every tick | Enable active/idle distinction and wake-on-inventory/order/worker event if profiling warrants |
| `src/game/world.cpp` | Water tracks active water tiles and grass uses candidate tiles; some optimizations already present | Preserve active lists and benchmark before replacing water/grass code |

### Critical potential hotspot: pending job retry storm

`Game::loop -> JobManager::onTick -> processJobPhases` iterates all jobs.
Pending jobs request closest matching materials via the inventory; when
requirements are absent, the job can retry next tick. If many pending
construction/workshop jobs require absent inputs, repeated spatial/reachability
queries may dominate. **Verify with counters first.**

Proposed safe fix:
1. Dirty/due set for PENDING jobs, excluding READY/HAULING/finished jobs from
   pending resource evaluation.
2. Initial claim attempt immediately; on failure record missing item/material
   requirements and schedule a bounded fallback retry.
3. Wake affected jobs on item creation, relocation, reservation release,
   stockpile changes, accessibility changes, and crafting availability.
4. Preserve **atomic item claims**, release on failure, prevent duplicate
   haul subjobs, and clear obsolete subscriptions on cancel/death/load.
5. Verify material availability and correctness *immediately* after a relevant
   event; never use stale bookkeeper inventory totals.
6. Test demand surges, pickup races, missing input types, load/reload, player
   edits and pending thousands of jobs.

### Critical potential hotspot: path search queue / game-thread stall

The current pathfinder performs some connected-region and short-path checks,
and shares goals. But `findPaths()` launches one async task per distinct
group, then calls `task.get()` for each. The callback matches results by
iterating `m_jobs`. Under many unrelated destinations, worker creation,
result scans or waits may be costly even though A* runs on other threads.

Instrument pending/running/completed counts, group sizes, worker concurrency,
nodes expanded, lock wait, `findPaths` wall time, and result-match operations
before changing concurrency. More threads can make throughput worse.

### UI refresh and simulation separation

`Game::loop` emits stockpile updates, creature info updates and other
signals each loop. When introducing RmlUi, maintain authoritative inventory
on the game thread and push **versioned changes** to UI models only when
data changes. Avoid GUI reads of partially updated world state. Rate-limit
purely visual redraws without delaying gameplay updates or user-visible
alerts. Measure CPU/render and UPS separately.

## First incremental implementation pass

On `feature/tick-conveyor-prototype`:
- `src/game/inventory.cpp`: in `Inventory::getClosestItems`, stop visiting
  further material-specific octrees once a positive requested count has been
  satisfied. No inventory counts/claims change; reduces repeated spatial
  searches. **Not yet legacy-game compiled or measured**.
- `src/game/game.cpp`: when debug mode is enabled, sample creature,
  gnome, job and pathfinding subsystem milliseconds once per 120 simulation
  ticks and print `KNOMIRA_PROFILE`. This is diagnostic-only. **Not yet
  legacy-game compiled or measured**.
- `src/base/tick_transport_line.h`: optional future event-scheduled
  conveyors with strict input/output tick rates, capacity and backpressure.
  Tested headlessly in `prototype/rmlui/tick_transport_line_test.cpp`;
  not attached to authoritative game inventory or research.
- Read [Mid-Game Conveyor / Tick Logistics](DESIGN_TICK_CONVEYORS.md) for
  actual integration constraints and the player unlock concept.

Do not interpret a passing Noesis-free shell test as proof that the original
simulation source compiles without Noesis. The current CI target builds the
standalone conveyor test, but not all original engine code.

## Prioritized roadmap

| Priority | Deliverable | Risk | Reason |
|---|---|---|---|
| P0 | Fully playable core, deterministic stress save, subsystem profiler | Low/medium | Can't know impact without reproducible actual gameplay |
| P1 | Instrument path requests, job-phase retries, item queries, GUI messages | Low | Find dominant real cost |
| P1 | Dirty/due job phase evaluation with bounded fallback | Medium | Directly targets repeated work shown in code |
| P1 | Change-driven RmlUi updates and workshop/stockpile signals | Low/medium | Likely avoids high-frequency UI overhead |
| P2 | Subsystem tick classes, sleeping idle workshops/creatures where safe | Medium | Defers work without changing outcomes |
| P2 | Shared positive/negative path cache for haul routes and patrols | Medium/high | Matches user requirements and Factorio precedent |
| P2 | Bounded path worker scheduling and keyed completion matching | Medium/high | Useful only if measurements show queue/lock pressure |
| P3 | Spatial index for nearby creature queries / threat candidates | Medium | Avoid map-wide scans if hot |
| P3 | Hierarchical A* / flow field prototypes, data-oriented memory overhaul | High | Only if profiling shows current algorithms cannot scale |

Do **not** add formations, leader-relative position steering or per-soldier
formation checks. Squad leader threat coordination is optional and must be
proven cheaper than the local behavior it replaces. Soldiers keep independent
self-defense and line-of-sight.

## Reproducible performance experiments

Test a baseline and each optimization independently, always from a known
save/seed. Suggested **workloads**, not reported benchmarks:

- **Small:** 20 gnomes, 30 animals, 5k items, 10 workshops; steady colony.
- **Medium:** 100 gnomes, 100 animals, 25k items, 30 workshops, several active
  stockpiles and farming areas.
- **Large:** 300 gnomes, 300 animals, 100k items, 80 workshops, many jobs and
  complex connected z-levels.
- **Path stress:** 10/50/200 patrol soldiers on shared circuits; construction
  closes doors/stairs; 1/5/20 squads; repeated hauling to same workshops.
- **Job stress:** 5k PENDING construction/crafting jobs requiring missing
  materials, then massive supply arrival and reservation releases.
- **Environment:** fast-changing water, many plants/animals, repeated terrain
  changes; at least one lengthy save/load/restart test.

Collect: simulation steps/s; per-subsystem p50/p95/p99 and worst tick ms;
AI decisions per tick; item lookup counts; pending jobs scanned/claimed/retried;
active entities ticked; paths requested/computed/cache hits/misses/failed;
A* nodes; async worker count and wait; route invalidations; allocations;
UI signals/rebuilds and render time; gnome idle time, work completed per
game-day, hauling throughput and distance walked; save/load integrity.

**Gate:** Accept an optimization only if whole-simulation throughput
improves measurably at large scale **without** unacceptable latency,
AI correctness, item reservation/ownership, deterministic save/load or
player-feedback regressions. No invented FPS/UPS savings.

## Critical design principles

1. Separate **simulation truth** from cached acceleration structures.
   Inventory must remain immediate and correct; indexes and route caches
   may be rebuilt if invalidated.
2. Reuse existing systems where possible. One pathfinding service for
   hauling, free movement, patrols and other path consumers.
3. Avoid O(number-of-jobs × number-of-items) repeated work on unchanged
   state. Make dependency changes wake blocked work directly.
4. Keep local threat response, environmental hazards and moving-creature
   safety responsive even when broad AI planning is less frequent.
5. Multithreading is not automatically a win; benchmark contention,
   synchronization, determinism and integration cost before refactoring.
6. Keep player-painted preferred routes and military patrol zones separate
   from hard impassable terrain and inventory ownership. Blockages should
   raise deduplicated actionable alerts with a fallback when possible.

**Related design docs:**
- [Preferred Routes, Workshop Logistics and Military Patrols](DESIGN_PREFERRED_ROUTES_AND_LOGISTICS.md)
- [Squad Patrol Performance](DESIGN_SQUAD_PATROL_OPTIMIZATION.md)
