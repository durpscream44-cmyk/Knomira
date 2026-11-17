# Proposed Knomira assignments — not approved for execution

These durable task packets exist because GitHub Issues were disabled when the multi-agent coordination PR was created. They are **proposals only**, not ready-to-run jobs. After enabling Issues and reviewing PR #24, copy relevant packets into GitHub Issues with the new task form. Do not auto-run without maintainer authorization and a capability check.

## COORD-01 — Windows runner acceptance test (Codex/human)
- Status: PROPOSED / BLOCKED by disabled GitHub Issues and no Windows execution in this chat.
- Base branch: `feature/multi-agent-github-coordinator` (read live SHA before starting).
- Task kind: windows-runtime; Risk: high.
- Required capabilities: git, github, windows, cmake; review-only unless explicitly approved.
- Allowed paths: `scripts/`, `docs/agent-coordination/`, `.github/workflows/`.
- Dependencies: [draft PR #24](https://github.com/durpscream44-cmyk/Knomira/pull/24). No gameplay PR changes.
- Acceptance: Windows test matrix for Python parser, protected paths, interruption/timeout, Git auth failures, duplicate-task refusal, PR creation, log preservation, no unauthorized pushes. Verify actual Gemini/Kilo/OpenCode CLI invocation separately; do not spend provider quota without consent. Leave a real SHA and RESULT_TEMPLATE.md report.

## COORD-02 — Expand agent-runner mock unit tests (Gemini or OpenCode)
- Status: PROPOSED / not approved, must depend on COORD-01 review if security-sensitive.
- Base branch: the accepted coordinator implementation branch at a recorded SHA.
- Task kind: tests; Risk: low after review.
- Required capabilities: git, github.
- Allowed paths: `scripts/test_agent_queue.py`, `scripts/test_local_agent_runner.py`.
- Acceptance: mock repo/network/CLI without using model requests or changing real GitHub state; cover malformed task form, missing capabilities, protected path refusal, incomplete handoff. Independent CI proves the new tests actually run.

## PLAY-01F — Find the next *real* playable-shell integration blocker (Codex)
- Status: PROPOSED / BLOCKED pending exact PR and CI reconciliation.
- Candidate base: verify stacked [PLAY-01D PR #21](https://github.com/durpscream44-cmyk/Knomira/pull/21) and [PLAY-01E PR #22](https://github.com/durpscream44-cmyk/Knomira/pull/22); **do not assume either is merged**.
- Task kind: engine-integration; Risk: high; provider: Codex with Windows C++/Qt testing.
- Goal: read-only audit first of RmlUi shell -> real GameManager/colony/world renderer bridge, scoped to one actual next blocker. Do not self-assign implementation until a tested branch/commit is known.
- Acceptance: small evidence-backed task packet, actual call paths and Windows-specific reproduction; no unverified playable claim.

## Shared rule
Codex may propose up to three next tasks before hitting quota/context limits. It must include existing branches, commit SHA, possible duplicate PRs, tests, out-of-scope files and a complete interruption handoff. Other agents decline tasks that they cannot perform with their current tools. Never infer success from a chat transcript.
