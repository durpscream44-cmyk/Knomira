# Demi-Human Dragonkin (Female) / Dragonkin (Dragonborn) — APPROVED Visual Baseline v1.0

**Approval:** 2026-11-15. The user explicitly said **"Perfect, add that to our baseline for [Demi] Human Dragons"**, referring to the newest **Dragonkin (Dragonborn)** race sheet with an eight-portrait **Draconic Heritage Variants** strip. This sheet, **not** the preceding lightly scaled Demi-Dragonkin candidate or the fully reptilian dragon-headed sheet, is the **canonical art-direction reference** for the user's preferred humanlike-but-substantially-draconic body balance.

**Primary, unchanged approved image:** `/Knomira/Sprite Design/Race Baselines v1/Dragonkin/Demi_Human_Dragonkin_Heritage_APPROVED.png` (1536×1024 RGB PNG, SHA-256 `a718baf2d23d2b188caa8c9df83442d98958feaf87f403889d6ae575dfdad778`). Its printed title reads **"Dragonkin (Dragonborn) – Character & Role Examples"**. No art was modified when registering this file.

**Foundation:** The exact earlier top-right panel `Dragonkin_Dragonborn_TOP_RIGHT_CANONICAL.png` (836×610 PNG, SHA-256 `888095a4f7aaa83ff09481ed2d5e0ddc4923e3baf16c23fdba4e8f7c26f0094b`), which the user previously selected as the desired balance of dragon vs humanoid. Both preserved in the same Library folder and downloadable ZIP.

**Naming:** `Demi-Human Dragonkin`, `Demi-Human Dragons` (voice transcription), and `Dragonkin (Dragonborn)` (printed sheet title) refer to the **same accepted visual design family** at this art stage. Do **not** create a second gameplay species ID by treating these as separate races without an explicit later decision.

## Locked body design
- Mostly humanoid, mature adult female faces/upper torsos with natural expressive noses, mouths, and flowing colored hair; distinctly **draconic**, not just a cat-ear human.
- Large curved/swept/rammed/crowned **dragon horns**, optional pointed ear detail, visible **scale clusters** on shoulders, arms, hips and thighs, robust curled **scaled dragon tails**, clawed hands/feet where exposed.
- Regal, strong, powerful athletic feminine builds, pronounced sculpted abdominal muscles, sturdy arms and legs. Maintain the approved dramatic but tasteful fantasy stylization and gilded equipment.
- Wings **optional** (None / Small / Medium / Large), not automatically present on every figure. Do not generate full reptilian snouts or heavy all-over lizard heads.
- Preserve carefully clustered dark-outlined gold-and-parchment **Knomira ornate retro pixel-art** art style per `RABBITKIN_STYLE_LOCK.md`.

## Eight approved draconic heritage appearances
1. **Flame** — Red/Gold
2. **Frost** — White/Blue
3. **Storm** — Blue/Silver
4. **Earth** — Brown/Bronze
5. **Void** — Black/Purple
6. **Nature** — Green/Gold
7. **Light** — Ivory/Gold
8. **Shadow** — Plum/Dark

These are **visual design families**; colors and the printed chart's trait labels do **not** confirm implemented bonuses, damage elements, gameplay mechanics, or lore.

## Character/reference sheet organization
Six main jobs as **visual role examples**: **Worker**, **Warrior**, **Ranger**, **Mage**, **Tinkerer**, **Elder**. Also preserve facial expression tile grid (Neutral, Proud, Confident, Wink, Roar, Serious, Calm, Amused), horn shapes (Carved, Swept, Rammed, Crowned), scale swatches (Crimson, Sapphire, Emerald, Obsidian, Ivory), optional wing silhouettes and proportionally coherent front/3/4/side/rear turnaround.

**Quality note:** The generated sheet's small text includes a misleading/reversed height annotation (`~1.75–1.20 m`). It is an **artifact, not a valid race size specification**; do not encode those values. Trust the visual character anatomy, not errors in generated captions.

## Retired references
- `DEMI_DRAGONKIN_CANDIDATE_V1.md` and `Demi_Dragonkin_Female_CANDIDATE.png` are an **earlier lightly scaled human candidate, now superseded** as the canonical Demi-Human Dragonkin design.
- `Dragonkin_Full_Female_APPROVED.png` remains **archived alternate reptilian character art**, not the approved primary race.
- The cropped upper-right Dragonborn reference is **historical foundation**; the full heritage-expanded image above is the current **main** visual guide.

## Codex and game-sprite handoff
Only art-direction **baseline approved**. No native sprite sheet, texture coordinate/index, animation pivot, species data, model, or playtest is confirmed. Before production, run ART-01A read-only audit of actual installed sprite assets and renderer code; do not overwrite maps, invent frame coordinates, implement a new system, or touch `feature/playable-db-embark-integration` / PLAY-01D. Keep exact source PNG in the ChatGPT Library and attach it to the sprite-design Codex chat; docs on GitHub are not a substitute for the visual reference.
