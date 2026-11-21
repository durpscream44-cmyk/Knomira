# Compact universal assignment packet

Use this in a GitHub issue or paste it into ChatGPT, Grok, Copilot, Gemini or another service. Link optional depth; do not paste whole conversations or repository dumps. A packet proposes work until a maintainer authorizes it.

```markdown
# Packet <permanent ID>: <title>
Issue/owner/lease: <URL, owner, UTC expiry or NONE>
Mission/success: <one measurable outcome>
Provider/model/role: <exact ID or UNVERIFIED; role>
Repo: durpscream44-cmyk/Knomira
Start: <branch> @ <full approved SHA>; PR target: <branch>
Authority: <exact actions/files>; prohibited: <actions/paths/data>
Required capabilities and actual limits: <tools, OS, quota, access>
Relevant source: <2–6 paths/links>; optional deep references: <links>
Known findings/previous checkpoint: <links and what not to repeat>
Roadmap: <ID-01> NOT STARTED — inspect; <ID-02> NOT STARTED — change;
         <ID-03> NOT STARTED — verify/handoff
Acceptance: <exact commands, expected evidence, independent reviewer>
Checkpoint: CONTINUITY_PROTOCOL.md after investigation, meaningful changes,
            transitions and before context/quota pressure
Sign-off: <agent/model, local SHA, pushed SHA or NOT PUSHED, changed files,
           actual tests, risks, next exact action>
```

If the approved base SHA is stale, stop and ask the owner to rebase the packet. Chat-only work marks all repository/test claims UNVERIFIED.

## Example: manual junior issue #28

Mission: add unit coverage for existing `docs/lib/util.py` sprite coordinate helpers. Start: `master` @ `7d481cd29df10dd22a7bda3a84a6ced76d94be1a`. [Issue #28](https://github.com/durpscream44-cmyk/Knomira/issues/28) is authoritative. Only `docs/tests/test_sprite_coordinates.py` may change. Roadmap: `KILO-28-01` inspect, `KILO-28-02` write tests, `KILO-28-03` reviewer verifies. Acceptance: `python -B -m unittest discover -s docs/tests -p test_sprite_coordinates.py -v`, `git diff --check`, one-file diff, Codex sign-off. Python 3.11 required by issue. **BLOCKED for launch** until the November 19 Kilo audit's effective-runtime and approval gate passes. `manual:ready` never authorizes unattended dispatch.

## Example: chat-only issue #29 review

Mission: inspect proposed item-quantity test cases in [#29](https://github.com/durpscream44-cmyk/Knomira/issues/29) against pasted `docs/lib/util.py` functions. Start: issue-pinned `master` @ `7d481cd29df10dd22a7bda3a84a6ced76d94be1a`. Authority: analysis only, no repository write. Roadmap: `CHAT-29-01` inspect supplied behavior, `CHAT-29-02` propose case review, `CHAT-29-03` hand off. Acceptance: cited input/output examples and explicit TESTS NOT RUN; Codex decides whether to amend the issue.
