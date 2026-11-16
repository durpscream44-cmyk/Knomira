# Knomira Race Design Baseline v1.0 — Rabbitkin, Tanukikin, Mousekin

**Status:** Approved **concept-art** direction; game runtime sprite-placement and new race implementation are **not verified**.

**Canonical artwork in ChatGPT Library:** `/Knomira/Sprite Design/Race Baselines v1/`
- `Knomira_Rabbit_Tanuki_Mouse_Baseline_v1.png` — three-race combined visual index (generated composite, **not an exact pixel-preserving assemblage**).
- `Rabbitkin_Original_Approved.png` — original approved Rabbitkin source.
- `Tanukikin_Original_Approved.png` — original approved Tanukikin source.
- `Mousekin_APPROVED_Second_Revision.png` — **second, slimmer Mousekin sheet is canonical**; do not substitute the first bulkier Mousekin attempt.
- `Rabbit_Tanuki_Original_Baseline.png` — earlier two-race visual index.

For precise anatomy/costume/facial detail, **use each original approved race sheet before the combined index**. Any deviations in regenerated overview art should be resolved in favor of approved single-race references.

## Universal visual language
See `RABBITKIN_STYLE_LOCK.md`. Ornate crunchy retro-fantasy pixel art: intentional crisp pixel clusters and stepped contours; thick dark charcoal/plum outlines; selective dithering; warm parchment and gold reference-sheet borders; controlled highlights; intricate layered equipment (buckles, pouches, embroidery, boots, props); expressive anime-inspired adult faces; readable compact silhouettes; six art roles Worker, Warrior, Ranger, Mage, Tinkerer and Elder; expressions, anatomy variants, palette, fur patterns, directional turnarounds.

## Race distinctions (concept-scale, unverified against engine)
| Race | Relative height | Silhouette | Identifying anatomy |
| --- | --- | --- | --- |
| Rabbitkin | c. 0.9–1.1 m | compact, sturdy, shortstack | long upright/swept/lop rabbit ears; small plush cotton tail; variable pink/cream/brown fur |
| Tanukikin | c. 0.9–1.1 m | compact, rounded and robust | dark eye mask, small round ears, dense striped ringtail, raccoon-dog fur |
| Mousekin | **c. 1.0–1.2 m** (slightly taller than Tanukikin) | **slimmer than Rabbitkin/Tanukikin, roughly half the bulk of the rejected first Mousekin attempt**, compact curvy adult outline | oversized round pink ears, delicate mouse snout, thin long hairless-looking mouse tail, cream/gray/brown/sable/spotted/piebald fur |

Do **not** translate these estimates into new code scales or collider values without auditing the game.

### Mousekin anatomy prompt module (APPROVED second revision)
Adult female Mousekin, gnome-scale but slightly taller than Tanukikin; slim/compact waist and more refined shoulder/arm/leg width than the first attempt, curvy and mature proportions with sturdy thighs and rounded hips rather than a very heavy build. Large round soft pink ears, fine whiskers and small mouse muzzle, long thin pink tail, expressive eyes; fur examples cream, gray, brown, sable, spotted and piebald; stylish layered fantasy profession gear matching Rabbitkin/Tanukikin rendering detail. **Maintain the exact proportions and outfit detail of `Mousekin_APPROVED_Second_Revision.png`**, not the bulky initial Mousekin sheet.

## Codex handoff
1. Start from `docs/art-direction/RABBITKIN_STYLE_LOCK.md` and this document; attach original baseline images from Library.
2. Keep the image baseline as **visual reference**, not game-ready atlas cells.
3. Before creating new sprites, perform ART-01A read-only audit of the actual installed original sprite sheets and engine index/caller code, then validate native-resolution silhouettes and palette on a separate art branch.
4. No automatic overwriting, tile ID changes, unverified atlas placement, new playable race mechanics, or modification to the `feature/playable-db-embark-integration` stabilization branch.
