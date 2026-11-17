# Agent-specific task gates

| Agent | Allowed work | Do not assign |
|---|---|---|
| Codex | Real simulation/Qt integration, save integrity, PR review, final testing and delegation | Auto-merge or unsupported "playable" claims |
| Gemini CLI | Bounded local C++ tests, docs and tooling | Core GameManager lifecycle, save-format changes or native asset integration unattended |
| OpenCode CLI | Bounded source analysis, tests, docs/tooling with configured model | Native Windows/Qt gameplay integration without verified environment |
| Kilo CLI | Small independent tests and scripts with available model quota | Heavy unbounded refactors, complex engine ownership |
| Antigravity | Supervised isolated IDE changes | Assumed free cloud automation |
| Freebuff | Manual non-sensitive snippets or test design | GitHub writes, licensed assets, private user data |
| Jules | NONE: read-only November 11, 2026 | Every new task |

Role is an upper bound; it does not prove a running model has a shell, Qt, a GitHub login, or free usage. If missing a required capability, decline and report blocker **before consuming more tokens**.

All agents must read CODEX_DELEGATION.md and commit/push *before* hitting context or quota limits.
