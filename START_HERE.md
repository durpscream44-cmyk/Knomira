# Knomira — Start Here (Universal AI Check-in)

**One-command check-in:** "Check in for Knomira."

Official repository: https://github.com/durpscream44-cmyk/Knomira  
Project mission: make the Ingnomia-derived single-player colony game **playable, functionally complete, stable, and performant** before expanding scope. Preserve save integrity and working gameplay.

This is the entry point for ordinary ChatGPT/Copilot/Grok/Gemini chats, coding CLIs, Codex, reviewers, and planning coordinators. A check-in is **not** permission to run an unattended agent, bypass review, spend money, change security settings, or merge code.

## Fast route (read in this order)

1. Read `AGENTS.md` on the **actual target branch**; its scope and safety rules are binding.
2. Read [CHECKIN_PROTOCOL.md](docs/agent-coordination/CHECKIN_PROTOCOL.md) for task selection, role boundaries, mission/checklist, checkpointing, and sign-off.
3. Check **live** GitHub issues, open/draft/closed PRs, branch heads, recent issue comments, and relevant CI results. Old handoffs are clues, not current truth.
4. Read only the **task-specific** issue, reference files, and checkpoint needed for the selected job. Do not dump the whole repo into context.
5. Declare a 1–3 sentence mission, what is allowed/forbidden, starting branch/SHA, and a miniature numbered roadmap; then **perform the authorized work**.
6. Update the checklist with evidence after every meaningful step. Before ending, produce the [checkpoint](docs/agent-coordination/CHECKPOINT_TEMPLATE.md) with the **NEXT EXACT ACTION**.

Use [CHAT_TASK_PACKET_TEMPLATE.md](docs/agent-coordination/CHAT_TASK_PACKET_TEMPLATE.md) when an AI cannot inspect the repository itself. Do not invent file contents or commits if access fails.

## Understand what "check in" means

| User says | Required action |
| --- | --- |
| **Check in for Knomira** / **Find work on Knomira** | Verify live state and own capabilities; select a suitable already-authorized low-risk task, or carry out useful read-only investigation and produce a proposed task packet. Start allowed work without lengthy re-explanation. |
| **Continue my Knomira work** | Identify the assigned issue and latest verified checkpoint; compare to remote state, then resume from the first unfinished step. |
| **Check in as coordinator** | Examine dependencies, overlap, blockers and quotas; prepare bounded assignments and handoffs. Do not approve your own changes. |
| **Check in as reviewer** | Examine review-ready PR diffs and evidence, report PASS/CHANGES NEEDED/BLOCKED; no implied permission to merge. |
| **Knomira status** | Summarize verified vs proposed vs untested work, branch/PR state, blockers and next action. |

If the user asks you to check in, **do not stop at a plan** if a permitted, useful task can be completed in the current environment.

## Hard gate: assignment authority

**Read-only investigation and a proposed patch/task packet are always the default.** To edit repository files, the assigned issue must authorize the service/role, scope, base branch, writable paths, and acceptance evidence; use an isolated branch and PR. Never equate an open GitHub issue, an available label, or writable Git credentials with authorization to execute it.

A manually launched task is not automatically available to a random AI. Specifically, [#28](https://github.com/durpscream44-cmyk/Knomira/issues/28) and [#29](https://github.com/durpscream44-cmyk/Knomira/issues/29) were designated **manual Kilo-only launch and Codex review** as of 2026-11-20. [#25](https://github.com/durpscream44-cmyk/Knomira/issues/25) is a high-risk coordinator security gate; [#26](https://github.com/durpscream44-cmyk/Knomira/issues/26) depends on it; [#27](https://github.com/durpscream44-cmyk/Knomira/issues/27) is Codex/supervised engine audit. Re-check their current contracts; **never dispatch them just because they appear in a queue**.

If no eligible execution task exists, choose a **read-only** valuable contribution: investigate an unclaimed bug hypothesis, review a patch, prepare tests as proposed code, identify a dependency, draft an issue/task packet, or update a copyable handoff. Make clear what has NOT been applied or tested.

## Historical versus current branches

- The existing [multi-agent coordinator draft PR #24](https://github.com/durpscream44-cmyk/Knomira/pull/24) is on `feature/multi-agent-github-coordinator` and was **not merged** when this entry point was written. Its `docs/agent-coordination/README.md`, `TASK_TEMPLATE.md`, `RESULT_TEMPLATE.md`, and `CODEX_DELEGATION.md` are important **draft references**, not deployed execution authorization.
- The playable/RmlUi work includes stacked draft PRs [#21](https://github.com/durpscream44-cmyk/Knomira/pull/21) and [#22](https://github.com/durpscream44-cmyk/Knomira/pull/22). Check actual bases/heads before working; do not assume code from either is on `master`.
- `CODEX_TICKETS.md` and `CODEX_MASTER_INSTRUCTIONS.md` provide historical context. The old "always start T01" instruction has been **retired** as the default check-in policy. Verify current issues before assigning anything.

## Integration guard: overlapping onboarding drafts

- [PR #31](https://github.com/durpscream44-cmyk/Knomira/pull/31) is the candidate **canonical one-phrase check-in entry point** in this file. [PR #30](https://github.com/durpscream44-cmyk/Knomira/pull/30), stacked on draft [PR #24](https://github.com/durpscream44-cmyk/Knomira/pull/24), independently edits `START_HERE.md`. Reviewers must reconcile both versions into one canonical entry point; neither should silently overwrite the other.
- Preserve PR #30's fail-closed `scripts/local_agent_runner.py` `--execute` refusal and `scripts/test_runner_execution_gate.py` regression when integrating coordinator code. Do not expose the unreviewed runner from PR #24. Independent security review [issue #25](https://github.com/durpscream44-cmyk/Knomira/issues/25) and explicit owner approval are prerequisites to enabling execution.
- This onboarding is on a **draft branch**, not yet deployed to `master`. Until merged, test using the explicit branch URL; the check-in phrase alone is not proof that the default branch includes this protocol.
- If all open issues are restricted, blocked, or not authorized for this provider, an ordinary check-in must complete useful **read-only** work and record a durable checkpoint. It does not grant ChatGPT a coding assignment.

## Efficiency and safety policy

- Prioritize **regular ChatGPT and free/freemium agents** for planning, documentation, static analysis, deterministic tests, and bounded implementation. Default **$0 extra spending** unless owner approves otherwise.
- Reserve premium Codex/high-capability engineering for serious integration, architecture, security, concurrency, persistence, and defects that free models cannot reliably resolve.
- The cheapest response is not automatically the cheapest **accepted change**. Track rework and review cost.
- Protect local saves, secrets, license files, third-party assets, repository branches, and other agents' work. A fork/worktree is **not** an OS security sandbox.
- No automation, no unrestricted command permissions, no self-approval, no auto-merge, no invented CI/test success.

**Required final output:** mission; provider/model (or unknown); issue/base SHA; checklist with VERIFIED versus only COMPLETED; changed/proposed files; actual tests and NOT RUN; exact remote commit or NOT PUSHED; blockers; and **NEXT EXACT ACTION**. A link to a chat without a durable checkpoint is not an adequate handoff.

## Legacy Codex onboarding

Earlier versions of this file instructed Codex to run `T01` from `CODEX_TICKETS.md`. That was a historical starting workflow, not a standing assignment. Use it only when explicitly requested and after confirming the current repository state.
