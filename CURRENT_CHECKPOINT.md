# Knomira — Current Development Checkpoint

**Checkpoint CP-2026-11-15-04** · **Branch:** `completion/play01d-real-lifecycle` · [Draft PR #21](https://github.com/durpscream44-cmyk/Knomira/pull/21)  
**Last CI-verified CODE SHA:** `8d5f67bec87ee2e1863b10cb06485a0f66eae10f`  
**At that SHA:** [Noesis-free Windows #43766361570](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43766361570), [Qt-hosted #43766361653](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43766361653), [RmlUi prototype #43766361619](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43766361619): **ALL PASS**.

## Fast pickup — don't replay the whole chat

1. Read this page and `AGENTS.md`. Read [CODEX_HANDOFF.md](CODEX_HANDOFF.md) **only for detailed source history**.
2. Protect the local working tree: `git status --short --branch`; never reset/overwrite user changes.
3. **Don't recheck old PRs/CI** when the exact code SHA above is unchanged. Only refresh when a new commit/PR or failure appears.
4. Do **one bounded playable-first task**, run the relevant Windows checks, and update this short page with the new code SHA, PASS/PARTIAL/BLOCKED, and next blocker.
5. Files in the original user save directory `Documents/My Games/Ingnomia` are off-limits to tests; use the existing **marked temporary data-root safeguard**.

## Verified and working

- Qt/RmlUi **menu shell**, save-slot metadata discovery, automated input/geometry tests and packaged original game data.
- Real original `Config`, `DB::init()`, `DB::initStructs()`, `Strings::init()` and `NewGameSettings` execute in **isolated linked smoke tests**.
- Original gameplay sources including `Game` and `GameManager` compile without Noesis/PCH in the existing selected-source gate.
- **PLAY-01D PASS:** a dedicated test library links the original simulation, GameManager, EventConnector and all 15 real aggregators. Three startup/queued-command/shutdown cycles execute on Windows, verify QObject parent/thread affinity, clean global pointers and no colony saves. Cooperative `deleteLater` / `quit` / `wait`; no thread termination. The full link target uses a Qt-only PCH, not the legacy Noesis PCH.
- Fixed confirmed lifecycle defects: explicit Util ownership, stale globals, missing AggregatorSettings include, OpenAL object destruction without the active context, and querying a closed OpenAL device.
- Local MSVC 19.44 / Qt 6.12 / CMake 3.30 Debug builds passed for shell, lifecycle, original source/header gates, Qt host/adapter and RmlUi prototype. All **10 CTest regressions passed**, including Config, DB rollback/job cache, two-material embark, save roots/catalog, input and layout. CI uses Qt 6.8.3.
- Real job caching fix from [PR #15](https://github.com/durpscream44-cmyk/Knomira/pull/15) prevents cross-job task/sprite records.
- **Newly integrated on draft PR #20:** PR #17's database startup failure detection and rollback, plus PR #18's two-material NewGameSettings fix. Both were tested in the **same CI run**: `Initialize original DB and Strings services in isolated runtime` and `Initialize real NewGameSettings and verify two-material embark roundtrip` succeeded, as did the original-source compilation and save-root checks.

## PR organization — avoid duplicate work

Canonical draft stack (none confirmed merged): **#8 → #10 → #11 → #12 → #14 → #15 → #18 → #20 → #21**; controls base #6. Each successive PR is based on the prior tested work.  
[#17](https://github.com/durpscream44-cmyk/Knomira/pull/17) is now **closed, included in #20**; [#13](https://github.com/durpscream44-cmyk/Knomira/pull/13), [#16](https://github.com/durpscream44-cmyk/Knomira/pull/16), and [#19](https://github.com/durpscream44-cmyk/Knomira/pull/19) were **closed unmerged as superseded/duplicates**. Do not resurrect any of them or re-extract their IO functions.  
[#7](https://github.com/durpscream44-cmyk/Knomira/pull/7) is optional logistics/profiling; [#9](https://github.com/durpscream44-cmyk/Knomira/pull/9) is unverified legacy mouse diagnostics. Neither is a blocker for initial game bootstrap.

## NEXT SINGLE CODING TASK: PLAY-01E

**Exercise a real colony through EventConnector: New Game, timer/heartbeat, pause, pending order, save, load and resume.** Reuse `knomira_lifecycle_engine` with its marked temporary-root guard. Authorized tilesheets are available locally in the existing native viewer build; they must remain external to Git and CI artifacts. Explicitly distinguish asset-free CI from actual colony runtime validation.

- Use fixed world/settings seeds and assert within-run state invariants, not invented simulation replacements.
- Verify loaded state and pending jobs, previous slot hashes, original game/selection deletion and clean manager shutdown. Never load player saves.
- `GameManager::postCreationInit`, `MilitaryManager::init`, `Game::start/loop`, original IO and SpriteFactory are the next active entry points.

**Not playable yet:** the RmlUi shell is not wired to colony creation or world rendering. Lifecycle PASS does not prove world generation, jobs or colony save/reload.

## Local preservation and known limits

- Development worktree: `C:/Users/durps/Knomira/work/play01d-real-lifecycle`, created from fetched integration head `b5d6348`. Original checkout remains on `feature/controls-menu-redesign` with all 53 pre-existing modified/untracked status entries preserved. Do not reset or merge over that work. Its `work/` entry now also contains this worktree.
- Test IO uses only marked QTemporaryDir roots; production data path/save format unchanged. `BUILDING.md` contains build/runtime commands. No original player-save directory was opened.
- Fresh temp roots log the existing optional `userpresets.json` warning. Existing compiler warnings include missing returns in `Util::borderPos` and `StockpileManager::getJob`; not fixed by this ticket.
- Legacy Noesis `src/main.cpp` still uses its historical thread termination path; this ticket verifies cooperative shutdown in the Noesis-free executable only. No legacy/Release/long-soak claim.

## Short rolling log

- **CP-04 · 2026-11-15:** PLAY-01D implemented on isolated branch, reproduced/fixed real audio teardown failures; all three Windows workflows and 10 local tests passed at `8d5f67b`. Next PLAY-01E, then RmlUi session/world integration.
- **CP-03 · 2026-11-14:** reconciled tested PR #17 into tested PR #18 on [PR #20](https://github.com/durpscream44-cmyk/Knomira/pull/20), verified one combined Windows CI run for rollback + embark parsing. Closed superseded #17 and duplicate #19 without merging. Next task PLAY-01D.
- Earlier history and bug investigations are retained in [CODEX_HANDOFF.md](CODEX_HANDOFF.md) and Git history, **not recopied here**.
