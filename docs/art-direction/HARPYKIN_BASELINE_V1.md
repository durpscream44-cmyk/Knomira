# Harpykin (Female) — Approved Revised Knomira Visual Baseline v1.0

**Approval:** The user's **second/final revised Harpykin generation** is approved as the canonical race design. The initial broader-bodied/upper-back-looking version is **superseded**.
**Exact original artwork:** ChatGPT Library `/Knomira/Sprite Design/Race Baselines v1/Harpykin/Harpykin_Female_Lower_Back_Wings_APPROVED.png`. Source: `knomira_harpykin_variant_compendium.png`, **1448×1086 RGB PNG**, SHA-256 `44a42b56d349cfffeb61968fe6b9c6fd42839fd4779fde92bbb88e6d38e3333b`.
**Reference pack:** `/Knomira/Sprite Design/Race Baselines v1/Harpykin/Knomira_Harpykin_Baseline_v1.zip`.

## Species silhouette and wing anatomy (MANDATORY)
- Adult **female avian humanoids**, bipedal, with normal fully usable humanlike arms PLUS an independent pair of feathered wings.
- **The wings attach at the LOWER BACK close to the hips / lumbar-pelvic region**, NOT at the upper back, shoulder blades, neck, or directly in place of arms. They sweep outward/upward behind the body. This low-back arrangement, as corrected in the user's final revised image, must remain legible in side and rear views.
- Birdlike clawed **talon feet**, feathered tail fans, decorative feathers and distinctive avian expressions.
- Tall mature stylized female proportions (sheet mentions approx. 1.6–1.8m, **concept-only, not engine scale**). Maintain a readable light silhouette consistent with the approved Knomira series and `RABBITKIN_STYLE_LOCK.md`.

## Four approved body-type variants
1. **Falcon (tribal scout):** lithest/narrowest; warm sandy–tawny–umber barred plumage, long tapered wings, tribal beadwork/wraps, bow/quiver and lightweight travel equipment.
2. **American Eagle (heroic guardian):** athletic lean adult frame, broad imposing wings with dark brown/black and white markings; regal silver hair, heraldic shield/sword, navy/gold outfits, strong but not stocky.
3. **Dove (graceful envoy/healer):** slender airy silhouette, softly rounded pearl-cream/blush-lilac feathers, delicate ceremonial flowing robes and jeweled staff.
4. **Owl (plush nocturnal scholar):** intentional **plump, pudgy, more voluptuous** frame, fuller hips/thighs/soft torso, large rounded warm mottled tawny wings, fluffy feather trim, lantern/potion/ritual ornaments.

**Very important proportion rule:** Falcon, Eagle, Dove = **visibly slimmer/lither flight builds than the first generation**, but **do not reduce their approved bust/chest size**. Owl stays deliberately rounder, softer and thicker. Do not make all four equally curvy or equally slender.

## Palette, anatomy panels, and props
- Wing variants: Falcon sleek/tapered; American Eagle broad/powerful; Dove rounded/graceful; Owl large/plush.
- Tail feathers, talon variants (light/standard/heavy), feather patterns with analogous real-bird markings, and facial feather paint.
- Expressions: Neutral, Happy, Laugh, Wink, Surprised, Pout, Thinking, Confident.
- Tribal bead strands, bangles, flight-feather charms, colorful woven wraps, jewel settings, leather travel straps and fantasy equipment.
- Approved art style: ornate chunky/crunchy retro-fantasy **pixel art**, sharp clustered stepped charcoal/plum outlines, small controlled dithered highlights, rich but readable patterned costumes, warm parchment background with dark brown/gold ornate border. Primary colors amber/tan/umber, ivory, crimson and ochre, blush/lilac, cool navy/teal and gold.
- Views: front, 3/4, side, 3/4 back, rear, showing exactly where both wing bones connect **low near the pelvis** and how they fold around limbs.

## Reusable image prompt
> Female Harpykin Knomira fantasy-race art sheet, matching **the approved second/final Harpykin source sheet** and ornate crisp pixel-art style. Humanoid adult women with separate feathered wings that **originate low on the back near the hips** (lumbar-pelvic joint), NOT shoulder-mounted and NOT replacing human arms; bird talon feet, layered feathery tails, expressive anime-fantasy faces. Falcon = lean, tribal sandy feathered scout, sleek tapered wings; American Eagle = lean athletic broad dark-brown/white-winged heroic guardian; Dove = slim airy graceful pearl-lilac winged healer; Owl = intentionally pudgy, fluffy, voluptuous nocturnal scholar with rounded large patterned wings. Preserve feminine chest volume on the three lithe types, but slim their waists/legs/arms compared with owl. Detailed layered leather, woven cloth, golden jewelry, tribal beads, ornaments, feather pattern panels, full anatomy turnaround illustrating **low-back wing roots**, wing/leg/tail options and palette. Gold-framed parchment Knomira compendium, 16-bit-inspired crunchy deliberate pixel clusters, dark stepped outlines, restrained dithering, high readability. No wings from shoulder blades, no wing-arms, no generic angels or uniformly thick characters, no smoothed/vector/3D style.

## Art vs playable game boundary
- **Visual reference only**, not a tiled sprite/atlas, race database entry, animation or implemented flying system.
- The approved source artwork takes precedence over combined baseline overviews if they diverge.
- Before any asset placement, perform **ART-01A read-only atlas audit** of genuine installed game graphics and consumers. Wings may need independent layered sprites with pivots/anchors to avoid clipping and to preserve the hips origin. Verify sprite size/offsets and animation handling at runtime first.
- Do not change `feature/playable-db-embark-integration` or derail `PLAY-01D` stabilization; new species mechanics and flight remain deferred.
