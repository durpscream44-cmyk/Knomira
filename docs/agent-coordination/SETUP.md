# Multi-agent coordinator setup (GitHub Actions + Jules)

## Activation checklist
1. Merge the coordination PR into the default branch **after reviewing it**. Workflows present only on a feature branch are not a scheduled running service.
2. Run Actions > `Agent queue coordinator` > Run workflow > `bootstrap` to create the queue labels.
3. Make GitHub Issues with the [agent task issue form](../../.github/ISSUE_TEMPLATE/agent-task.yml) or use [TASK_TEMPLATE.md](TASK_TEMPLATE.md). Fill in a real, existing `Base branch` and meaningful acceptance tests.
4. Review issues for duplicate work/overlapping files. Only a maintainer should add `agent:approved` and `agent:ready`. For Jules, also add `agent:provider:jules`.
5. Open [Jules](https://jules.google.com), install/authorize its GitHub app for this repository, and generate an API key under settings.
6. Run `GET https://jules.googleapis.com/v1alpha/sources` with `x-goog-api-key` privately to find the exact Knomira source resource name. Do **not** commit or display the key or guess the identifier.
7. GitHub repository Settings > Secrets and variables > Actions:
   - Secret `JULES_API_KEY` (real provider secret)
   - Variable `JULES_SOURCE` (e.g., a `sources/...` resource returned by the Jules API)
   - Variable `AGENT_JULES_ENABLED` = `true` to enable actual dispatch
8. Run the workflow in `scan` mode first, then `run`. A schedule also runs every six hours; missing key/source/enable flag leaves it read-only.
9. Check issue comments for the Jules session link; the next runs reconcile `COMPLETED` or `FAILED` sessions, annotate PRs when returned and move issues into review/blocked. Human/Codex review is mandatory before merge.
10. Disable launches immediately by setting `AGENT_JULES_ENABLED` to `false`; this does not terminate already-started sessions.

## Safety and limitations
- Max one new task per dispatch and max one active tracked Jules task at a time. Failed/orphaned claims stay blocked until checked, avoiding duplicate paid/free requests.
- Enable only known low-risk tasks first. A maintainer-approved issue body is passed to an external model; public repo code and issue descriptions are not private.
- Existing CMake workflow may require proprietary assets/keys; don't send these to Jules or commit them. A Jules PR needs native Windows CI verification.
- Jules quota, plan behavior and `v1alpha` API may change. There is no guaranteed automatic transfer to Gemini, Kilo, or Codex.
- GitHub Actions workflow permissions are `contents: read` and `issues: write`. The Jules GitHub app, not the Actions token, opens code PRs.
- To review queue locally: `python scripts/agent_queue.py --mode scan` with `GITHUB_TOKEN`, `GITHUB_REPOSITORY` set.
- To test locally without any provider: `python -m unittest discover -s scripts -p 'test_agent_queue.py'`.

## Manual agents
Codex/Gemini/Kilo/OpenCode/Freebuff agents read `AGENTS.md`, this protocol, the relevant issue and its dependencies; record a claim and link their branch/PR, and deliver a result log. Switching agents is **a fresh task or committed checkpoint handoff**, never simultaneous edits to the same worktree.
