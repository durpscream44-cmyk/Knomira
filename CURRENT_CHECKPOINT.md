# Knomira — CURRENT CHECKPOINT (single fast pickup)

**Verified:** 2026-11-14. **Canonical reviewed working branch:** `feature/playable-newgame-embark-smoke` ([draft PR #18](https://github.com/durpscream44-cmyk/Knomira/pull/18)), at **pre-checkpoint** code SHA `71be050e68863a4480e6af2f54f0af0e8506dc19`.
**Status:** Original Config, DB, localization and NewGameSettings execute in isolated Windows tests; Noesis-free RmlUi menus/saves shell builds. **No linked GameManager, world, live colony, actual map rendering or gameplay save/load.**

This is the **fast source of truth for what to do next**. The longer [CODEX_HANDOFF.md](CODEX_HANDOFF.md) is a history and detailed dependency reference. After completing a ticket, update this file's date, branch/SHA, exact CI run, one remaining blocker, and next task. Do not repeatedly re-review older work unless the checked-out Git SHA or CI has changed.

## One-minute pickup (Codex and ChatGPT)

1. `git status --short --branch` first. **Preserve local uncommitted changes**. Then `git fetch origin`, inspect `origin/feature/playable-newgame-embark-smoke` and PR #18's current head. Never force checkout/reset an active worktree.
2. Read this checkpoint and root `AGENTS.md`. Consult `CODEX_HANDOFF.md` only for historical context or a named blocker; it contains older headings/branch names.
3. Inspect current CI **only for the exact current branch SHA** and any new PRs created after #18. No need to repeatedly check #6–#16 if their commits and PR statuses are unchanged.
4. Work on **one playable-first integration ticket**, run Windows CI, and write a short appended checkpoint result (PASS / PARTIAL / BLOCKED, source paths and run number).

## Confirmed recent results

| PR | Branch / relationship | Verified result | Notes |
| --- | --- | --- | --- |
| [#6](https://github.com/durpscream44-cmyk/Knomira/pull/6) | `feature/controls-menu-redesign`, draft | RmlUi menu and automated input tests | Physical mouse and controller behavior still require desktop verification |
| [#8](https://github.com/durpscream44-cmyk/Knomira/pull/8) | `feature/playable-engine-compile-gate`, draft, based on #6 | Original Game/GameManager and selected core source files compile without Noesis/PCH | **Compile only**, no live GameManager |
| [#10](https://github.com/durpscream44-cmyk/Knomira/pull/10) | `feature/playable-db-bootstrap-smoke`, draft, based on #8 | Packaged SQL, AI and JSON assets validated | In-memory DB fixture, no colony |
| [#11](https://github.com/durpscream44-cmyk/Knomira/pull/11) | `feature/playable-safe-data-root`, draft, based on #10 | Marked temporary save-root, original IO folder creation and production path regression passed | No real user saves touched |
| [#12](https://github.com/durpscream44-cmyk/Knomira/pull/12) | `feature/playable-config-startup-smoke`, draft, based on #11 | Real original `Config` constructor linked and ran in a sandbox | [CI 43580189380](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43580189380) |
| [#14](https://github.com/durpscream44-cmyk/Knomira/pull/14) | `feature/playable-db-services-after-config`, draft, based on #12 | Original `DB::init()`, `DB::initStructs()`, `Strings::init()` linked and ran | [CI 43587102028](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43587102028) |
| [#15](https://github.com/durpscream44-cmyk/Knomira/pull/15) | `feature/fix-job-cache-task-scope`, draft, based on #14 | Fixed cross-job `Jobs_Tasks` / `Jobs_SpriteID` cache mixing; validated per-job counts | [CI 43628614627](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43628614627) |
| [#18](https://github.com/durpscream44-cmyk/Knomira/pull/18) | **Current canonical** `feature/playable-newgame-embark-smoke`, draft, based on #15 | Fixed combined-item second material reload; linked real `NewGameSettings` and tested two-material embark save/reload | **Current head CI [43639509947](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43639509947) PASS** |
| [#17](https://github.com/durpscream44-cmyk/Knomira/pull/17) | `completion/play01-db-init-fail-closed`, draft, **sibling of #18** based on #15 | `DB::init()` rejects missing/malformed SQL and rolls back failed import | [Current head CI 43632843766](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43632843766) PASS; **not included in #18** |

**Do not merge:** [PR #13](https://github.com/durpscream44-cmyk/Knomira/pull/13) and [PR #16](https://github.com/durpscream44-cmyk/Knomira/pull/16) were **closed as superseded**; they contain competing/duplicate configuration extraction. The canonical dependency stack is #8 → #10 → #11 → #12 → #14 → #15 → #18, **with #17 separately based on #15**. #7 logistics/profiling and #9 old map mouse diagnostics are separate and deferred.

All of #6/#7/#8/#9/#10/#11/#12/#14/#15/#17/#18 were **open draft PRs** at checkpoint verification; none was confirmed merged. **Review/merge dependencies in order; do not assume one successful CI run proves full integration.**

## Precise next two tasks, in order

### PICKUP-01: Resolve the #17 / #18 sibling relationship without duplicate changes

Inspect `src/base/db.h`, `src/base/db.cpp`, `src/main.cpp`, `prototype/rmlui/engine_services_bootstrap_test.cpp` on #17 versus #18.
- #17 changes `DB::init()` from `void` to `bool`, adds failure rollback and a guarded call in legacy main; its existing tests cover missing/corrupt data.
- #18's `newgame_settings_bootstrap_test.cpp` currently calls `DB::init()` under the original signature. After integration, **check the return** instead of silently proceeding.
- **Verified compare vs common #15 base:** #17 changes only `src/base/db.cpp`, `src/base/db.h`, `src/main.cpp`, `prototype/rmlui/engine_services_bootstrap_test.cpp`, and its historical `CODEX_HANDOFF.md`. #18's independent gameplay source change is `src/game/newgamesettings.cpp` plus new `io_json_writer.cpp` and `newgame_settings_bootstrap_test.cpp`. This minimizes source-level overlap, but `DB::init()` return handling must be updated in the new-game test and any other callers before CI. Keep the canonical checkpoint/handoff instead of copying the historical handoff diff.
- Do not simply declare #17 covered: check both the missing-SQL/rollback assertions and the embark two-material assertions run against one new integrated SHA.
- Choose a carefully reviewed integration branch/rebase or a small conflict-resolved cherry-pick. Preserve both sets of tests; do not merge PR #13/#16 or overwrite branch #18 blindly.
- Acceptance: one CI run exercises **both** original-game DB failure/rollback and original NewGameSettings two-material roundtrip on a single commit. Never touch personal saves. Record actual commit and CI run here.

### PICKUP-02: PLAY-01C — Real GameManager and EventConnector startup/shutdown

Inspect `src/game/gamemanager.cpp`, `src/gui/eventconnector.cpp`, `src/game/gamestate.cpp`, `src/base/util.cpp`, `src/main.cpp` and `prototype/rmlui/CMakeLists.txt`.
- Original `GameManager::GameManager` constructs a real `EventConnector(this)`, `Global::util = new Util(nullptr)`, `Global::newGameSettings = new NewGameSettings(this)`, then `GameState::init()`.
- Original `EventConnector` constructor constructs **15 GUI aggregators**, so a complete link may require other services; isolate legitimate pieces rather than stubbing them or declaring success from compile-only objects.
- Audit `Global::util` / `Global::sel` replacement and Qt QObject ownership, and replace the old `gameThread.terminate()` shutdown approach with cooperative quit/wait in the new shell.
- Run with `KNOMIRA_ISOLATED_TEST_DATA_ROOT` **compiled only into test targets**, explicit `KNOMIRA_TEST_DATA_ROOT` pointing to a marked existing `QTemporaryDir`; validation must fail closed. Do not run the original constructor against Documents/My Games/Ingnomia.
- Acceptance: a real, linked startup/shutdown path executes with no save-folder writes outside the temporary root and existing shell/DB/embark tests stay green. Mark PARTIAL if only object compilation succeeds.

## Definition of first playable

**Create a real world → render original terrain/sprites → move camera and click/select a tile → issue a real dig/stockpile/workshop order → gnome fulfills the job → save → exit → reload and continue.**

Not yet present: linked GameManager in RmlUi, world renderer, world generation via New Game, live mouse selection or a verified playable save cycle. The separate [PR #9](https://github.com/durpscream44-cmyk/Knomira/pull/9) map-click fix still needs real desktop reproduction. Do not mislabel shell menu CI as gameplay.

**Parked:** physical conveyors, instant inlet/outlet logistics (preferred future design in PR #7), patrol optimizations, new races/sprites, extensive modding/relics/Ages, multiplayer. Do not implement until a playable baseline and actual profiling.

## Last checkpoint action

- **2026-11-14:** audited PRs #6–#18 and exact latest heads. Found additional proven #12/#14/#15/#17/#18 work and closed duplicate #13/#16. Created this fast checkpoint so future sessions consult the most recent status rather than replaying this history.
- **Next owner:** Codex or this chat, starting with PICKUP-01. Update this section after the next bounded task.
