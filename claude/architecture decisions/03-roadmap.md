# Roadmap

Ordering principle: **networking first**. Retrofitting replication onto single-player systems is where solo extraction projects most often die. Build the multiplayer skeleton before the content that has to run on it.

## Phase 0 — Networked vertical slice (4-6 wks)
Gray-box. No art. This is the make-or-break phase.
- 4 players connect to a session, move around a box level, see each other
- One gatherable reagent that replicates correctly
- One extraction point that ends the run for everyone
- **Goal:** prove you can build and debug replicated gameplay at all
- **Cut line:** none — if this phase defeats you, pivot to single-player or a smaller concept *now*, before art and content are sunk. That is a legitimate and good outcome of Phase 0, not a failure.

## Phase 1 — Combat + one creature (3-4 wks)
- Player attack, damage, death, downed state
- One melee creature with basic AI (chase, attack)
- Revive mechanic
- **Cut line:** revive can become respawn-at-extraction if the downed system fights you

## Phase 2 — Alchemy hub + brewing (3-4 wks)
- Hub scene, brewing UI, 3 potions
- Skill tree with ~9-12 nodes across three branches
- Persistence for tree + recipes
- **Cut line:** flatten to a single-branch tree; branch structure is polish, recipe unlocks are the substance

## Phase 3 — Potion gating (2-3 wks)
- Implement at least 2 real gates (one zone gate, one creature gate)
- Add the potion-gated creature type
- **Cut line:** none. This is the hook. If gating slips, the game is a generic co-op looter. Protect this phase over everything except Phase 0.

## Phase 4 — Zone build with real assets (5-7 wks)
- Build the modular tile library first, then assemble 2 zones from it
- Reagent tables per zone
- **Cut line:** ship with 1 zone + a harder "deep" variant of it, rather than 2 distinct zones

## Phase 5 — Extraction pressure + remaining creatures (3-4 wks)
- Threat escalation system
- Creatures 3 and 4
- Perishable reagents
- **Cut line:** creature 4 (the elite/boss) cuts first; perishables cut second

## Phase 6 — Elixir endgame (2-3 wks)
- Elixir-tier reagents, final recipe, completion state
- **Cut line:** the Elixir can be a recipe completion + ending screen rather than a bespoke final encounter. Do this deliberately — a weak final boss is worse than an earned crafting climax.

## Phase 7 — Polish + netcode hardening (5-8 wks)
- Lobby/matchmaking UX, disconnect handling, desync fixes
- Onboarding for the gating system
- **Cut line:** protect disconnect handling and onboarding above cosmetic polish. Budget generously here — netcode bugs surface late and take longer than expected.

## Total estimate: ~27-39 weeks
Wider and longer than the farming sim, driven almost entirely by networking and the creature/animation gap. Treat the top of that range as the realistic number, not the bottom.

## Must-ship core (never cut)
4-player networked session → gather reagents in a zone → at least two real potion gates → extract → spend progress in an alchemy tree that unlocks new access.
