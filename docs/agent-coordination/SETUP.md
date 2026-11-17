# Agent coordination setup

Jules became **read-only November 11, 2026** and is shutting down December 31. The previously proposed Jules API dispatch is disabled.

1. Review and merge the coordination draft PR. Scheduled GitHub Actions only run from default branch.
2. Run Actions > Agent queue coordinator > Run workflow > bootstrap to create labels. The 6-hour schedule **only scans** for work and never launches a model.
3. Open AI agent development issues with exact Base branch, Task kind, Required capabilities, Allowed paths, Risk, relevant stacked PRs and test acceptance.
4. A maintainer explicitly applies agent:approved, agent:ready and the provider label (agent:provider:gemini, agent:provider:kilo or agent:provider:opencode). The workflow cannot approve its own tasks.
5. On the home PC install Git, Python, GitHub CLI and an authenticated coding CLI. Run `gh auth login` and `gh auth setup-git` to enable local Git pushing.
6. Set GITHUB_REPOSITORY and GITHUB_TOKEN (from `gh auth token`) as local environment variables. Never commit credentials.
7. Run `python scripts/agent_queue.py --mode scan` to inspect the queue. To run one candidate: `python scripts/local_agent_runner.py --agent gemini --once` is a dry run; append `--execute` only when ready. Other provider names: kilo, opencode.
8. A Windows Task Scheduler task can run the chosen opt-in worker periodically while the PC is on. **Run only one dispatcher instance**. Labels are not a reliable distributed lock across multiple machines.

The runner has a 15-minute per-task CLI timeout and a final Git checkpoint attempt. It cannot predict a model's hidden context/quota; agents are instructed to save early. Only low-risk tasks with approved paths and capabilities are eligible. No engine integration or proprietary asset tasks run unattended. Git commit/push/PR failure leaves work locally and task BLOCKED. Draft PRs require actual Windows CI and Codex/human review; successful CLI exit is not gameplay proof.
