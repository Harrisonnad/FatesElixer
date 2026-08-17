# Systems Spec

## Alchemy skill tree
The central progression pillar. Three suggested branches, each unlocking recipes rather than raw stats:

- **Transmutation** — offensive/damage potions, creature-vulnerability brews
- **Warding** — resistance/protection potions, environmental gates (heat, toxin, pressure)
- **Extraction** — utility: carry capacity, reagent yield, run-timer extensions, revive brews

Rules that keep it a *tree* and not a stat bar:
- Nodes unlock **recipes and capabilities**, never flat percentage buffs alone
- Branches should be too expensive to fully max early, so a 4-player team naturally spreads across them
- At least one Elixir-of-Life prerequisite node sits deep in each branch — forces the team to collectively cover all three

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
