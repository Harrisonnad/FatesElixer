# Asset Map

## What the Hivemind bundle covers: environments only
The Builder's Vault packs are modular *environment* kits. They give you world geometry and props. They do **not** give you player characters, creatures, animation sets, or combat VFX.

Plan accordingly — see "The gap" at the bottom.

## Zone → pack mapping

| Zone | Difficulty tier | Primary pack |
|---|---|---|
| Overgrown ruins (starter zone) | Low | Stylized Nature + Modular Castle & Dungeon (ruined pieces) |
| The Deep Dungeon | Mid | Dungeon Forge: Modular Dungeon Building Kit |
| Ravenhold catacombs | Mid-High | Ravenhold: Modular Dark Fantasy Megapack |
| Mountain Temple (Elixir site) | Endgame | Mountain Temple: Japanese Shrine / Temple |
| Abandoned village (optional zone) | Low-Mid | Modular Medieval Village / Bandit Village |

**v1 only needs two of these.** Recommend starter ruins + Deep Dungeon: visually distinct, mechanically different (open vs. corridor), and both are already in the bundle.

## Alchemy hub
- Modular Wooden Buildings or VillaForge for the hub structure
- Sundown shop/store kit for brewing station interior dressing
- Hub should be a **small single space**, not a town — it's a menu with geometry, not a location to explore

## The gap — what you still need to source
- [ ] Player character model + animation set (locomotion, attack, gather, downed/revive)
- [ ] 3-4 creature models with animation
- [ ] Combat and potion VFX
- [ ] Brewing station interaction props (may need custom)
- [ ] UI/HUD art

Options for the gap: Unreal marketplace/Fab creature packs, Mixamo for humanoid animation, or a second bundle purchase targeted at characters. Decide this **before** Phase 2 — creature design constrains combat design, not the other way round.

## Modular kit discipline
Extraction games reuse maps heavily. Build a **tile/module library** early (rooms, corridors, junctions at consistent snap dimensions) so zones can be recombined or semi-randomized later without re-authoring geometry. This is the single highest-leverage thing you can do with modular packs.
