# Systems Spec

## Alchemy skill tree — the Sequence system
**Decision (2026-08-18):** the tree is framed as three **Sequence pathways**, one per branch —
a spin on the Sequence-potion progression from *Lord of the Mysteries*: each pathway is a ladder
you climb by researching, brewing, and drinking increasingly potent sequence potions, counting
*down* from Sequence 9 (entry) toward **Sequence 0** (the height of that pathway's power). Counting
down instead of up is deliberate — see `06-narrative.md`'s framing of what approaching 0 costs.

The three pathways are the same three disciplines already speced, unchanged in role:

- **Transmutation** — offensive/damage potions, creature-vulnerability brews
- **Warding** — resistance/protection potions, environmental gates (heat, toxin, pressure)
- **Extraction** — utility: carry capacity, reagent yield, run-timer extensions, revive brews

**Advancing a sequence** (per pathway): find/reconstruct the recipe (recipe fragments — the
existing narrative discovery loop, now explicitly framed as sequence-potion formulae) → brew it at
the hub → drink it. Drinking permanently unlocks that tier's new recipes and capabilities. This is
pillar 1 (*potions are keys, not consumables*) applied literally: a sequence potion doesn't buff
you, it changes what you're capable of.

**Node budget stays as speced (~9-12 nodes total, Phase 2's roadmap line) — not a literal 9-rung
ladder per pathway.** Each pathway exposes a handful of meaningful sequence numbers with narrative
gaps between them (e.g. Sequence 9 → 6 → 3 → 0, four nodes per pathway), not nine grindy steps.
This keeps the Sequence framing's weight and rarity — these are rare, hard-won thresholds, not a
checklist — while staying inside Phase 2's existing scope. `03-roadmap.md`'s cut line for this
phase ("flatten to a single-branch tree if it slips") still applies unchanged.

Rules that keep it a *tree* and not a stat bar:
- Nodes unlock **recipes and capabilities**, never flat percentage buffs alone
- Pathways should be too expensive to fully max early, so a 4-player team naturally spreads across them
- Reaching Sequence 0 in a single pathway is that pathway's height of power, but it is not enough
  on its own — the Elixir of Fate (see `06-narrative.md`) requires deep investment across all
  three pathways collectively, so no single player or pathway can complete it alone

## Potion gating (the hook)
Three gate types. Every potion in v1 should be at least one of these:

1. **Zone gates** — the zone is inaccessible or lethal without the brew (toxic marsh needs an antitoxin; sealed vault needs a solvent)
2. **Creature gates** — a creature is immune/near-immune until the right brew is applied. Applied to the *weapon*, the *player*, or *thrown at the creature* — pick one mechanic and stay consistent
3. **Route gates** — a shortcut, hidden cache, or faster extraction path opens with the right brew

Design check: if a potion doesn't fit one of these three, it's a stat buff and belongs in the backlog.

## Reagents
- Tiered by zone difficulty: common (any zone) → uncommon (zone-specific) → rare (deep/gated areas) → Elixir-tier (endgame only)
- Reagents are the *only* extraction stake — losing a run means losing carried reagents, not skill progress. Skill tree progress is permanent.
  - **Why:** permanent-loss extraction is brutal for a small player base. Losing reagents stings enough to create tension without driving people off.
- Some reagents are perishable (decay if you don't extract in time) — gives the extraction timer teeth beyond just "you die"

## Extraction pressure
Pick ONE escalation model for v1 and tune it; don't stack all three:
- **Timer** — hard clock, simplest to build and tune
- **Threat escalation** — creature spawn rate/difficulty rises over time
- **Resource drain** — light/torch/air depletes, forcing a decision to push deeper or leave

Recommendation: start with **threat escalation** — it's more interesting than a clock and reuses your existing creature AI rather than needing new systems.

## Creatures
- v1 target: 3-4 types
  - 1 basic melee (fodder, tests core combat)
  - 1 ranged/harasser (forces movement)
  - 1 potion-gated (proves the gating hook)
  - 1 zone-boss or elite (optional, first to cut)
- Reuse via variants: retexture + stat/behavior swap is far cheaper than new models

## Co-op specifics
- Team size 4, but the game must be **completable with fewer** — solo/duo viability protects you against a small player base at launch
- Shared reagent haul vs. individual inventories: **recommend shared team stash on extraction** — reduces loot-drama and simplifies replication
- Downed/revive system: revive brews tie the alchemy tree directly into moment-to-moment co-op

## Persistence
Save per player: skill tree state, known recipes, reagent stock, brewed potion inventory, Elixir component progress.
