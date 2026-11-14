# Knomira — Current Development Checkpoint

**Checkpoint CP-2026-11-14-03** · **Branch:** `feature/playable-db-embark-integration` · [Draft PR #20](https://github.com/durpscream44-cmyk/Knomira/pull/20)  
**Last CI-verified CODE SHA:** `8b6497e1244f2fc6634858462f319d9c03260ea4`  
**At that SHA:** [Noesis-free Windows #43645488725](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43645488725), [Qt-hosted #43645488702](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43645488702), [RmlUi prototype #43645488655](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43645488655): **ALL PASS**.

## Fast pickup — don't replay the whole chat

1. Read this page and `AGENTS.md`. Read [CODEX_HANDOFF.md](CODEX_HANDOFF.md) **only for detailed source history**.
2. Protect the local working tree: `git status --short --branch`; never reset/overwrite user changes.
3. **Don't recheck old PRs/CI** when the exact code SHA above is unchanged. Only refresh when a new commit/PR or failure appears.
4. Do **one bounded playable-first task**, run the relevant Windows checks, and update this short page with the new code SHA, PASS/PARTIAL/BLOCKED, and next blocker.
5. Files in the original user save directory `Documents/My Games/Ingnomia` are off-limits to tests; use the existing **marked temporary data-root safeguard**.

## Verified and working

- Qt/RmlUi **menu shell**, save-slot metadata discovery, automated input/geometry tests and packaged original game data.
- Real original `Config`, `DB::init()`, `DB::initStructs()`, `Strings::init()` and `NewGameSettings` execute in **isolated linked smoke tests**.
- Original gameplay sources including `Game` and `GameManager` **compile without Noesis/PCH**, but **do not run together as a simulation**.
- Real job caching fix from [PR #15](https://github.com/durpscream44-cmyk/Knomira/pull/15) prevents cross-job task/sprite records.
- **Newly integrated on draft PR #20:** PR #17's database startup failure detection and rollback, plus PR #18's two-material NewGameSettings fix. Both were tested in the **same CI run**: `Initialize original DB and Strings services in isolated runtime` and `Initialize real NewGameSettings and verify two-material embark roundtrip` succeeded, as did the original-source compilation and save-root checks.

## PR organization — avoid duplicate work

Canonical draft stack (none confirmed merged): **#8 → #10 → #11 → #12 → #14 → #15 → #18 → #20**; controls base #6. Each successive PR is based on the prior tested work.  
[#17](https://github.com/durpscream44-cmyk/Knomira/pull/17) is now **closed, included in #20**; [#13](https://github.com/durpscream44-cmyk/Knomira/pull/13), [#16](https://github.com/durpscream44-cmyk/Knomira/pull/16), and [#19](https://github.com/durpscream44-cmyk/Knomira/pull/19) were **closed unmerged as superseded/duplicates**. Do not resurrect any of them or re-extract their IO functions.  
[#7](https://github.com/durpscream44-cmyk/Knomira/pull/7) is optional logistics/profiling; [#9](https://github.com/durpscream44-cmyk/Knomira/pull/9) is unverified legacy mouse diagnostics. Neither is a blocker for initial game bootstrap.

## NEXT SINGLE CODING TASK: PLAY-01D

**Link and construct the original `GameManager` / `EventConnector` in a disposable, marked temp-root startup/shutdown smoke executable.** Source entry points: `src/game/gamemanager.cpp`, `src/gui/eventconnector.cpp`, aggregator constructors, `src/game/newgamesettings.cpp`, `src/base/gamestate.cpp`, `src/base/util.cpp`, and `src/main.cpp`.

- Existing `GameManager` constructor creates `EventConnector` (multiple UI aggregators), `Global::util`, `NewGameSettings` and initializes `GameState`. **Do not fake these objects** to report success. Expect unresolved linker dependencies; isolate and document the first concrete blocker.
- Audit `Global::util` / `Global::sel` ownership, QObject parents and shutdown; avoid legacy `gameThread.terminate()`.
- Require `KNOMIRA_ISOLATED_TEST_DATA_ROOT` only in smoke targets, valid `KNOMIRA_TEST_DATA_ROOT`, safe no-save startup, and clean shutdown. Keep production save path unchanged.
- PASS only after a **linked real GameManager lifecycle executes** in Windows CI and existing Config/DB/embark/UI tests still pass. A source-only compile is PARTIAL.

**Not playable yet:** no actual colony/New Game simulation in the RmlUi shell, live world renderer, map mouse selection, functioning gnome jobs, or actual colony save/reload. Those follow GameManager lifecycle.

## Short rolling log

- **CP-03 · 2026-11-14:** reconciled tested PR #17 into tested PR #18 on [PR #20](https://github.com/durpscream44-cmyk/Knomira/pull/20), verified one combined Windows CI run for rollback + embark parsing. Closed superseded #17 and duplicate #19 without merging. Next task PLAY-01D.
- Earlier history and bug investigations are retained in [CODEX_HANDOFF.md](CODEX_HANDOFF.md) and Git history, **not recopied here**.
