# Mandatory mission and continuity record

Store the active card/checkpoint in the issue or branch; chat-only models return Markdown marked CHAT ONLY for a maintainer to post.

## Mission card — fill before work

- Issue/assignment ID, owner, lease expiry UTC (or NONE):
- Exact mission and success criteria:
- Authorized actions and exact file allowlist:
- Forbidden actions/paths:
- Start branch/full SHA, target PR base:
- Provider and exact model (or UNVERIFIED); actual tools, OS, quota and limits:
- Prior checkpoint, related PRs and duplicate work checked:
- Expected acceptance commands/evidence and independent reviewer:
- Save-format impact and risk; maintainer authorization link:

A lease is an issue comment naming owner, branch, start and expiry. Before reclaiming an expired lease, check remote branches, PRs, comments and local-only recovery notes. If claims conflict, stop writes and let the maintainer choose a canonical branch. A coordinator cannot approve its own high-risk work.

## Permanent-ID mini roadmap

Use IDs such as `TASK-01`, never renumber them. Allowed statuses: `NOT STARTED`, `IN PROGRESS`, `COMPLETED — NOT VERIFIED`, `VERIFIED`, `BLOCKED`.

| ID | Output | Status | Exact evidence or blocker |
| --- | --- | --- | --- |
| TASK-01 | Inspect issue/base/overlap | NOT STARTED | — |
| TASK-02 | Scoped implementation | NOT STARTED | — |
| TASK-03 | Acceptance and handoff | NOT STARTED | — |

A passing claim needs evidence at the exact SHA. Record failed checks instead of removing steps.

## Checkpoint — publish early and repeatedly

Update after investigation, each meaningful milestone, before task transitions and before expected usage/context pressure. Never depend on the model detecting its hidden context limit.

- UTC timestamp, issue, provider/model, owner/lease:
- Starting branch/SHA; current branch; ending local SHA; confirmed remote SHA or **NOT PUSHED**:
- Roadmap IDs/statuses; verified completed work and evidence:
- Unfinished work/blockers; exact modified files/diff/PR; save impact:
- Commands, exit codes, test counts, CI URL plus tested SHA; state NOT RUN explicitly:
- Unpushed worktree/patch path and recovery owner, or NONE:
- What the next AI must NOT repeat:
- Next exact command/file/action, dependency and reviewer:
- Labels: IMPLEMENTED / TESTED / PROPOSED / BLOCKED / UNVERIFIED, as applicable.

## Resume pointer

`RESUME: issue #N | canonical branch <name> | verified SHA <full SHA or NONE> | last verified step <ID> | next <exact action> | blocker <item or NONE> | updated <UTC>`

A replacement verifies remote SHA, issue/PR state and any local recovery path, then resumes from the last verified step. Never infer pushed or merged state from a chat claim. Do not post secrets to GitHub.
