# Mandatory early checkpoint and cross-agent handoff

Applies to Codex and every other coding agent. No model can reliably inspect its exact hidden context limit. **Do not wait for the last message.** Use small tasks with an explicit budget and checkpoint at the first coherent milestone, then after each significant change. Prepare a handoff by approximately 50-70% of the task's planned duration.

## Before editing
- Read AGENTS.md, profiles.json, current target branch checkpoint, open/draft/closed PRs and the exact GitHub issue. Check duplicate work and stacked PR dependencies.
- Decline assignments without the needed shell, Git push, OS, Qt, licensed asset access, verified model quota or approved paths.
- One issue, one worktree, one branch. Record base SHA and intended test evidence.

## Before quota/context reset or interruption
1. **Stop expanding scope.** Commit partial changes as WIP, push branch, and confirm remote SHA.
2. Post RESULT_TEMPLATE.md in issue comments: agent/model, issue, base and head SHA, changed files, commands and results, what was *not* tested, blocker, and exact next action.
3. If push is impossible, preserve a local commit/patch and report its actual path; mark BLOCKED or PARTIAL, not PASS. Never claim GitHub contains files that were not pushed.
4. Identify work the next agent MUST NOT redo. Link any draft PR and its exact base; a new agent must resume from repo state rather than inaccessible chat history.

## Codex delegates before ending
Codex may prepare up to three bounded task packets using TASK_TEMPLATE.md with Base branch, tested SHA, open stacked PRs, Task kind, Required capabilities, Allowed paths, Risk, acceptance commands, exclusions and provider. Codex does **not** automatically approve the tasks it proposes. A maintainer applies agent:approved and agent:ready. Core engine/gameplay integration and save-format changes remain with Codex/human reviewers. No auto-merging.

## Definition of successful handoff
A reviewable committed/pushed WIP branch plus reproducible report, even when incomplete. Compilation only proves compilation; no claim of a playable game until actual runtime tests pass.
