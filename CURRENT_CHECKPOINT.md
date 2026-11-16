# Knomira — Current Development Checkpoint

**Checkpoint CP-2026-11-15-05** · **Branch:** `completion/play01e-colony-roundtrip` · [Draft PR #22](https://github.com/durpscream44-cmyk/Knomira/pull/22), based on [#21](https://github.com/durpscream44-cmyk/Knomira/pull/21)

**Last CI-verified CODE SHA:** `f982056470e3fbf5d046d2582f278b6c4d4856e7` · **Local runtime PASS; all three Windows workflows PASS.**

**At that SHA:** [Noesis-free Windows #43768110225](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43768110225), [Qt-hosted #43768110220](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43768110220), [RmlUi prototype #43768110231](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43768110231). CI executes PLAY-01D and verifies PLAY-01E's missing-asset skip; the full colony runtime pass uses local authorized assets.

**Prior CI-verified lifecycle SHA:** `8d5f67bec87ee2e1863b10cb06485a0f66eae10f`; [Noesis-free Windows](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43766361570), [Qt-hosted](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43766361653), [RmlUi prototype](https://github.com/durpscream44-cmyk/Knomira/actions/runs/43766361619): **ALL PASS**.

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
- **PLAY-01E local runtime PASS:** actual New Game through EventConnector, 532,480 tiles and one settler; real timer/heartbeat, pause and fast speed; pending tree order saved/reloaded; settler completes restored order and produces wood. Squad identity/order, population/inventory/clock and original-slot SHA256s survive reload; second slot preserves the first. All **11 CTest tests passed locally**, including staged authorized audio resource cleanup. Missing assets explicitly skip the colony test (77); CI cannot claim its runtime coverage.
- Fixed the reproduced New Game assertion in `MilitaryManager`: squad entity IDs were used as QList insertion indexes; append preserves creation and serialized order. The existing dirty checkout already had this narrow fix; it was ported without overwriting that work. No save schema or production save-path changes.
- Real job caching fix from [PR #15](https://github.com/durpscream44-cmyk/Knomira/pull/15) prevents cross-job task/sprite records.
- **Newly integrated on draft PR #20:** PR #17's database startup failure detection and rollback, plus PR #18's two-material NewGameSettings fix. Both were tested in the **same CI run**: `Initialize original DB and Strings services in isolated runtime` and `Initialize real NewGameSettings and verify two-material embark roundtrip` succeeded, as did the original-source compilation and save-root checks.

## PR organization — avoid duplicate work

Canonical draft stack (none confirmed merged): **#8 → #10 → #11 → #12 → #14 → #15 → #18 → #20 → #21 → #22**; controls base #6. Each successive PR is based on the prior tested work.

[#17](https://github.com/durpscream44-cmyk/Knomira/pull/17) is now **closed, included in #20**; [#13](https://github.com/durpscream44-cmyk/Knomira/pull/13), [#16](https://github.com/durpscream44-cmyk/Knomira/pull/16), and [#19](https://github.com/durpscream44-cmyk/Knomira/pull/19) were **closed unmerged as superseded/duplicates**. Do not resurrect any of them or re-extract their IO functions.  
[#7](https://github.com/durpscream44-cmyk/Knomira/pull/7) is optional logistics/profiling; [#9](https://github.com/durpscream44-cmyk/Knomira/pull/9) is unverified legacy mouse diagnostics. Neither is a blocker for initial game bootstrap.

## NEXT SINGLE CODING TASK: PLAY-02A

**Connect the RmlUi shell to the verified session and original world renderer.** `prototype/rmlui/qt_host.cpp` still reports "world generation will be wired after simulation bootstrap" for New Game. `MainWindowRenderer`'s header is Noesis-free, but its implementation still includes/owns a legacy `MainWindow*` and calls that host's context/redraw methods. The preserved native viewer has useful host-decoupling work; inspect and selectively reuse it, never merge the dirty checkout wholesale.

- Keep production and test engine objects separate: `knomira_lifecycle_engine` has a mandatory test-only temp-root definition and must not be shipped as the production simulation library.
- Preserve the original renderer/selection and simulation. Align Qt/RmlUi GL context, GLAD loader and the world's OpenGL 4.3 compute-shader requirements; wire heartbeat acknowledgments and lifecycle signals.
- Test New Game → rendered world → pause/resume → save/load → orderly exit using marked isolated roots and authorized assets. Verify actual desktop behavior before claiming UI playability. Keep legacy production saves untouched.

**Not playable yet in RmlUi:** colony simulation, one job type and save/reload now execute in the single-Qt-thread regression, but the menu, live map and gameplay input are not connected. Active simulation/render-thread integration, malformed-save recovery, broader jobs, fresh-session resets after loaded games and multi-season stability remain unverified on this branch.

## Local preservation and known limits

- Development worktree: `C:/Users/durps/Knomira/work/play01d-real-lifecycle`, created from fetched integration head `b5d6348`. Original checkout remains on `feature/controls-menu-redesign` with all 53 pre-existing modified/untracked status entries preserved. Do not reset or merge over that work. Its `work/` entry now also contains this worktree.
- Test IO uses only marked QTemporaryDir roots; production data path/save format unchanged. `BUILDING.md` contains build/runtime commands. No original player-save directory was opened.
- Fresh temp roots log the existing optional `userpresets.json` warning. Existing compiler warnings include missing returns in `Util::borderPos` and `StockpileManager::getJob`; not fixed by this ticket.
- The settings/C RNG seeds are fixed, but original subsystems reseed from the clock; assert within-run persistence invariants, not identical cross-run population counts. Missing optional `military.json` also logs on a fresh colony. Full changed-file list, commands and boundaries: [validation report](docs/PLAYABLE_LIFECYCLE_VALIDATION.md).
- Legacy Noesis `src/main.cpp` still uses its historical thread termination path; this ticket verifies cooperative shutdown in the Noesis-free executable only. No legacy/Release/long-soak claim.

## Short rolling log

- **CP-05 · 2026-11-15:** PLAY-01E on separate branch/PR #22, prerequisite #21. Reproduced/fixed squad-index startup crash; 11 local tests pass, including real restored-job completion and save preservation. All three Windows workflows pass at `f982056`; colony asset/runtime boundary recorded above. Next PLAY-02A.
- **CP-04 · 2026-11-15:** PLAY-01D implemented on isolated branch, reproduced/fixed real audio teardown failures; all three Windows workflows and 10 local tests passed at `8d5f67b`. Next PLAY-01E, then RmlUi session/world integration.
- **CP-03 · 2026-11-14:** reconciled tested PR #17 into tested PR #18 on [PR #20](https://github.com/durpscream44-cmyk/Knomira/pull/20), verified one combined Windows CI run for rollback + embark parsing. Closed superseded #17 and duplicate #19 without merging. Next task PLAY-01D.
- Earlier history and bug investigations are retained in [CODEX_HANDOFF.md](CODEX_HANDOFF.md) and Git history, **not recopied here**.
