# Knomira Race Design Baseline v2.2 — Female Vampirekin registered

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

## Additional approved centaur-family baseline — Moosekin (Female)
**Canonical design:** [MOOSEKIN_BASELINE_V1.md](MOOSEKIN_BASELINE_V1.md). The approved 1536×1024 source image is in ChatGPT Library at `/Knomira/Sprite Design/Race Baselines v1/Moosekin/Moosekin_Female_Original_APPROVED.png`; SHA-256 `d9a82d1fe6b72f3b22e5d765282b3053743e4f4fcf6cd876381124e40f9e56b4`.

**Do not confuse Moosekin with Mousekin.** The latter are slender bipedal mousefolk. Moosekin are *female antlered centaurs*, using the original Knomira female centaur reference sheets as the primary anatomy/size family, plus the shared crunchy pixel-art style language. Body plan: compact humanoid adult upper half, moose-like four-legged lower half, palmated antlers, cloven hooves, broad back/rump, moose ears, short tuft tail.

Five approved aesthetic variants: **Woodland (Standard), Great Bullhorn (Heavy), Marshkin (Pastoral), Frosthorn (Northern), Trailrunner (Scout)**. See the dedicated document for exact variant palettes, visual roles, expressions, coat designs, modular antlers, turnarounds, and Codex constraints. All remain *visual concepts*, not wired game species, texture IDs, or animation data.

## Additional approved arachnid-centaur baseline — Arachnikin (Female)
**Canonical sheet:** [ARACHNIKIN_BASELINE_V1.md](ARACHNIKIN_BASELINE_V1.md) and original PNG in ChatGPT Library at `/Knomira/Sprite Design/Race Baselines v1/Arachnikin/Arachnikin_Female_Elven_Ears_APPROVED.png` (1448×1086, SHA-256 `afd50f9ce7811f6a8f9e1aa549f26e8bafebf3d07139ba68cbeaed0656cb9a59`). This **revised** image is authoritative, superseding the earlier version's ears.

**Anatomy:** compact adult female upper torso joined as one body with a robust **eight-legged spider lower half** and patterned abdomen, matching the original female Centaur family's stylized proportions and equipment quality. **Short pointed elvish ears** (shorter than conventional elf ears), not human round/animal ears. No horse body or separate mount. Concept sheet gives c. **1.4–1.7 m total height** and **1.1–1.3 m withers**; unverified in engine.

Five distinct approved variants: **Webspinner (Standard), Widowkin (Dark), Jumperkin (Athletic), Silkweaver (Pastoral), Frostweb (Northern)**. Preserve expressions, body turnarounds, spider-leg and spinneret shapes, abdomen/web markings, color palettes, and six illustrative role costumes. This is a **concept-art registration**, not an implemented species, sprite mapping or playable feature. ART-01A atlas audit and validation remain prerequisites for any game asset placement.

## Additional approved elven-family baselines — Regular Elfkin and Dark Elfkin

Two approved elven visual designs share the universal art standard but **must not be conflated**:

- **[Regular Elfkin (slender approved revision)](ELFKIN_BASELINE_V1.md)** — tall, slender elegant adult female elf body plan, c. 1.6–1.8 m concept scale, long pointed ears, narrow shoulders and limbs, understated curves, conventional leafy/fantasy cloth and armor. **The user's second, thinner Elfkin illustration is the canonical source**; the first thick Elfkin render is superseded.
- **[Dark Elfkin (gothic approved version)](DARK_ELFKIN_BASELINE_V1.md)** — based on the slender Elfkin anatomy but with fuller, more curvaceous proportions, dusky lavender/violet skin, dramatic silver hair and red/violet eyes, fitted gothic black leather, dark capes and gold filigree. **Approved by user**, not to be blended into the regular elf palette.

**Canonical images in ChatGPT Library:**
`/Knomira/Sprite Design/Race Baselines v1/Elfkin/Elfkin_Regular_Slender_APPROVED.png` (1536×1024, SHA-256 `11b809a95365d1c4f91cb1d0aa5c88bd3d41ff2c063c4c411f08aa198b214a76`)
and `/Knomira/Sprite Design/Race Baselines v1/Elfkin/Dark_Elfkin_Gothic_APPROVED.png` (1448×1086, SHA-256 `a97cc26fbddf68f249644dbb93bc05dd0b651c09480ff9964631c6ccea25443b`).

