# Knomira Race Design Baseline v1.7 — Dragonkin canonical reference corrected

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

## Demi-Dragonkin (Female) — candidate, pending approval
[Candidate specification](DEMI_DRAGONKIN_CANDIDATE_V1.md). **Not an approved baseline yet.** Exact standalone new image `/Knomira/Sprite Design/Race Baselines v1/Dragonkin/Demi_Dragonkin_Female_CANDIDATE.png` (1536×1024 RGB, SHA-256 `7d37f8ee35a2fa8ea6a12282c5bc7e794a703cbe1283640ffc4debde6e76700e`). User requested **mostly human adult female**, muscular abs and regal Dragonkin style, with only claw-like fingers/toes and small localized colorful upper-thigh/hip scales; avoid full snouts, prominent dragon horns, wings, long tails and extensive body scales. **Do not register as approved until user reviews.**

## Dragonkin canonical correction — user's top-right Dragonborn panel
**IMPORTANT supersession:** The user confirmed the **top-right panel labeled Dragonkin (Dragonborn)** from the composite Foxkin/Dragonkin/Demi-Dragonkin illustration is **exactly** the dragon-to-human balance desired for future canonical Dragonkin race sheets. It is **not** "too dragon-like." Earlier assertions to that effect are wrong. This later user confirmation takes precedence over the previous full dragon-headed Dragonkin baseline.

- **New canonical reference:** `/Knomira/Sprite Design/Race Baselines v1/Dragonkin/Dragonkin_Dragonborn_TOP_RIGHT_CANONICAL.png` (original exact crop from user-uploaded image; 836×610; SHA-256 `888095a4f7aaa83ff09481ed2d5e0ddc4923e3baf16c23fdba4e8f7c26f0094b`).
- **Canonical updated spec:** [DRAGONKIN_BASELINE_V1.md](DRAGONKIN_BASELINE_V1.md) **v2.0**, which supersedes the preceding description in this index's earlier "Approved full Dragonkin" section. Use the latest reference and spec where they disagree.
- **Body style:** humanlike faces/hair with prominent horns, substantial draconic scales on shoulders/arms/legs, strong visible abs, powerful female anatomy, thick curled scaled tail, optional wings and high-fantasy armor. More draconic than the separate mostly human Demi-Dragonkin but not a fully reptilian dragon head.
- **Superseded image:** `Dragonkin_Full_Female_APPROVED.png` is now an *archived alternative only*, no longer canonical for standard Dragonkin.
- **Do not confuse:** `DEMI_DRAGONKIN_CANDIDATE_V1.md` is still an unapproved *separate* race design, not the top-right canonical Dragonkin.
- **Scope:** illustration reference only; not engine sprite or gameplay implementation.
