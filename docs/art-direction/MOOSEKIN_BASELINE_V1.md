# Moosekin (Female) — Approved Knomira Concept Baseline v1.0

**Approval:** User approved the exact generated `Moosekin (Female) Race Reference` sheet on 2026-11-15. It is the canonical visual reference for this race, not a production sprite atlas.

**Canonical original image:** `/Knomira/Sprite Design/Race Baselines v1/Moosekin/Moosekin_Female_Original_APPROVED.png` in ChatGPT Library. Original 1536 × 1024 RGB PNG; SHA-256 `d9a82d1fe6b72f3b22e5d765282b3053743e4f4fcf6cd876381124e40f9e56b4`.

**Use alongside:** Approved Knomira **female Centaur variant and complete reference sheets** (primary anatomical and proportional baselines), and `docs/art-direction/RABBITKIN_STYLE_LOCK.md` (shared general rendering grammar). Do **not** use the separately approved **Mousekin / Mauskin** (small bipedal mousefolk) baseline for Moosekin body design; similar names are unrelated races.

## Required silhouette and anatomy
- Adult female **centaur** design: expressive, stylized humanoid upper torso rising naturally from a compact **four-legged moose-like ungulate lower body**. Four independently visible legs ending in **broad cloven moose hooves**, with a substantial shoulder/rump volume and very short tufted tail.
- Broad, flattened **palmate moose antlers**, confidently anchored to the head and occupying a readable wide silhouette. Antlers are present on these fantasy female designs by artistic choice. Do not accidentally render ordinary horse ears/horns as a substitute for antlers.
- Moose-like ears and a pronounced dark brown, tan, cream or pale-gray coat; dense fur and feathered/cuff-like leg textures; warm leather-and-cloth armor with ornate buckles, bags, capes and tools.
- Female upper-body proportions remain in the established Knomira centaur family: curvy/chibi-heroic, sturdy, compact, not elongated adult-realism or fully human biped anatomy. Heavy draft variants broaden the moose body rather than arbitrarily stretching the human upper torso.
- **Illustrated scale target (not engine-verified):** reference sheet labels total visual height approximately 1.5–1.8 m and withers 1.2–1.4 m. The huge antler span affects bounding-box/collision decisions, which are **undetermined**.

## Five canon design variants from the approved sheet
1. **Woodland (Standard)** — balanced, versatile; warm dark-brown coat and medium broad antlers, green/earth palette.
2. **Great Bullhorn (Heavy)** — largest draft build, broad shoulders, powerful hooves and oversized antlers; deep brown coat, purple/gold workwear, loaded pouches.
3. **Marshkin (Pastoral)** — softer farm/herding build with piebald patches, flower basket and apron; antlers decorated lightly with woodland greenery.
4. **Frosthorn (Northern)** — pale/gray-white coat and hair, fur-lined clothing, pale frosted antlers; colder blue-violet/silver accents.
5. **Trailrunner (Scout)** — athletic, comparatively lean forest/moose body, long legs relative to other variants without losing compact Knomira silhouette; bow/quiver and ranger cloak.

## Reference-sheet details to preserve
- **Expressions:** Neutral, Happy, Laugh, Stern, Tired, Focused.
- **Views:** Front, 3/4 front, side, 3/4 back, back. Check antler symmetry, all four legs, rump alignment and tail position across turns.
- **Antler modules:** classic broad, young small, asymmetric/natural, decorated/ribbons/charms, frosted, regal ornamentation.
- **Coat variants:** Wild Brown, Tawny, Dark Chocolate, Red-Brown, Piebald, Fawn, Gray-Tan, Snowy.
- **Modular anatomy:** head and hair, moose ears, antlers, torso/arms, four legs, cloven hooves, moose barrel/flank, and short tuft tail. Keep accessories separate where feasible.
- **Profession examples shown:** Worker, Ranger, Guard, Farmer, Elder. These are illustrative costume examples, not newly wired game jobs.
- **Palette:** warm cream/tan/chocolate/brown with charcoal outlines, purple/teal/forest green, cool frosted blue, and gold hardware accents.
- **Style:** crisp chunky intentional pixel clusters, step outlines, detailed readable costumes, selective dithering, warm parchment, dark-gold decorative borders, standardized race anatomy panels.

## Reusable generation prompt
> Knomira ornate crunchy retro-fantasy pixel-art race sheet, exactly matching the approved female Centaur references and Moosekin v1 source. Adult female Moosekin centaur: compact curvy humanoid upper torso on a robust four-legged, moose-shaped woodland body, wide palmate antlers, moose ears, cloven hooves, short tuft tail, natural moose coat variants and thick fur at the shoulders and legs. Preserve clear anatomy and readable four-leg silhouette, rich hand-made layered fantasy equipment, strong charcoal outlines, stepped clustered pixels, selective dithering, warm parchment and gold panels, expressive anime-fantasy faces. Five distinct variants Woodland, Great Bullhorn, Marshkin, Frosthorn, Trailrunner, with front/side/back turnarounds, expressions, antler variants, coat patterns and separate equipment breakdowns. No smooth vector paint, 3D realism, blurred pixels, deer-only branching antlers, horse hooves, missing legs or full-sized towering centaurs.

## Codex / asset implementation boundaries
This image is **approved art direction only**. Do not silently insert Moosekin into a texture atlas or active species list. First perform a read-only audit of real installed tilesheets, coordinates, layer ordering and engine consumer code as in `CODEX_RABBITKIN_ART01_HANDOFF.md`. A future centaur/moose asset prototype must fit native sprites, handle antler occlusion, animation turns, four hooves, and upper/lower body offsets; antlers may require a separate render layer. Do not assume this is implemented. Do not change the ongoing playable stabilization branch `feature/playable-db-embark-integration`.
