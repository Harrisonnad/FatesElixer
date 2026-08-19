# Elixir — Game Design Document

## One-line pitch
A 4-player PvE co-op extraction game where alchemy is the progression system: potions aren't just buffs, they're keys that unlock zones and make certain creatures killable. The long-term goal is brewing the Elixir of Life.

## Pillars (check every feature against these)
1. **Potions are keys, not consumables.** If a potion only makes you slightly stronger, it's the wrong potion. Every meaningful brew should open something that was previously closed — a zone, a creature type, a route.
2. **Prep is half the game.** The pre-run alchemy hub is where teams plan. A run should be winnable or losable based on what you brewed before you dropped in.
3. **Co-op through specialization, not classes.** Players differentiate by what they've invested in the alchemy tree and what they carry — not by locked class roles.

## Core loop
Brew + plan at hub → drop into zone → gather reagents, fight potion-gated creatures → extract before threat escalates → turn in, unlock recipes and skill tiers → access new zones → repeat toward the Elixir.

## SCOPE REALITY — read this before Phase 0
This design is significantly larger than a solo dev typically ships. The honest risks:

- **Networked multiplayer is the single biggest lift.** 4-player replicated co-op in Unreal means replication, authority, lag compensation, session/lobby handling, and desync debugging. This alone can consume more time than every other system combined.
- **PvE creatures need AI + combat + animation.** The Hivemind packs are *environment* assets. They do not include creatures, player characters, or animation sets. That art/animation gap is a real, separate cost.
- **Extraction games live or die on content volume.** Players burn through zones fast. A shippable extraction game needs more content than a farming sim to feel worth the price.

**This is not a reason to abandon the concept.** It's a reason to make two decisions early and explicitly:
1. Decide whether v1 ships **single-player-with-co-op-later** or **co-op from day one**. Building single-player first and retrofitting networking is usually harder than building networked from the start — so if co-op is non-negotiable, commit in Phase 0, not Phase 4.
2. Budget for creature assets separately. Either scope creatures to a small reusable set (3-4 with variants), or plan to buy/commission a creature pack.

**Decisions (2026-08-18):** Alchemy tree reframed as a **Sequence system** (spin on *Lord of the
Mysteries*' Sequence potions) — the three existing branches become three Sequence pathways
(9 → 0, height of power at 0), advanced by finding/brewing/drinking sequence potions. Node budget
is unchanged from Phase 2's roadmap line (~9-12 nodes total); this is a framing/naming decision,
not a scope increase. The endgame artifact is named **the Elixir of Fate** (the Concord's own
completed work keeps the name **Elixir of Life**; what the player builds is a distinct thing,
their own reckoning with the same choice). Full writeup: `01-systems-spec.md`'s alchemy tree
section and `06-narrative.md`'s "The Sequence system" / "The Elixir of Fate" sections.

**Decisions (2026-08-16):** Co-op from day one — confirmed. Character/creature assets — sourced (the Risk 2 asset gap is resolved; see `02-asset-map.md`/`05-risk-register.md`), rigged third-person (full body). Engine: Unreal Engine 5.8 (upgraded 2026-08-17 from 5.5.4 specifically to get Epic's official first-party Unreal MCP editor bridge — see below), project created from the Third Person C++ template (not Blank as originally planned, but equivalent — custom Character/Pawn classes per the systems spec, same perspective outcome). Unreal Editor MCP bridge: Epic's official `ModelContextProtocol` plugin (Experimental, ships with 5.8) is enabled in the project, chosen over community alternatives (chongdashu/unreal-mcp, sam-david/unreal-mcp) for native long-term support — this removes the "no editor access" constraint documented throughout `elixir-team/`'s agent files for `technical-artist`/`level-designer`, once the editor is running with the plugin active. Camera perspective: **third-person** — fits co-op-through-specialization (seeing teammates' gear/carried reagents, downed/revive states) and shows the Steeped creature designs at full silhouette; also the only option the sourced character rig supports. Networking backend: **Epic Online Services (EOS)** — chosen over the Steam Online Subsystem for cross-platform reach and to avoid requiring the Steam client; chosen over a dedicated/custom backend because listen-server (peer-hosted, server-authoritative) is sufficient for a PvE, non-competitive game and a dedicated-server fleet + matchmaking service is unnecessary v1 scope.

## Scope boundary — the "must ship" line
- 4-player networked co-op session (drop-in lobby, one zone type)
- Alchemy hub with brewing + a skill tree (even a shallow one)
- 3-5 potions, at least 2 of which are true gates (zone access, creature vulnerability)
- 2 zones with meaningfully different reagent tables
- 3-4 creature types, at least one potion-gated
- Extraction mechanic with escalating pressure (timer or threat level)
- Persistence: skill tree, recipes, reagent stock

## Non-goals
- PvP of any kind
- Player-vs-player trading/economy
- Open world — zones are discrete instanced maps
- Voiced narrative / cutscenes

## Reference docs in this kit
- `01-systems-spec.md` — alchemy tree, potion gating, extraction pressure, reagents
- `02-asset-map.md` — Hivemind pack → zone mapping, plus the creature/character gap
- `03-roadmap.md` — build order with cut lines, networking-first ordering
- `04-backlog.md` — parked ideas
- `05-risk-register.md` — the things most likely to kill this project, and early warning signs
