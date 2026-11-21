# Universal Agent Check-in Protocol (Knomira)

**Status:** documentation-only protocol; not a runner, permission grant, issue dispatcher, or claim that an AI is working asynchronously.  
**Applies to:** ChatGPT and other chat-only AIs, free/freemium/local coding agents, coordinators, reviewers, and Codex-level integration agents.  
**Entry point:** [START_HERE.md](../../START_HERE.md); always obey [AGENTS.md](../../AGENTS.md) first.

## Mission and operating priorities

Make Knomira playable, stable and functionally complete before major scope expansion. Preserve integrity and isolate changes. Prefer regular ChatGPT/free agents for bounded work; reserve premium agents for difficult integrations, security and critical review. Optimize for **verified accepted progress per dollar and per token**, not volume of generated text.

## 60-second check-in: minimal context, maximum action

1. **Identify environment:** provider/service; exact model or UNKNOWN; source access; filesystem/terminal/test ability; GitHub read/write; known quota. Do not pretend tools exist.
2. **Verify live state:** default branch and current SHA, relevant PR/issue statuses, related owners/comments and dependent stacked PRs. The newest chat is not authoritative.
3. **Select role:** free chat research/review; junior implementer; planner; reviewer; integrator/security. Roles do not grant repository permissions.
4. **Select safe work:** resume assigned unfinished work first; otherwise inspect eligible approved unclaimed work. If no eligible task, start a *read-only* code investigation, focused review, test-design, or task proposal without editing the repository.
5. **Preflight before edits:** read exact task contract; confirm base SHA, permitted paths, test commands, no overlap, required environment. Ask only for a genuinely missing blocker.
6. **Mission declaration + numbered mini-roadmap:** keep it concise. Execute the first permitted step rather than ending at general advice.

**No self-assignment by inference.** Task metadata is not dispatch authorization. A claim must follow the issue's explicit manual/agent procedure; for any task marked manual-only, the owner must launch that exact provider explicitly. No unapproved runner, GitHub Action trigger, or label modification.

## Step state vocabulary

- `[ ] NOT STARTED`: no work or evidence.
- `[~] IN PROGRESS`: currently investigating or implementing.
- `[x] COMPLETED / NOT VERIFIED`: draft, patch, or artifact exists but not independently validated.
- `[V] VERIFIED`: the exact acceptance check actually passed on the recorded artifact/SHA.
- `[!] BLOCKED`: required tools, permission, dependency, or test unavailable.

Use permanent step IDs (S01, S02, ...). After each step, record **evidence reference** (commit/PR, file path, issue comment, command output, or pasteable code). Never infer VERIFIED from an AI self-report.

## Mission card (produce at assignment start)

- Mission (one sentence):
- Role; provider/model; identity VERIFIED / SELF-REPORTED / UNKNOWN:
- Source issue, target base branch, exact starting SHA, related PRs:
- May touch / must not touch:
- Success criteria and exact verification:
- Expected files and risk:
- Roadmap: S01 ..., S02 ..., S03 ...
- NEXT EXACT ACTION:

The mini-roadmap should normally be 3–7 small steps. Planning overhead must not dominate the task. Do not restate the entire mission every reply; briefly check alignment at step transitions.

## Mandatory checkpoint rules

Checkpoint after investigation, after meaningful implementation, before changing task, at a blocker, and before known session/quota expiration. No model can reliably know its hidden context limit, so **checkpoint early**.

A checkpoint must contain:
- issue/task, provider/model, branch/base/head SHAs (or NOT PUSHED), timestamp;
- concise mission and scope;
- per-step statuses + evidence; changed files;
- tests executed with results vs NOT RUN and remaining risks;
- what the next agent **MUST NOT REPEAT**;
- **NEXT EXACT ACTION** and dependencies.

If permitted, publish checkpoint to the assigned GitHub issue/PR or branch. Otherwise produce an entire copyable packet using [CHECKPOINT_TEMPLATE.md](CHECKPOINT_TEMPLATE.md). Never claim remote persistence from a local or in-chat draft.

## Role-specific work and escalation

| Role | Allowed by default | Additional authorization needed |
| --- | --- | --- |
| Chat-only AI | Read accessible code, investigate, draft patch/test and review report | GitHub edits, running tools, claiming restricted issues |
| Junior coding agent | Read public repo and propose bounded tasks | Any file mutation must satisfy exact authorized issue and isolated branch/worktree; manual-only tickets require manual launch |
| Planning coordinator | Review state, build task packets, identify dependencies, route by capability/cost | Approval labels, permissions, merges, modifying owner priorities |
| Independent reviewer | Review diff, identify required tests, mark confidence | Approval/merge of security-critical work; cannot approve own high-risk change |
| Codex/integrator | Critical analysis and authorized implementation | Security changes, unrestricted tools, money, secrets, production merges require explicit owner authorization |

For risky changes involving save/load, item integrity, threading, engine architecture, permissions, dependencies or CI, escalate to authorized experienced reviewer. Never merge just because unit tests are green.

## Task discovery and priorities

1. **Resume** a legitimately owned unfinished task with evidence.
2. **Review** a ready, independently submitted change within your capabilities.
3. **Execute** an available approved, explicitly compatible low-risk task.
4. **Investigate read-only** when no execution task is authorized; generate a ready-to-review task packet rather than inventing permission.
5. **Escalate** only when the dependency, complexity, risk, or verified lower-cost failure warrants it.

Before selecting work, inspect exact task issue, labels, recent comments, existing PRs/branches and overlap. Stale labels, blocked dependencies and manual-only tickets cannot be treated as free capacity.

## Handoff and evidence

Keep claims distinct: **PROPOSED** (no committed code), **IMPLEMENTED** (recorded artifact), **TESTED** (specific commands/CI at SHA), **REVIEWED** (who and what), **MERGED** (real merge SHA). Record agent identity as SELF-REPORTED unless provider metadata independently verifies it; do not invent exact model IDs.

For each test record: exact command, environment, exit status, output location, SHA, and what it actually proves. A passed Python unit test does not establish playable C++/Qt behavior. A failed test or missing CI is not PASS.

If disconnected or replacing another AI: retrieve the newest durable checkpoint, verify the actual remote HEAD and diff, start with the first unfinished **and safe** step, and never blindly rerun stale commands or redo verified work.

## Minimum check-in response

Use compact prose or a short checklist:

- "I am [provider/model or UNKNOWN], acting as [role]."
- "Mission: [one-sentence task]. Base: [branch@SHA or unknown]."
- "Allowed: [paths/actions]. Forbidden: [high-risk/irrelevant work]."
- "Mini-roadmap: S01 ...; S02 ...; S03 ... ."
- **Then do the work.** Finish with evidence, outstanding tests, provider sign-off and next exact action.

Do not say "working in the background" without an actual authorized scheduler. If repository data is inaccessible, provide a self-contained portable packet and mark it unverified.
