# Agent task handoff / work log

- Issue: #...
- Agent/provider/model:
- Status: PASS | PARTIAL | BLOCKED | NOT A BUG | DORMANT
- Started (UTC), finished (UTC):
- Target base branch + starting commit SHA:
- Working branch + ending commit SHA:
- PR: (draft link, or explicit "none")
- Existing related PRs/issues reviewed:

## Investigated
What actual code path was traced; reproduction, hypothesis or observed root cause.

## Files changed and diff
Exact paths, clear behavior change, and any save-format implications.

## Verification (facts only)
| Command or CI URL + SHA | Result | What this proves |
| --- | --- | --- |
| ... | PASS/FAIL/NOT RUN | ... |

## Not verified / blockers
Include external assets, Windows desktop limitations, timing, CI failures, model uncertainty and partial tests.

## Next safe task
Specific next action, dependency and files. Another agent must be able to resume using only repo state and this log.

## Integration status
Isolated test only | PR awaiting review | Merged at SHA (with proof). Do **not** label work merged if PR remains draft/open.

## Mandatory context/quota boundary
- Last actually pushed branch and commit (or NOT PUSHED):
- Local-only worktree or patch location:
- What the next agent MUST NOT repeat:
- Exact resume command, missing tests and blockers:
- Provider/context/quota reason for stopping:
