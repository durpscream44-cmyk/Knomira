# Knomira multi-agent development protocol

Goal: keep multiple coding services productive **without** duplicate edits, premature "done" claims, or unsafe integration. This protocol applies to Codex, Gemini, Jules, Kilo, OpenCode and human contributors, including temporary chats.

## Source of truth (read in this order)
1. `AGENTS.md` (and component-local AGENTS.md), this protocol, then `CURRENT_CHECKPOINT.md` **on the actual target branch**. Check branch SHA and timestamp: a historical checkpoint may lag newer stacked PRs.
2. The GitHub PR list (including draft, stacked and closed/unmerged PRs), branches, recent commits and Windows Actions runs. A CI result proves only the test it ran at its exact SHA.
3. GitHub Issues for currently approved assignments; issue discussion is the durable live log. Consult related issue/PR links and overlap in changed files.
4. `CODEX_HANDOFF.md`, `CODEX_TICKETS.md` and older documents for history, never as authoritative proof of current readiness.

**Observed on 2026-11-16:** gameplay PR #21 (PLAY-01D) and stacked draft #22 (PLAY-01E) are open, unmerged; the older checkpoint on `feature/playable-db-embark-integration` still says PLAY-01D is next. Verify live GitHub again; do not start that task a second time. The RmlUi frontend is not yet proven playable.

## Assignment protocol
1. Search open and recent closed issues/PRs for the same area. Record specific references and target branch. Check for current owners and overlapping paths.
2. Only a maintainer may mark an issue `agent:approved` and `agent:ready`. An automated Jules task additionally needs `agent:provider:jules`. Agent may propose tasks but cannot approve its own suggestions.
3. Respect `Base branch:` in the issue body. If missing, outdated, or not found, **stop and report blocked**. For stacked gameplay, target the specific tested branch, not `master` by assumption.
4. Claim through the issue before editing (manual agent: comment with provider, model, time, starting SHA and planned files; change labels to `agent:running` only if authorized). One task per working tree. A pending `agent:running` task is owned; do not duplicate it.
5. Investigate the active call path and tests; make the smallest coherent change. Do not rewrite simulation or save format. Preserve the existing Noesis-free/RmlUi path and disposable save safeguards.
6. Create a dedicated branch and draft PR to the issue's base branch. Explain what was tested vs only compiled vs untested; link `Agent task: #N` (never use a close-on-merge keyword until acceptance).
7. Post a final result comment on the issue following `RESULT_TEMPLATE.md`; include exact commit SHA, CI URLs, failures and honest evidence. Move to `agent:review` (not `agent:done`), or `agent:blocked` if unfinished. Do not auto-merge or close the issue.
8. Reviewer (normally Codex/human): independently reproduce relevant tests, inspect PR base/diff, resolve overlaps, merge only when authorized; close issue and mark done only after accepted integration. Update current checkpoint in the actual gameplay branch with verified SHA and next task.
9. Before context or quota exhaustion, leave the result template even if blocked; partial work must have reproducible branch/commit, test output and exact next step. Never claim continuation without an actual scheduled runner.

## Label state
- `agent:approved`: maintainer has authorized the scope.
- `agent:ready`: safe to claim; not already running.
- `agent:running`: claimed. An automated dispatch may have an active Jules session.
- `agent:review`: code/report ready for independent review; **not** merged/done.
- `agent:blocked`: needs a decision, missing assets, authentication, CI, etc.
- `agent:done`: accepted integrated work, with evidence.
- `agent:provider:jules`: eligible for optional cloud dispatch once enabled.
- `priority:P0`, `priority:P1`, `priority:P2`: scheduling priority.

Do not mark `agent:review` as `agent:done` automatically. Treat expired or orphaned claims as blocked and investigate before retrying because network timeouts may have already created a remote coding session.

## Automation
`.github/workflows/agent-queue.yml` runs a read-only scan by default (manually and periodically). A maintainer can bootstrap labels via its manual `bootstrap` input. To enable Jules, connect the repository in Jules, set secret `JULES_API_KEY`, repository variable `JULES_SOURCE` to its **actual** `sources/...` identifier returned by List Sources, and variable `AGENT_JULES_ENABLED` to `true`. Dispatcher then reconciles existing sessions, claims **at most one** newly approved issue and submits it for an automatically created PR. All PRs still require review. See [SETUP.md](SETUP.md).

This is a provider adapter, not unlimited free model switching. Gemini/Kilo/OpenCode can use the same issue protocol manually or via a future authenticated local runner; no invisible quota bypass, no cross-agent chat replay, and no uncontrolled API spend.

## Guardrails
No direct pushes to protected development branches, no force pushes, no auto merge, no broad refactors, no edits of local real saves, no fabricated CI, no credential logging. For Jules remote Linux execution, Windows-specific Qt/Noesis/OpenAL and user-owned assets may be unavailable: report limitations honestly and rely on existing Windows Actions/desktop checks for integration.
