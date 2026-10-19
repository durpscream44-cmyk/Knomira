# Start Here: Using Codex on the Ingnomia Fork

1. Put the Ingnomia source under Git and create a working fork/branch.
2. Copy `CODEX_MASTER_INSTRUCTIONS.md` into the repository root (you may rename it to `AGENTS.md` if your Codex workflow automatically reads that file).
3. Copy `CODEX_TICKETS.md` into `docs/` or the repository root.
4. Give Codex only **T01** first. Do not ask it to execute the entire queue in one run.
5. Review its diff and build/test report before moving to the next ticket.
6. For T03 and later runtime tickets, require a reproduction before accepting a fix.
7. Commit known-good work after each ticket so a bad agent change can be reverted cleanly.

## First prompt to Codex

Read `CODEX_MASTER_INSTRUCTIONS.md` completely. Work only on **T01 — Reproducible Windows build** from `CODEX_TICKETS.md`.

Before editing anything, inspect the repository's CMake files, CI configuration, submodule/dependency declarations, and current documentation. Report your build plan and the files you expect to change. Then execute the plan.

Do not make gameplay changes. Do not remove features just to make the build pass. Use the actual source/build configuration as authoritative when it conflicts with old documentation.

When finished, report status, exact files changed, build commands and results, remaining blockers/risks, and whether T01 meets its acceptance criteria. Stop after T01 and wait for review.
