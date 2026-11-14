# Knomira RmlUi — local Codex / Computer Use instructions

These instructions apply only to the RmlUi prototype, independent game shell,
and related Windows desktop tests. Keep the root AGENTS.md rules for protecting
saves, credentials and unrelated engine code. The user has explicitly
authorized the ongoing RmlUi replacement experiment in this directory.

## Current playable-first handoff

For the next engine integration milestone, read the root
[CODEX_HANDOFF.md](../../CODEX_HANDOFF.md) on
`feature/playable-engine-compile-gate` and use that branch for PLAY-01.
The original controls test instructions below still apply when investigating
that shell and its separate draft PR #6. Before switching branches, inspect
`git status` and protect local changes. Don't treat the current shell as
having a playable map.

## Local machine and tools

- Project checkout: `C:\Users\durps\Knomira`.
- Working branch: `feature/controls-menu-redesign` (check `git status` and
  do not discard local user changes).
- The tested Noesis-free Qt/RmlUi shell executable is expected at
  `build\rmlui-prototype\desktop-test-controls-v2\knomira_game_shell.exe`.
- Windows PowerShell 5.1 and GitHub CLI `gh` are installed on the user's PC.
- The user wants the *local desktop agent* to use terminal/PowerShell and,
  with permission, Computer Use to test native Qt windows itself.
- Run `prototype\rmlui\Test-GameShell.ps1` to check the current shell and
  launch it without changing its installation. Use `-Install` only when the
  user requests installing/updating from the latest CI build.
- The optional local `-Screenshot` argument saves a screenshot of the game's
  window to the ignored build folder. Inspect the image rather than relying
  exclusively on compilation success.

## Reproduce the current bug first

Issue: in Settings > Controls, a **mouse left click** on a visible primary
or alternate binding, or on **Change**, sometimes scrolls the list instead
of opening the capture dialog. Controller selection has worked. Prior fixes
covered row layout and early hit testing, but desktop interaction must be
observed to prove the bug is fixed.

1. Start the Noesis-free shell and inspect the actual window visually.
2. Open Settings > Controls > Menu > Keyboard using the mouse.
3. Click the **displayed assignment text** and then the **Change** button.
   The capture dialog must open on the first click without scrolling.
4. Cancel capture, use a second row, and repeat with the World and Panels
   categories, primary and alternate fields, and both device tabs.
5. Remap a nonessential menu key to F3; verify the table updates immediately,
   the new action works, and the JSON profile persists after relaunch.
6. Check keyboard navigation, focus scrolling, Escape/Tab recovery,
   Exit confirmation, and regression tests. For Xbox, inspect existing
   code/tests; physical gamepad UI verification requires a connected pad.
7. Record reproducible steps, screenshots and precise failure outputs.
   Fix the smallest cause, test again, and report PASS/PARTIAL/BLOCKED
   separately for compilation, input behavior and actual visual tests.

## Safety and workflow

- Do not remove or rewrite `Documents\My Games\Ingnomia\save`.
- Never use elevated PowerShell for ordinary testing. Request approval for
  installing software, granting UI access, changing protected settings,
  modifying save data, deleting files, or using network-wide commands.
- Keep screenshots/logs inside `build\rmlui-prototype\desktop-tests` and
  avoid capturing other apps, private data or unrelated desktop windows.
- Do not disable Windows security, start persistent remote-control servers,
  expose ports, create a self-hosted runner, or store credentials.
- Stop the tested process when the user requests it; don't leave loops or
  monitoring tasks running after the task ends.
- Do not claim visual mouse success from a headless geometry test alone.
- The new RmlUi shell is not yet connected to GameManager. Menu/input tests
  are not a substitute for actual colony simulation tests.
