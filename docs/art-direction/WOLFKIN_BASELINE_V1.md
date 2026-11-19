# Wolfkin (Female) — Approved Knomira Visual Baseline v1.0

**Status:** Approved by user as the canonical Wolfkin female concept art. This is an **art-direction baseline only**, not a game-ready atlas, animated sprite set, or implemented race.

**Original approved artwork:** `/Knomira/Sprite Design/Race Baselines v1/Wolfkin/Wolfkin_Female_APPROVED.png` in ChatGPT Library.
- Source file (unaltered): `wolfkin_character_role_examples.png`
- Dimensions: **1448 × 1086** pixels (RGB PNG)
- SHA-256: `3e0302dbfcf9746e3bdc41e97539c5c347d6321575c95f484aad8d2dfe11e509`

**Reference priorities:** This exact Wolfkin sheet is the canonical race-specific reference for proportions, expressions, fur marking and clothing language. Use the approved **Felikin** concept sheet for consistent human-sized anthro rendering, and the broader `RABBITKIN_STYLE_LOCK.md` for the common Knomira retro-fantasy pixel-art style. Do not turn Wolfkin into human figures with only wolf ears or tails.

## Anatomical silhouette — Wolfkin female
- **Human-sized bipedal, mature female anthropomorphic wolves** with noticeable canine muzzle, black/brown nose, expressive angular eyes, pointed erect wolf ears, furry cheeks/neck ruffs, large fully furred tails and distinctive paws/claws.
- Fully anthro/furred limbs, furry chest/shoulder ruff and patterned fur, animal-like feet rather than regular human skin and footwear-only depiction.
- **Toned and physically powerful**, with substantial shoulders/upper arms and muscular legs, while keeping adult feminine waist/hip curves and reasonably athletic bust/torso proportions. More visibly powerful and feral than Felikin; no extreme human-realism, generic cat face, or oversized baby/chibi proportions.
- In the approved sheet the annotated **concept height is approx. 1.6–1.8 m**, roughly equal to human/Elfkin scale. Do not treat this illustration annotation as validated engine collider or tile dimensions.
- **Wolf muzzle variation:** standard soft, stronger/refined, broad/rugged. Keep species unmistakably canine; protect muzzle and nose shapes across expressions.
- **Ears:** upright/tall, slightly round, tufted/fluffy, dark-tipped. **Tails:** straight/full, curved/bushy, long plumed, flowing sickle, ruffed/thick, contrasting tipped.
- **Fur colors/markings:** gray classic, white/snow, black/sable, brown/earth, red/rust, brown sable/wild. Preserve natural cream/white muzzle/chest markings, dark mask contrast, variegated darker limbs and tail tips.

## Six canonical illustrated roles
1. **Worker:** hearty gatherer/hauler with produce backpack, belts, leather bracers, fur trims and earthy travel clothes.
2. **Warrior:** powerful armored frontline guard with sword, shield, reinforced gauntlets and chest armor, white/gray coat.
3. **Ranger:** earth/russet wolf scout with bow, quiver and green woodland cloak.
4. **Mage:** dark-furred ritualist in ornate purple/gold robes wielding a jewel-tipped staff.
5. **Tinkerer:** brown-furred engineer with goggles, wrench, utility pouches and belt-mounted tools.
6. **Elder:** mature pale wolf in elaborate teal/cream ceremonial robes, shawl and book.

These roles describe *design references*, not implemented racial abilities, starting kits, or profession mechanics.

## Reference panels to keep
- Eight expression examples: Neutral, Happy, Sly, Wink, Surprised, Pout, Thinking, Fierce.
- Distinct three-angle muzzle and four ear options, not just hair variants.
- Six tail types and six fur coat examples, with clear paw and tail patterning.
- Bipedal full-body reference turnaround: front, 3/4, side and back; paw shapes and broad tail should be consistent between views.
- Earthy/warm and dark fur accent palette with leather browns, forest greens, gold-orange and purple jewel highlights.
- Ornate dark-gold Knomira frames, cream parchment, expressive anime-inspired faces, detailed layered attire, deliberate crisp pixel clusters, stepped charcoal-plum outlines and restrained dithering.

## Reusable art prompt
> Approved Knomira female Wolfkin fantasy species, matching the exact Wolfkin reference sheet and the established Felikin anatomy scale: human-height adult anthropomorphic wolf women, fully furred, softly angular but unmistakeably canine muzzle, erect wolf ears, long shaggy thick wolf tails and visible clawed paws. Lean-muscular and athletic, firm strong shoulders, toned thighs and legs, graceful feminine waist/hips and mature curves without exaggerated bulk. Varied gray, snow-white, sable black, brown, rust and tawny fur with natural masks and chest ruffs. Six distinctive profession concepts: Worker, Warrior, Ranger, Mage, Tinkerer, Elder. Rich detailed leather, metal and woven fantasy gear, dark gold ornate parchment concept sheet with muzzle/ear/tail/fur variants, anime-fantasy expression board, native-scale 4-direction turnarounds, strong clustered pixel-art contours and selective dithering. Avoid human skin with pasted ears, feline snouts, four-legged werewolves, blurred/vector/3D rendering or inconsistent sprite scale.

## Codex / implementation boundary
The reference is not a sprite sheet with known native engine size/coordinates. Before any sprite generation/placement, perform **ART-01A** read-only inspection of real installed tilesheet files, rendering layers, animation frames, existing sprite indices, anchor points, transparent edges and directions. No atlas overwrite or sprite placement without evidence; do not implement mechanics or touch `feature/playable-db-embark-integration`. Treat aesthetic traits listed above as **concept themes, not validated gameplay effects**.
