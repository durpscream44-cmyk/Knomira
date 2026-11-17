# Knomira agent task registry — live GitHub Issues

GitHub Issues was enabled on November 17, 2026. The earlier file-only proposed tasks have now been converted to **unapproved** GitHub Issues:

| Task | GitHub Issue | Status | Assigned work |
| --- | --- | --- | --- |
| COORD-01 | [#25](https://github.com/durpscream44-cmyk/Knomira/issues/25) | Proposed / manual safety review | Codex/Windows acceptance and runner security |
| COORD-02 | [#26](https://github.com/durpscream44-cmyk/Knomira/issues/26) | Proposed / dependent on COORD-01 | Gemini/OpenCode candidate for mock-only tests |
| PLAY-01F | [#27](https://github.com/durpscream44-cmyk/Knomira/issues/27) | Proposed / Codex high-risk | Audit the next live RmlUi-to-colony integration blocker |

Do not create duplicate issues from these historical packets. **Do not apply agent:approved or agent:ready** until the safety review and specific scope approval. Pull request [#24](https://github.com/durpscream44-cmyk/Knomira/pull/24) is a draft, unmerged, and the local CLI worker has NOT been validated on the user's Windows computer.

## Existing gameplay stack
- [PLAY-01D #21](https://github.com/durpscream44-cmyk/Knomira/pull/21) — real GameManager lifecycle, draft, not merged.
- [PLAY-01E #22](https://github.com/durpscream44-cmyk/Knomira/pull/22) — original colony save/reload smoke, stacked draft, not merged.
- Do not infer a playable RmlUi frontend from these isolated tests.

## Queue workflow
The GitHub Actions queue-scanning workflow is proposed in draft #24 and is **not yet scheduled** (must be reviewed and merged to default branch). Initial `bootstrap` label setup must run only after the workflow becomes available on `master`.

## Session continuity
All agents follow [CODEX_DELEGATION.md](CODEX_DELEGATION.md) and [RESULT_TEMPLATE.md](RESULT_TEMPLATE.md). A report that lacks a pushed SHA or an explicit unsynced local recovery path is incomplete.