**Forest Elfkin candidate (NOT YET APPROVED):** `/Knomira/Sprite Design/Race Baselines v1/Elfkin/Forest_Elfkin_ATHLETIC_CONCEPT_PENDING_APPROVAL.png`; tan/deep-brown complexion options, Hishi Amazon–influenced athletic/tomboyish energy, long pointed elf ears, fieldwork/woodland material culture and clothing, lean toned muscular silhouette. This draft is a distinct forest branch and is **not currently an approved art baseline**. Do not derive production sprites until the user approves/adjusts it.

All are design references only; do not write runtime species logic or change tile sheet IDs without the ART-01A atlas audit.

## Approved Wolfkin (Female) — human-sized feral canine race

**Canonical design:** [WOLFKIN_BASELINE_V1.md](WOLFKIN_BASELINE_V1.md), backed by the **exact original approved image** `/Knomira/Sprite Design/Race Baselines v1/Wolfkin/Wolfkin_Female_APPROVED.png` in ChatGPT Library. Original **1448 × 1086** RGB PNG, SHA-256 `3e0302dbfcf9746e3bdc41e97539c5c347d6321575c95f484aad8d2dfe11e509`.

**Anatomy:** Adult female **fully anthropomorphic wolves**, not humans with pasted-on ears. Human-scale ~**1.6–1.8 m** *concept illustration* (unverified in engine), bipedal with strong canine muzzle and nose, triangular wolf ears, shaggy furry neck/shoulders, distinctive furry legs and paw feet, long plumed bushy tail. **Toned, lean-muscular, physically powerful and feral** while preserving mature feminine contours; distinct from the shorter, rounder Rabbitkin/Tanukikin and feline-muzzled Felikin.

**Image style lock:** Classic six-role Worker, Warrior, Ranger, Mage, Tinkerer, Elder lineup; ear/muzzle/tail/fur pattern palettes, eight expressions, front/side/back shape turnarounds; dark-gold parchment ornamental frame, rich clustered sharp pixel-art textures and fine equipment. Gray, white, black, brown, rust, sable coats. Preserve the exact approved reference for future concept generations and treat game placement/animation as **NOT YET VERIFIED**.

**Naming:** Wolfkin and Felikin are distinct canine/feline *anthro races*; Wolfkin should not inherit the Felikin cat tail or cat muzzle.

## Approved Harpykin (Female) — lower-back-winged avian humanoids

**Canonical artwork:** [HARPYKIN_BASELINE_V1.md](HARPYKIN_BASELINE_V1.md) with the exact **second/final revised** PNG preserved in ChatGPT Library at `/Knomira/Sprite Design/Race Baselines v1/Harpykin/Harpykin_Female_Lower_Back_Wings_APPROVED.png` (1448×1086 RGB PNG; SHA-256 `44a42b56d349cfffeb61968fe6b9c6fd42839fd4779fde92bbb88e6d38e3333b`). This approved revision **supersedes** the initial first Harpykin image; do not use the latter as canonical.

**Mandatory anatomy:** Adult female avian humanoids with normal arms and **separate feathered wings emerging from the LOWER BACK near the hip/pelvic region**, *not* shoulder blades. Legs end in clawed talons and feathered tails are visible. Front, 3/4, side and back views must agree about wing root placement. Humanlike upright upper body; approx. 1.6–1.8 m concept-only scale, not validated game heights.

**Four approved variants:** **Falcon** (tribal tan/brown, light and lithe scout), **American Eagle** (athletic, broad dark/white wings, armed guardian), **Dove** (slender elegant pale lilac/ivory healer/diplomat), and **Owl** (intentionally pudgy, voluptuous, thickly feathered nocturnal scholar). Falcon/Eagle/Dove are appreciably *lither* than the original generation while retaining their **approved mature feminine chest proportions**. Only Owl keeps the very rounded soft, full-bodied figure. Maintain the common Knomira crisp pixel-art/gold-parchment styling and their separate wing/tail/feather variants.

**Art only:** No active engine sprite sheet, wing animations, flight physics, game species integration, atlas indices or tested gameplay. Complete ART-01A audit and engine-specific layering/pivot plan before implementation; avoid changes to `feature/playable-db-embark-integration` / PLAY-01D.

## Approved Foxkin (Female) — elegant feral fox anthro
**Canonical reference:** [FOXKIN_BASELINE_V1.md](FOXKIN_BASELINE_V1.md); exact approved image in ChatGPT Library at `/Knomira/Sprite Design/Race Baselines v1/Foxkin/Foxkin_Female_APPROVED.png`. Original 1448×1086 RGB PNG, SHA-256 `7b4938fbe27fb7a5126df8b4a4bd58be3c8a668a09c5a9083f08f18bbabf4edd`.

**Distinctive visual lock:** adult feminine, lean/sly **fully anthropomorphic foxfolk** with delicate true fox muzzles, rich cheek/limb fur, erect vixen ears, claws/paws, fluffy broad fox tails and confident, charming trickster / refined noble presence. Asian-inspired fantasy embroidered layered clothing, flowers, ornaments, silk, tassels, gold filigree; seven+ coat families including red, silver, arctic, fennec, cross, crimson, kitsune multi-tail, and shadow fox. Approximately 1.5–1.7m concept-size, not verified against engine. The six Worker/Warrior/Ranger/Mage/Tinkerer/Elder outfits and expression/ear/tail/paw/body/coat reference panels are authoritative **concept art only**.

## Superseded earlier full-reptilian Dragonkin artwork — archived reference ONLY

The older standalone `Dragonkin_Full_Female_APPROVED.png` shows **fully reptilian dragon-headed women**. The user later explicitly rejected that as the primary design and chose the composite sheet's **upper-right Dragonkin (Dragonborn)** characters instead. The older full-reptilian art is retained as an archive/possible variant, **NOT** the canonical Dragonkin appearance. See [the corrected Dragonkin v2.0 specification](DRAGONKIN_BASELINE_V1.md) and the bottom-of-file correction block for the **exact** approved source.

## Approved Demi-Human Dragonkin / Dragonkin (Dragonborn) — Eight Heritage Variants

**CURRENT APPROVED SHEET:** [DEMI_HUMAN_DRAGONKIN_BASELINE_V1.md](DEMI_HUMAN_DRAGONKIN_BASELINE_V1.md). The user approved the newest exact original **Dragonkin (Dragonborn)** image with eight heritage portraits as the **Demi-Human Dragons** baseline. Primary image in ChatGPT Library at `/Knomira/Sprite Design/Race Baselines v1/Dragonkin/Demi_Human_Dragonkin_Heritage_APPROVED.png` (1536×1024 RGB PNG; SHA-256 `a718baf2d23d2b188caa8c9df83442d98958feaf87f403889d6ae575dfdad778`). This becomes the primary reference for future character sheets and replaces the earlier lightly scaled human candidate.

**Body design:** elegant recognizable humanlike faces, dramatic substantial horns, scaled shoulders/arms/hips/thighs, robust curved dragon tails, pronounced muscular abs and powerful feminine builds. Optional wings. **Not** fully reptilian headed, **not** merely a human with tiny thigh-scale patches. Main sheet has Worker/Warrior/Ranger/Mage/Tinkerer/Elder roles; heritage variants **Flame, Frost, Storm, Earth, Void, Nature, Light, Shadow**. The generated height label is erroneous and not an engine constraint. `DEMI_DRAGONKIN_CANDIDATE_V1.md` is archived; **Demi-Human Dragons and Dragonkin (Dragonborn) are art-name aliases for the approved style, not automatically separate playable races**.

## Dragonkin original design foundation — user's top-right Dragonborn panel
**IMPORTANT supersession:** The user confirmed the **top-right panel labeled Dragonkin (Dragonborn)** from the composite Foxkin/Dragonkin/Demi-Dragonkin illustration is **exactly** the dragon-to-human balance desired for future canonical Dragonkin race sheets. It is **not** "too dragon-like." Earlier assertions to that effect are wrong. This later user confirmation takes precedence over the previous full dragon-headed Dragonkin baseline.

- **Original foundational crop:** `/Knomira/Sprite Design/Race Baselines v1/Dragonkin/Dragonkin_Dragonborn_TOP_RIGHT_CANONICAL.png` (original exact crop from user-uploaded image; 836×610; SHA-256 `888095a4f7aaa83ff09481ed2d5e0ddc4923e3baf16c23fdba4e8f7c26f0094b`).
- **Canonical updated spec:** [DRAGONKIN_BASELINE_V1.md](DRAGONKIN_BASELINE_V1.md) **v3.0**, which defers to the latest heritage-expanded image and replaces the former full-reptilian artwork as the race's primary visual guide. Use the latest reference and spec where they disagree.
- **Body style:** humanlike faces/hair with prominent horns, substantial draconic scales on shoulders/arms/legs, strong visible abs, powerful female anatomy, thick curled scaled tail, optional wings and high-fantasy armor. This originally selected panel is the foundation for the newly approved **Demi-Human Dragonkin / Dragonkin (Dragonborn)** heritage-expanded sheet above; it has humanlike faces and clear draconic features but not fully reptilian heads.
- **Superseded image:** `Dragonkin_Full_Female_APPROVED.png` is now an *archived alternative only*, no longer canonical for standard Dragonkin.
- **Do not confuse:** `DEMI_DRAGONKIN_CANDIDATE_V1.md` is an **archived, superseded** minimally scaled concept; the latest heritage-expanded sheet above is the currently approved design.
- **Scope:** illustration reference only; not engine sprite or gameplay implementation.

