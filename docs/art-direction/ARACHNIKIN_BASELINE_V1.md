# Arachnikin (Female) — Knomira Approved Concept Baseline v1.0

**Approval:** User approved the **revised** Arachnikin sheet with *short, pointed elvish ears*, superseding the initial illustration's ear design. This is the canonical reference, not an engine sprite atlas or implemented species.

**Original approved image:** `/Knomira/Sprite Design/Race Baselines v1/Arachnikin/Arachnikin_Female_Elven_Ears_APPROVED.png` in ChatGPT Library. Image: 1448×1086 RGB PNG. SHA-256: `afd50f9ce7811f6a8f9e1aa549f26e8bafebf3d07139ba68cbeaed0656cb9a59`.
**Reference lineage:** Approved **female Centaur variant/reference sheets** for compact anatomy and proportions, plus `docs/art-direction/RABBITKIN_STYLE_LOCK.md` for common crunchy ornate pixel-art rendering.

## Canonical anatomy and size
- **One coherent arachnid-centaur body**, not a separate spider mount with a rider: compact adult feminine humanoid upper body directly joined to a large eight-legged spider lower body and patterned abdomen.
- **Short elvish ears, pointed but significantly shorter than typical fantasy elves.** The user's corrected reference is authoritative on ear shape. No rounded animal ears, very long elf ears, or antennae.
- The original Knomira female centaur family sets compact/chibi-heroic adult torso proportions, expressive anime-inspired faces, legible fantasy clothing; anatomy below the waist is a spider abdomen/body with **eight articulated chitinous legs**, not four ungulate limbs.
- Approximate *concept sheet* scale: total height c. 1.4–1.7 m, spider withers c. 1.1–1.3 m. These are illustration annotations, **not verified game-world/collider metrics**.
- Use transparent separation and coherent overlays for forelegs/body/abdomen/gear when future animation work is undertaken. Validate how many legs are visible from each angle, their foot contact points, and overlapping layers; don't infer sprite slots.

## Five approved subtypes
1. **Webspinner (Standard)** — purple/gold broad, balanced spider abdomen, ornate woven web motifs; adaptable.
2. **Widowkin (Dark)** — deep black and red patterns, sleek stealth-oriented forms; perceptive/mysterious.
3. **Jumperkin (Athletic)** — compact spotted tan/brown spider body and agile explorer kit; lively.
4. **Silkweaver (Pastoral)** — pink/cream/salmon, silkcraft/herbal harvest details, soft webwork and floral touches.
5. **Frostweb (Northern)** — pale lavender/white cool-toned abdomen and chitin, dense warm cloth trim; cold-hardened.

## Reference sheet modules to retain
- **Expressions:** Neutral, Happy, Stern, Tired, Focused, Mischievous.
- **Views:** Front, 3/4 front, side, 3/4 back, back for base body, maintaining position of ears, eight legs and abdomen.
- **Modular sections:** Head/hair, eye patterns, **short elvish ear variants**, leg profiles (slender/balanced/sturdy), abdomen shapes (compact/standard/broad), spinnerets, web/abdomen markings, jewelry and gear.
- **Markings:** Orb-Weave (purple/gold), Widow Mark (red/black), spotted, silken wrap, frost web, leaf web; conserve unique form differences.
- **Six role-based *art examples*:** Worker, Ranger, Mage, Tinkerer, Guard/Warrior, Elder. These are **costumes/examples**, not currently wired race jobs.
- **Palette and look:** warm creams/peach and deep umber, purple, forest green, gold, rose, red/charcoal and frost white/lavender, high-contrast clustered pixels, crisp dark outlines, selective dithering, gold and parchment fantasy encyclopedia panels.

## Reusable visual prompt
> Approved Knomira female Arachnikin fantasy-race reference sheet in the same ornate crunchy retro pixel-art style as our female Centaur baselines: compact curvy adult female humanoid torso seamlessly emerging from a robust eight-legged spider body and round patterned abdomen; visible **short pointed elvish ears** shorter than conventional elf ears; layered leather-and-silk armor, belts, web motifs, ornate tiny gear, expressive anime-inspired faces, rich gold-bordered parchment layout, sharp charcoal contour clusters, stepped pixel outlines, hand-made highlights and restrained dithering. Show Webspinner, Widowkin, Jumperkin, Silkweaver and Frostweb variants; expression board, head and ear details, spider leg anatomy, abdomen patterns, spinnerets, front/side/back turnarounds and profession examples. Avoid long elven ears, animal ears, mounts/saddles suggesting a separate rider, four legs, human biped bodies, horse anatomy, smooth 3D, vector art, blurry pixels or inconsistent scale.

## Sprite-engine boundaries (Codex)
**Art direction ONLY.** Keep the exact original reference image available to the art-design Codex session. As with Moosekin and Rabbitkin, before inserting into any spritesheet, audit the *real* installed atlas, renderer layering, anchors, frame IDs, animation state and available space. Do not invent new tile coordinates or assume `content/tilesheet/` is checked into the repository. No implementation or runtime functionality has been verified; avoid modifying the separate `feature/playable-db-embark-integration` stabilization branch.
