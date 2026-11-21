# Multi-AI coordination roadmap

Status: proposed architecture, except for linked repository files and CI. This document contains no local host configuration or permission inventory.

## Current coordination inventory

- Draft [PR #24](https://github.com/durpscream44-cmyk/Knomira/pull/24) implements issue templates, provider profiles, handoff rules, a read-only queue scan and an opt-in local runner. It is unmerged. Python coordination checks passed at its prior head `755d1689c41bd32ded1b26129da9b0539db8adaa` in [run 44044153790](https://github.com/durpscream44-cmyk/Knomira/actions/runs/44044153790); that run does not prove Windows runner safety or gameplay.
- [#25](https://github.com/durpscream44-cmyk/Knomira/issues/25) is the pending independent runner/security acceptance. [#26](https://github.com/durpscream44-cmyk/Knomira/issues/26) is proposed mock testing. [#27](https://github.com/durpscream44-cmyk/Knomira/issues/27) is a separate high-risk gameplay audit.
- [#28](https://github.com/durpscream44-cmyk/Knomira/issues/28) and [#29](https://github.com/durpscream44-cmyk/Knomira/issues/29) specify one-file, manual-only junior tasks. Labels are metadata, not dispatch permission.
- This stacked branch implements universal onboarding, mission/roadmap/checkpoint templates and a fail-closed runner execution gate. A junior lab, automated routing and accepted gameplay integration remain **PROPOSED**.

## Priority order

1. **P0:** independently review #25; keep runner execution disabled until accepted. Review this onboarding branch against #24.
2. **P0:** continue playable stabilization on the actual verified gameplay branch. Keep save/load, threading and inventory integrity with experienced engineers; do not rewrite the engine.
3. **P1:** after safety acceptance, manually supervise one exact-scope junior task and independently review its tests and diff.
4. **P1:** implement #26 mock failure tests and small task-packet tooling only where it measurably reduces repeated work.
5. **P2:** seek explicit approval for a separate junior repository namespace, isolation design, credentials and any schedule before creating or enabling them.
6. **P2:** measure cost and review effort per accepted contribution; adjust provider routes using actual outcomes.

## Proposed junior lab

A true fork may need a separate GitHub namespace; an extra branch is not a fork. Use one pinned branch/worktree per issue, exact file allowlists, isolated credentials and pre-write enforcement. A repository fork does not isolate the operating-system process. Candidate PRs carry base/head SHA, diff, model identity, commands/results and a checkpoint. A coordinator triages duplicates and review readiness; an independent authorized integrator verifies and merges into the appropriate official development branch. Notify the official repository only of review-ready candidates. No fork, credentials or unattended execution are created here.

If the approved queue is empty, juniors may propose preapproved categories such as documentation tests, read-only source maps and small deterministic tooling. A maintainer must pin scope before coding. Juniors must not independently alter save formats, core architecture, production threading, inventory integrity, security settings or release branches.

## Ownership, identity and cost

An issue claim names owner, branch, start and expiry UTC. Expiry requires checking branches, PRs and handoffs before reassignment. Conflicts go to a maintainer. Coordinators may plan and preliminarily review, but may not approve their own high-risk work.

Record per candidate: issue, base/head SHA, provider and exact model (or UNVERIFIED), task type/difficulty, changed files, actual tests, verified cost or UNKNOWN, review minutes, corrections, scope violations, author/reviewer/integrator, accepted commit or NONE, and next action. Default to $0 additional paid inference. Prefer local, free, freemium, then paid only with authorization; reserve Codex for difficult engineering and independent verification. Optimize accepted contributions including review cost. Never bypass provider quotas or assume free chat includes unattended API use.

## Supervised permission recovery

Before changing any live Kilo or Antigravity security setting, inventory configuration sources and effective runtime permissions, preserve a backup and evidence, and prepare an exact reviewed change with rollback. Obtain explicit approval for live changes. Validate a fresh supervised session with disposable positive and negative permission tests, including forbidden files, path aliases, mixed edits, terminal and delegation. Keep any unknown effective rule blocked. Check product-specific schemas and installed versions before drafting Antigravity settings; no unverified keys or live configuration changes are supplied here.

Unattended execution requires independent #25 acceptance, pre-write and OS/credential isolation, CLI version/argument checks, failure-path tests, Windows evidence and explicit approval of the launcher and credential scope. A queue scan or post-run diff check cannot certify this.