## Approved Tiger Demi-Human Kin (Female) — predominantly humanoid tigerfolk
**Exact approved source:** [TIGER_DEMI_HUMAN_KIN_BASELINE_V1.md](TIGER_DEMI_HUMAN_KIN_BASELINE_V1.md), with canonical `/Knomira/Sprite Design/Race Baselines v1/Tiger_Demi_Human_Kin/Tiger_Demi_Human_Kin_Female_APPROVED.png` (1536×1024 RGB PNG; SHA-256 `244e47033c87211c9e7b38171c2f3514728a4ed321bee437127559b9901ac9f9`). This earlier design has **humanlike faces** with tiger ears, visible stripes, feline claw/paw accents and tails; strong thick thighs/hips and mature powerful feminine proportions. Eight coats including White Tiger. The generated original title said Tigerkin/Feral Tigerkin, but the user explicitly renamed it **Tiger Demi-Human Kin**. Do not conflate with full-anthro Feral Tigerkin below.

## Approved Feral Tigerkin (Female) — fully anthropomorphic tigerfolk
**Exact approved source:** [FERAL_TIGERKIN_BASELINE_V1.md](FERAL_TIGERKIN_BASELINE_V1.md), with canonical `/Knomira/Sprite Design/Race Baselines v1/Feral_Tigerkin/Feral_Tigerkin_Female_APPROVED.png` (1536×1024 RGB PNG; SHA-256 `50194fafba99f60f5bbf07d0695e0eb19a6390c8466d6a36e03bd44d6fe46f0a`). Latest user instruction explicitly approved this as **a different base race** from Tiger Demi-Human Kin. Full tiger muzzles/fangs and head fur, muscular fully furred striped limbs, wide feline paw pads/claws, thick ringed/striped tails. Adult female silhouette stays powerfully muscular with strong thighs and broad hips/glutes. Eight coat variants: Classic, White Tiger, Golden, Black Tiger, Red Tiger, Silver, Sable and Arcane. Three robust body build studies; six illustrative profession costumes, expression, tail, ear, paw, muzzle, pattern and body turnaround references.

**CRITICAL distinction:** **Tiger Demi-Human Kin = primarily human-faced with tiger features**; **Feral Tigerkin = fully furred tiger-headed anthropomorphic bodies**. Neither should substitute for the other. Both remain **approved visual concepts only**, not yet gameplay species or native sprite sheets. ART-01A audit required for actual placement. No gameplay-stabilization changes.

## Approved Human (Female) — natural skin-tone variants

**Exact reference:** [HUMAN_BASELINE_V1.md](HUMAN_BASELINE_V1.md) and unmodified approved image at `/Knomira/Sprite Design/Race Baselines v1/Human/Human_Female_Original_APPROVED.png` (1536×1024 RGB, SHA-256 `f595427faa3f08444022b95c3116ae1d26727c8894a270b15657780c94fdb80c`). Mature ordinary **human** anatomy, no animal traits, and diversity of natural **pale, fair, olive, tan, brown, deep, dark, ebony** skin-tone illustrations; all colors available across all roles/body types. Six visual role examples Worker, Warrior, Ranger, Mage, Tinkerer, Elder; expressive facial samples, hair/eye options and athletic, curvy, fuller and strong builds. The illustrated ~1.55–1.80m height is a concept-only annotation, not engine scale. **Do not** attribute ability, temperament, ethnicity or profession to skin tone.

## Approved Gnome (Female)

