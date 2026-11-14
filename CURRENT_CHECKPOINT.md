# Knomira — CURRENT DEVELOPMENT CHECKPOINT

**Checkpoint:** CP-2026-11-14-01 · **Last checked:** 2026-11-14  
**Verified base branch:** `feature/playable-db-services-after-config` (draft [PR #14](https://github.com/durpscream44-cmyk/Knomira/pull/14))  
**Base commit checked:** `ec33accfae2e089d1da535e52761e92801ed0a10`  
**CI at that SHA:** [Noesis-free Windows #43587626704](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43587626704), [Qt-hosted #43587626702](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43587626702), [RmlUi prototype #43587626706](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43587626706) — all **success**.

## Read this first, not the whole conversation

This page is the **short, current pickup point**. The much longer historical [CODEX_HANDOFF.md](CODEX_HANDOFF.md) contains source-level detail, tests, pitfalls and previous tickets. When work continues:

1. Read this page and `AGENTS.md`, inspect `git status` for user modifications.
2. Query GitHub PR heads / CI **only if the branch SHA has changed**, the results are missing/failed, or a fresh test is actually required.
3. Implement **one bounded playable-first task**, verify its exact commit, then replace this page's current status and add one short entry to the log below.
4. Avoid repeatedly reciting all earlier PRs or generating another full history.

## What is genuinely working

- New Qt/RmlUi **menu shell** and source-level UI/input/save-discovery test targets build; there is **no rendered or playable map inside this shell**.
- Real original `Config` initialization in an isolated temporary directory, with original JSON/default settings.
- Real `DB::init()`, `DB::initStructs()` and `Strings::init()` **execute in a linked smoke executable** using the original game data, with cached jobs/workshops and translation checked.
- Windows CI compiles selected real original simulation sources without Noesis/PCH, including Game and GameManager, with Qt AUTOMOC. Compilation is **not** a link or engine simulation test.
- Test save-root override is fail-closed; normal game save path remains unchanged. All runtime tests use disposable marked temp directories.

## Still blocked / unverified

- Full `GameManager` construction and orderly destruction, real game thread and world generation.
- Live world renderer, camera/world mouse clicks, designations, gnome jobs and actual New Game / Save / Load through RmlUi.
- Physical mouse rebinding / legacy map-click regression on a real desktop.
- FPS/UPS gains from optional inventory or logistics experiments.

## Branch/PR integration map

- [#6](https://github.com/durpscream44-cmyk/Knomira/pull/6): controls redesign — draft.
- [#8](https://github.com/durpscream44-cmyk/Knomira/pull/8): compile gate — draft.
- [#10](https://github.com/durpscream44-cmyk/Knomira/pull/10): game-data bootstrap — draft, based on #8.
- [#11](https://github.com/durpscream44-cmyk/Knomira/pull/11): isolated save-root protection — draft, based on #10.
- [#12](https://github.com/durpscream44-cmyk/Knomira/pull/12): **canonical Config constructor startup** — draft, based on #11.
- [#14](https://github.com/durpscream44-cmyk/Knomira/pull/14): **canonical DB/Strings startup** — draft, based on #12. **Use this head for new work.**
- [#13](https://github.com/durpscream44-cmyk/Knomira/pull/13) was closed as **superseded** by #14. **Do not merge or resurrect it**: its overlapping Config/IO extraction can duplicate definitions.
- [#7](https://github.com/durpscream44-cmyk/Knomira/pull/7) (experimental conveyors/performance) and [#9](https://github.com/durpscream44-cmyk/Knomira/pull/9) (mouse diagnostics) remain separate side branches; don't integrate ahead of playability.

## Next safe coding task

**PLAY-01C — isolate and run original NewGameSettings startup in the verified Config + DB + Strings service test**, without generating or saving a world. Original `NewGameSettings` constructor loads `newgame.json` and presets, queries game data/translated species names, and randomizes seed/kingdom name. If its linking dependencies are too broad, document the first actual blockers. Test `worldSize`, `zLevels`, starting gnome/item lists and positive/valid seed; verify no saves and all settings writes stay in the isolated temp directory. Then proceed to real GameManager/EventConnector construction.

Alternate larger next step: directly link `GameManager` / `EventConnector` / aggregators with real service objects and test construction and teardown; this is likely a much larger linker dependency graph. Do not fake managers for a green build.

**Do not** begin logistics, relics, broad mod API, pathfinding rewrites or new UI polish while the game world cannot run.

## Short checkpoint log

- **CP-2026-11-14-01:** Reconciled PRs 6–14, verified PR #14 head and three green Windows workflows. Noted real NewGameSettings constructor as a bounded next initialization step. No simulation or world renderer linked.
