# ART-01 — Knomira Rabbitkin Sprite/Placement Handoff (Codex)
**Status:** Art reference documented; binary source image not yet in this GitHub branch. In ChatGPT Library: **Rabbitkin Race Concept Sheet.png** (original 1448×1086, SHA-256 `398db612b952452773bbe5275ad7852e134a5e9072680cea1eb43cb4060f8438`). Use `docs/art-direction/RABBITKIN_STYLE_LOCK.md` as the canonical prompt/style module. A downloadable ZIP with original PNG, README, STYLE_LOCK.md and CODEX_SPRITE_HANDOFF.md was prepared separately for the sprite-design chat.

## Objective
Translate the approved six-role **concept art** into a game-scale Rabbitkin sprite proof, then identify exact non-destructive tilesheet positions **using real game data**. Do not interpret illustrated panels as frame maps or code-level IDs.

## ART-01A (first task) — Read-only atlas and placement audit
1. Read repository `AGENTS.md` and identify code that references tilesheet paths, sprite IDs/indexing, animation frames, directions, pivots, alpha handling and texture loading. Trace active callers.
2. Obtain the actual installed Ingnomia `content/tilesheet/` source graphics (these are **absent from the repository's tracked `content/` directory** on `master` as checked 2026-11-15). Identify which sprite files are actually supplied, e.g. `gnomes.png`, `goblin.png`, `animals.png`, `mobs.png`. Do not assume they exist until verified.
3. Record original dimensions, SHA-256, pixels-per-cell/frame where available, offsets, IDs, direction variants, animation layouts, transparency rules, consumer references and whether free slots exist.
4. Generate contact sheets with labeled pixel-grid overlays and a machine-readable mapping. Clearly mark VERIFIED / INFERRED / BLOCKED; do not invent sprite placement, tile sizes or anchors.

## ART-01B — One worker proof
After ART-01A has verified actual native dimensions, produce one isolated Rabbitkin worker demonstration matching style-lock anatomy/cluster rhythm at 1× game resolution. Preserve the original source, include pixel-exact PNG and 1× / 2× / gameplay-scale previews; document what ornate details were reduced for small-scale readability. Six concept roles are *design references*, not automatically six engine jobs.

## ART-01C — Placement proposal
Propose a CSV/JSON manifest (source sheet, exact `x,y,w,h`, frame/direction, anchor, original hash, target asset, consumers, conflicts) and before/after overlay for review. Never shift existing cells, overwrite gnome/goblin assets, or merge game asset changes without validated source files and visual tests.

## Boundaries
- This is a **separate ART branch**, not a playable-stabilization ticket. Do not alter `feature/playable-db-embark-integration` or derail PLAY-01D.
- No new race mechanics, data schema, combat/crafting, animation rewrites or existing renderer overhauls now.
- Keep third-party/proprietary runtime tilesheets out of the repository unless rights and redistribution are established. Existing local installation files can remain local for comparison.
- If input atlas/binary image is missing, report BLOCKED and stop before inventing positions.

## Acceptance/evidence
- Checked actual source atlas bytes and active sprite consumers; authentic screenshots or coordinate overlays; complete placement/index manifest.
- Native-resolution proof with readable silhouette + consistent outline/pixel scale and preservation of race anatomy.
- Exactly listed files changed, test commands/results, warnings, unverified risks and next step. `PASS` only if evidence supports it.
- No claims of in-game working sprites before actual in-engine visual smoke test.

## Resume prompt
> Continue Knomira **sprite design** from ART-01, not the gameplay branch. Open `docs/art-direction/RABBITKIN_STYLE_LOCK.md` and this handoff, use approved Rabbitkin source PNG from the shared ChatGPT Library/art pack, inspect native installed tilesheets first, and build an exact non-destructive sprite placement map. Begin with ART-01A. Distinguish verified from inferred; do not overwrite original graphics or implement race gameplay yet.