**Canonical:** [GNOME_BASELINE_V1.md](GNOME_BASELINE_V1.md), exact `/Knomira/Sprite Design/Race Baselines v1/Gnome/Gnome_Female_APPROVED.png` (1536×1024 RGB, SHA-256 `048f8b18a2f6d620892c818b5d22c624bbc0fa12ec9100b7f22eec8fb2c273c5`). **User explicitly approved the previous candidate without revisions**. Short adult sturdy and curvy silhouettes, pointed gnome ears, elaborate practical work gear, goggles, hairstyles and natural skin tones. Six illustrated roles and turnarounds. `GNOME_CANDIDATE_V1.md` is a historical superseded proposal. Art approval only, native game scale unverified.


## Approved Feral Hyenakin (Female) — fully anthropomorphic

**Canonical reference:** [HYENAKIN_BASELINE_V1.md](HYENAKIN_BASELINE_V1.md), original unmodified image `/Knomira/Sprite Design/Race Baselines v1/Hyenakin/Hyenakin_Feral_Female_APPROVED.png` (1536×1024 RGB PNG, SHA-256 `c44e3b8db4934f47e4687495f3d62c3ccc113da68992134570b4e5fc1e55df70`). **Feral Hyenakin have true hyena snouts/jaws, fully furred body, strong digitigrade/pawed limbs, spotted/striped coat, large furry mane and tail**, typically strong/muscular but with lithe and plump adult variants. The iconic toothy laugh/grin is key, with a range of expressions; six role costumes, full art/anatomy modules, multiple coat colors. Distinct from Wolfkin and Feral Tigerkin.

## Approved Demi-Hyenakin (Female) — revised, 90%-human hybrid

**Canonical reference:** [DEMI_HYENAKIN_BASELINE_V1.md](DEMI_HYENAKIN_BASELINE_V1.md), exact latest revised image `/Knomira/Sprite Design/Race Baselines v1/Demi_Hyenakin/Demi_Hyenakin_Female_Revised_APPROVED.png` (1536×1024 RGB PNG, SHA-256 `5180f5cf05ceb56d4abe6941d8ec44cca9630033b0b97f5dbe5149b09c091ebc`). **This revised design was approved. The initial animal-muzzled Demi attempt is rejected.**

**MANDATORY:** **Human female face, nose, cheeks, mouth, hair and 90% human anatomy**, but rounded hyena ears, subtle fangs/ever-present recognizable mischievous grin, dark spots/rosettes/stripe patterns on darker **caramel/bronze/umber/ebony** skin, ample fuzzy fur at outer thighs/hips and forearms/wrists/paw-gloves, a prominent hyena tail and tasteful claws on mostly human hands/feet. Adult body variants lithe/toned, balanced, strong/muscular, plump/voluptuous, with six art-role examples, expressions and anatomy panels. Do not generate full hyena snouts or all-over anthro fur for Demi. Darker complexions and patterns are **cosmetic**, not ability/temperament linked.

**Race separation:** Hyenakin and Demi-Hyenakin are **two independent approved visual races** with different facial morphology and fur distribution. No gameplay race IDs, native sprites, animation or traits are verified; remain on art branch and do not disturb `feature/playable-db-embark-integration` (PLAY-01D).

## Approved Vampirekin (Female) — gothic humanlike vampires

**Exact canonical visual reference:** [VAMPIREKIN_BASELINE_V1.md](VAMPIREKIN_BASELINE_V1.md), original unaltered `/Knomira/Sprite Design/Race Baselines v1/Vampirekin/Vampirekin_Female_APPROVED.png` (**1448 × 1086 RGB PNG**, SHA-256 `b349306d99a7568c147d890d782959ce5df7a951ee3e1119bcd215d6e9424945`). User approved this image as an official **art reference** for Knomira's Vampirekin on 2026-11-18.

**Appearance:** Primarily normal human-faced adult women with subtle varying fangs, crimson/wine-red/amber/violet eyes, long black/silver hair, elegant aristocratic expressions, **black gothic corsets**, dark lace, fitted thigh-high legwear, deep burgundy cloaks, gold chains and rose jewelry. Example complexions Pale Ivory, Porcelain, Dusk Rose and Olive Dusk. Preserve the six original visual role costumes, expression tiles, eye/fang, hair and cape variants and anatomical turnarounds. Remain **distinct from Succubuskin and Demonkin**; no default horns, animal ears, large wings, or tails. Natural skin tone variation does not determine abilities or morality.

**Status:** Approved **visual concept only**. Printed height ~1.60–1.78 m is not game-verified. No new gameplay species, mechanics, sprite sheets or animations. ART-01A native atlas audit remains a prerequisite. Do not alter the playable-stabilization branch for race-art registration.
