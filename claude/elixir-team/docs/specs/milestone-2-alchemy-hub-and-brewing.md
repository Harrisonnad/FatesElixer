# Feature Spec: Alchemy Hub + Brewing

**Author:** game-designer · **Status:** draft
**Milestone:** M2 — Alchemy Hub + Brewing (see `03-roadmap.md`)

## Summary
Phase 1 proved players can fight and survive together. Phase 2 proves the
other half of the core loop: the pre-run hub where a team plans, brews, and
permanently grows. Players gather in a shared hub space, spend reagents at a
brewing station to craft potions from recipes they already know, and drink
those potions to permanently unlock nodes on their own alchemy skill tree —
the first live proof of pillar 1 (*potions are keys, not consumables*) and
pillar 3 (*co-op through specialization, not fixed classes*). Still
gray-box. This phase does not add new zone content, new creatures, or real
potion gates (zone/creature/route gating is Phase 3) — it proves the
brew → drink → unlock loop and that it persists and replicates correctly.

## Player-facing behavior
1. Players spawn into a new hub space (separate from the Phase 0/1 gray-box
   test zone) that contains a brewing station and each player's alchemy
   tree access point.
2. Each player starts with 3 known recipes — one per pathway
   (Transmutation, Warding, Extraction) — and a starting stock of 10 units
   of a single placeholder reagent (**Ferment Dust**, the same substance
   Phase 4's zones will make scarce and tiered; see `06-narrative.md`).
3. At the brewing station, a player opens a brewing UI listing their known
   recipes. Each of the 3 starter recipes costs 4 units of Ferment Dust and
   takes 3 seconds to brew. On completion, one potion is added to that
   player's inventory and the reagents are consumed.
4. A player can drink a potion they hold. Drinking a pathway's Sequence 9
   potion permanently unlocks that pathway's Sequence 9 node on **that
   player's own** alchemy tree — it does not affect teammates' trees.
5. Each player's alchemy tree UI shows all ~9-12 node slots across the three
   pathways (Transmutation, Warding, Extraction), counting down from
   Sequence 9 to Sequence 0 per `01-systems-spec.md`'s Sequence system.
   Only the Sequence 9 node in each pathway is unlockable this phase — the
   other tiers are visibly present but locked, since their recipes depend on
   reagents/fragments Phase 4's zones haven't shipped yet.
6. Unlocking a Sequence 9 node is permanent and persists across sessions —
   closing and relaunching the game, or leaving and rejoining the hub,
   restores exactly the nodes that player had unlocked.
7. Ferment Dust stock is also persistent per player (not reset each hub
   visit), so a player who brews 2 of 2 affordable potions and logs out
   returns with 2 units left, not a refilled stash.

## Data

### Starter potions/recipes (first-pass numbers — expect tuning after playtest)
| Pathway | Recipe name | Reagent cost | Brew time | Unlocks on drink |
|---|---|---|---|---|
| Transmutation | Ninth Draught of Transmutation | 4× Ferment Dust | 3s | Sequence 9 — Transmutation node |
| Warding | Ninth Draught of Warding | 4× Ferment Dust | 3s | Sequence 9 — Warding node |
| Extraction | Ninth Draught of Extraction | 4× Ferment Dust | 3s | Sequence 9 — Extraction node |

### Starting values
| Field | Value |
|---|---|
| Starting Ferment Dust per player | 10 |
| Recipes known at hub entry | 3 (the Sequence 9 recipe in each pathway) |
| Alchemy tree node slots (total) | 12 — 4 per pathway (Sequence 9 / 6 / 3 / 0) |
| Node slots unlockable in Phase 2 | 3 (Sequence 9 only, one per pathway) |
| Node slots visible but locked | 9 (Sequence 6/3/0 × 3 pathways) |

### Alchemy tree node data (schema for `gameplay-programmer`)
Each node needs: `PathwayID` (Transmutation/Warding/Extraction),
`SequenceTier` (9/6/3/0), `DisplayName`, `RecipeID` (the potion that unlocks
it, or none if not yet implemented), `PrerequisiteNodeID` (null for
Sequence 9 — each pathway's entry point). Sequence 6/3/0 nodes exist as data
rows this phase with no `RecipeID` yet (locked, no path to unlock) —
Phase 4+ fills these in as deeper reagents/fragments ship.

## Co-op considerations
- **Solo:** a solo player has the full loop available — brew, drink, unlock
  — with no dependency on teammates. Nothing in this phase requires a
  second player.
- **Shared hub space, individual progress:** multiple players can be in the
  hub together (base movement/visibility replication already proven in
  Phase 0), but brewing and tree state are entirely per-player. Two players
  brewing at the same station simultaneously must not cross-consume each
  other's reagents or block each other from brewing — this is simpler than
  Phase 0's reagent-node contention because there's no shared resource being
  contested, but `netcode-engineer` should confirm the brewing station
  supports concurrent independent use rather than being a single-user-locked
  interactable.
- **Persistence per player, not per session:** since tree/recipe/reagent
  state must survive a player leaving and rejoining (criteria 6-7), this is
  the first system in the project that needs real save/load, not just
  in-session replication. A player who was mid-team in one session and solo
  in the next must see the same unlocked nodes either way.

## Acceptance criteria
1. A player can open the brewing UI at the hub's brewing station and see
   their 3 known recipes with correct reagent costs.
2. Brewing a recipe the player can afford consumes exactly the listed
   reagent cost, takes the listed brew time, and adds exactly one potion to
   that player's inventory; brewing a recipe the player can't afford is
   blocked with clear feedback and consumes nothing.
3. Drinking a Sequence 9 potion unlocks that pathway's Sequence 9 node on
   the drinking player's tree only — teammates' trees are unaffected.
4. The alchemy tree UI correctly shows all 12 node slots (3 unlockable this
   phase, 9 visibly locked) and accurately reflects each player's own
   unlock state.
5. Two players brewing at the same station at the same time each get
   correct, independent results — no lost potions, no double-consumed
   reagents, no blocking the other player out.
6. A player's unlocked nodes, known recipes, and Ferment Dust count persist
   correctly across a session restart (leave hub, relaunch, return) for both
   a solo player and a player who was previously in a multiplayer session.
7. All of the above holds identically regardless of which connected player
   is hosting (host/client parity — `netcode-engineer` verifies, same as
   Phases 0-1).

## Open questions for other disciplines
- **netcode-engineer:** persistence mechanism — recommend a per-player
  `USaveGame` written locally (mirrors the "defer the heavy backend"
  precedent set in Phase 0, where EOS session/lobby was logged as a
  fast-follow rather than a blocker). Real EOS-backed cloud save/user
  stats can follow later without changing the gameplay-facing save/load
  API, the same way Phase 0 shipped on direct IP connections before EOS
  matchmaking existed. Flag if this assumption is wrong.
- **technical-artist:** hub UI needs a brewing panel (recipe list, cost,
  brew-time progress) and a tree panel (12 node slots across 3 columns,
  locked/unlocked/affordable states). No new VFX required this phase beyond
  a simple brew-complete feedback beat.
- **level-designer:** the hub needs to be a distinct gray-box space from the
  Phase 0/1 test zone — small, since it only needs to hold a brewing station
  and be walkable by up to 4 players at once. Confirm whether this reuses
  the existing test level's `PlayerStart` transition pattern or is a wholly
  separate map players load into first.
- **narrative-writer:** first recipe-fragment-flavored copy enters the
  pipeline here — the 3 starter recipe names/descriptions and the tree
  node tooltips are the first player-facing text since Phase 0/1's
  placeholder labels. Keep to the narrative bible's tone guardrails
  (melancholy, not gory; no exposition dumps).

## Locked decisions (producer, 2026-08-19)
- **Hub is a separate gray-box level**, not a repurposed corner of the
  Phase 0/1 test zone — the core loop is explicitly hub → zone → extract →
  hub, and that only reads correctly if the hub is its own space.
- **Recipes are known from the start this phase**, not gated behind
  recipe-fragment discovery. Fragment discovery is a real, designed
  mechanic (`06-narrative.md`), but it needs zone content to hide fragments
  in, which doesn't ship until Phase 4. Wiring fragment discovery to zones
  that don't exist yet would be building ahead of the phase it belongs to.
- **Reagents are a flat starting grant this phase** (10 Ferment Dust per
  player), not a gathering loop. Real reagent tables per zone are Phase 4's
  job; Phase 2 only needs enough reagent economy to prove brewing works.
- **Tree progress and known recipes are strictly per-player**, never
  shared or pooled — this is the mechanical foundation for pillar 3
  (co-op through specialization), and it must be right from the first
  version, not retrofitted later.

## Implementation notes (filled in by implementers, not the designer)
- **gameplay-programmer:** _pending_
- **netcode-engineer:** _pending_
- **technical-artist:** _pending_
- **level-designer:** _pending_
- **narrative-writer:** _pending_
