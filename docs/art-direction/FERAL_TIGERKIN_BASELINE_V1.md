# Feral Tigerkin (Female) — Approved Knomira Race Design Baseline v1.0

**Status:** User-approved **visual race concept**, not implemented gameplay or engine sprites. Approved after the fully-anthropomorphic Feral Tigerkin sheet was generated, explicitly distinct from earlier Tiger Demi-Human Kin.

**Exact unaltered source image:** `/Knomira/Sprite Design/Race Baselines v1/Feral_Tigerkin/Feral_Tigerkin_Female_APPROVED.png` (1536×1024 RGB PNG; SHA-256 `50194fafba99f60f5bbf07d0695e0eb19a6390c8466d6a36e03bd44d6fe46f0a`).

**Approved reference package:** `/Knomira/Sprite Design/Race Baselines v1/Feral_Tigerkin/Knomira_Feral_Tigerkin_Baseline_v1.zip`.

## Distinction and visual anatomy

**Feral Tigerkin:** completely anthropomorphic, adult female tigerfolk with unmistakable **tiger snouts/muzzles**, furred cheeks and ruffs, prominent feline fangs, expressive cat eyes, natural striped coats over face, torso, limbs and tails, fully animal-like **padded paws, clawed fingers and feet**, long thick striped/banded tails and strong feline ears. Big powerful athletic shoulders and torso, stout/muscular thighs, substantial hips/glutes and adult feminine curves; intentionally bolder, feral and more physically formidable than generic catfolk.

**Tiger Demi-Human Kin** ([separate baseline](TIGER_DEMI_HUMAN_KIN_BASELINE_V1.md)) is the previously approved predominantly humanoid, anime-human-faced species with tiger ears, stripes, tails, paws and robust curves. **These are two different approved visual races; do not merge them or substitute their reference images.** This is not merely the demi-human sprite plus extra stripes.

## Eight coat / heritage visuals from the exact approved Feral sheet

1. **Classic:** orange with bold black stripes.
2. **White Tiger:** white and gray/purple-gray with dark high-contrast stripes (**mandatory supported coat variant**).
3. **Golden:** golden sand and black markings.
4. **Black Tiger:** onyx/dark fur and subtle markings.
5. **Red Tiger:** red/rust with black stripes.
6. **Silver:** silver/light fur with dark stripes.
7. **Sable:** brown with black stripes.
8. **Arcane:** violet and white fantasy patterning.

These refer to **visual palette variants**, not new playable subclasses or confirmed elemental abilities.

## Body types and full sheet modules

- Approved example body builds: **Athletic (lean/agile)**, **Curvy (balanced)**, **Powerful (muscular)**. All should keep strong tiger anatomy and fully anthropomorphic faces/paws. The user specifically values **strong thick thighs and a developed rear/hips**.
- Six illustrative profession styles: **Worker, Warrior, Ranger, Mage, Tinkerer, Elder**, each with fantasy equipment and readable silhouette.
- Expressions: Neutral, Happy, Playful, Wink, Serious, Confident, Teasing, Fierce.
- Ears: Standard, Tufted/Fluffy, Rounded/Thick, Notched/Wild.
- Tail forms: Classic, Fluffy, Thick, Ringed, Tufted.
- Studies: head/muzzle, eyes, fangs, paw-hands/foot pads, claws, fur/stripe textures, and forward / three-quarter / side / rear anatomical turnarounds.
- **Concept illustration size** ~1.75–2.10 m for adult females; this must not be encoded as engine units without inspection.
- The approved art style is the shared Knomira ornate crunchy retro-fantasy pixel-art language: cream parchment panels, dark brown and gilt filigree frame, small detailed pouches and weapons, rich clustered pixels, crisp charcoal outlines and selective dithering. Refer to `RABBITKIN_STYLE_LOCK.md`.

## Reusable generation prompt
> Female Feral Tigerkin in the **exact approved Knomira sheet** style: tall, fully furred anthropomorphic tiger women, genuine expressive tiger muzzles and cheek ruffs, big padded clawed paws and digitigrade-esque feline feet, long thick banded tiger tails, tiger stripes across entire body, realistic recognizable orange, white, golden, black, red, silver, sable and violet-white fantasy pattern variants. Powerful, well-developed muscular thighs, hips and glutes, broad athletic shoulders with feminine curves, confident feral feline warrior presence. Distinct body models athletic, curvy and powerfully muscular. Six role costume variants Worker, Warrior, Ranger, Mage, Tinkerer, Elder. Parchment/gold ornate fantasy character compendium with expressions, ear/tail/fur panels and multiple direction turnarounds. Maintain dark stepped linework, detailed intentional pixel clusters, select dithering and native art-family consistency. **Do not** generate an anime-human face with tiger ears; that is Tiger Demi-Human Kin, a different race. No blurred sprite pixels, smooth vector rendering, or collapsed generic Felikin/Wolfkin morphology.

## Implementation and Codex handoff boundaries
This is a saved **concept-art baseline**, not a native texture atlas or a gameplay race. No animation frames, bone/pivot sizes, species definitions, tile IDs, or coat genetics have been developed/verified. Before any production use: ART-01A read-only audit of actual available tilesheets, render references, alpha cells, offsets and native sprite scale. Keep separate from `feature/playable-db-embark-integration` and PLAY-01D stabilization work.
