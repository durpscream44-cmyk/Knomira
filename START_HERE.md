# Join Knomira

Knomira is a single-player colony simulator under active stabilization. This is the entry point for humans and AI services, including chat-only models. This page alone authorizes no task.

1. Read [AGENTS.md](AGENTS.md) and any component-local AGENTS.md for your assigned paths.
2. Read the [coordination checkpoint](docs/agent-coordination/CURRENT_CHECKPOINT.md) and [protocol](docs/agent-coordination/README.md). Verify their branch, SHA and date against live GitHub issues, PRs and CI. A checkpoint may lag.
3. Choose a [role prompt](docs/agent-coordination/ROLE_PROMPTS.md). Declare mission, authority, prohibitions, exact starting branch/SHA, provider/model or UNVERIFIED, actual capabilities and acceptance evidence using the [mission card](docs/agent-coordination/CONTINUITY_PROTOCOL.md).
4. Check duplicate work and owner/lease. Take only a maintainer-authorized assignment. For chat-only work, use a pasted [compact packet](docs/agent-coordination/TASK_PACKET.md) and relevant source excerpts.
5. Work from the pinned base. Maintain permanent-ID roadmap steps; checkpoint after investigation, meaningful changes, before transitions and before expected context/quota limits. Hand off using [RESULT_TEMPLATE.md](docs/agent-coordination/RESULT_TEMPLATE.md).
6. An independent reviewer verifies a candidate. Do not equate a PR, compile pass or model response with accepted gameplay.

| Role | Authority |
| --- | --- |
| Free chat-only AI | Analyze, design tests and draft packets/patch text; no claimed Git or test execution |
| Junior coding agent | Approved low-risk allowlisted task in isolated branch; submit evidence for review |
| Planning coordinator | Decompose, route and detect duplicates; no self-approval or dispatch |
| Review coordinator | Preliminary diff/evidence review; no approval of own high-risk work |
| Codex-level integrator | Critical engineering, independent tests and reviewed integration when authorized |
| Security reviewer | Audit and propose recovery; no live permission change without explicit approval |

Read task-relevant source only. Search callers when changing behavior.

**Safety:** [PR #24](https://github.com/durpscream44-cmyk/Knomira/pull/24) remains draft. The runner execution path is disabled on this branch pending [#25](https://github.com/durpscream44-cmyk/Knomira/issues/25). Issues [#28](https://github.com/durpscream44-cmyk/Knomira/issues/28) and [#29](https://github.com/durpscream44-cmyk/Knomira/issues/29) permit manual user launch only; their labels are not dispatch authority. A fork/worktree is not Windows process isolation.

Use IMPLEMENTED, TESTED, PROPOSED, BLOCKED and UNVERIFIED precisely. Resume from the last verified checkpoint after checking the current remote SHA.
