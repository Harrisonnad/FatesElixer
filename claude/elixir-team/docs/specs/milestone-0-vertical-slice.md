# Feature Spec: Vertical Slice — "Four Players, One Box, One Extraction"

**Author:** game-designer (starter draft — treat as a first pass to hand to
`producer` for review, not a locked spec) · **Status:** draft
**Phase:** Phase 0 — Networked Vertical Slice (see `03-roadmap.md`)

## Why this scope
Per `03-roadmap.md` and `05-risk-register.md`, networked 4-player co-op is
the single biggest risk to this project — the discipline most likely to
consume the entire build if it's not proven early. Phase 0 deliberately cuts
everything except the networking skeleton: no art, no combat, no alchemy,
no gating. Gray-box only. The goal is not a fun demo — it's proof that 4
players can connect to a session and see correctly-replicated shared state.
**Cut line: none.** If this phase defeats the team, that's a legitimate
trigger to pivot to single-player or a smaller concept now, before art and
content are sunk — not a failure to hide or push through at all costs.

## Summary
Four players connect to a session (drop-in lobby) and spawn into a single
gray-box box-room level. They can see and move around each other in
real time. One reagent node in the room can be gathered by any player and
replicates correctly to all clients. One extraction point, when a player
interacts with it, ends the run for the entire party. Nothing else exists
yet — no combat, no brewing, no gating, no zones.

## Player-facing behavior
1. Up to 4 players join a session via a drop-in lobby (game remains
   completable/testable at 1-4 players — solo/duo completability is a hard
   requirement for the shipped game, and Phase 0 is where that assumption
   starts getting exercised).
2. Each player spawns into the same gray-box level and can see every other
   connected player move in real time, with no visible lag artifacts under
   normal conditions.
3. One reagent node sits in the level. Any player can walk up and interact
   (hold **Interact** for 1s) to gather it. Once gathered, the node is
   consumed/depleted identically for every client — no duplication, no
   player seeing a "phantom" reagent still present after another player
   took it.
4. One extraction point sits elsewhere in the level. Any player interacting
   with it ends the run for the entire party — all clients see the run end
   at the same time, regardless of who triggered it or who is hosting.

## Data
| Field | Value |
|---|---|
| Max players | 4 (must remain functional at 1-4) |
| Reagent nodes in level | 1 |
| Extraction points in level | 1 |
| Level type | Gray-box box room, no art pass |

This phase intentionally has almost no tunable data — it exists to prove
plumbing, not to be balanced. The reagent-node replication pattern
established here is what `01-systems-spec.md`'s full reagent tables build on
in later phases.

## Co-op considerations
- With 1-4 players: the session and replication model must not assume
  exactly 4 are present — this is the first checkpoint for the risk
  register's "small player base" mitigation (the game must not require a
  full squad).
- Contention: if two players reach for the same reagent node simultaneously,
  exactly one succeeds and the other gets clear no-op feedback — no
  duplication, no desync, no crash.
- Host/client parity: every behavior above must hold identically regardless
  of which connected player is hosting. This is the specific thing
  `netcode-engineer` verifies before this phase is called done.
- Disconnect handling is explicitly **out of scope** for Phase 0 (it's
  budgeted properly in Phase 7) — but if a disconnect crashes the session
  outright during testing, that's worth flagging early rather than waiting.

## Acceptance criteria
1. Four separate clients (or the maximum the test setup allows) can connect
   to one session via the drop-in lobby and see each other move in real
   time.
2. The reagent node can be gathered by exactly one player, and its
   depleted/consumed state is visually and functionally consistent across
   all connected clients within a normal latency window.
3. Interacting with the extraction point ends the run for every connected
   player at effectively the same time, regardless of who triggered it.
4. All of the above holds identically regardless of which player is host
   (host/client parity — `netcode-engineer` verifies).
5. The session remains stable at 1, 2, 3, and 4 connected players — no
   behavior that only works at exactly 4.

## Open questions for other disciplines
- **netcode-engineer:** backend is decided (EOS, via `OnlineSubsystemEOS`
  — see `CLAUDE.md`/`00-GDD.md`). Open item: the authoritative-ownership
  approach for reagent-node contention.
- **technical-artist:** none for this phase — Phase 0 is explicitly gray-box,
  no art/VFX/UI polish pass. Flag now if a minimal placeholder HUD (e.g.
  "player count connected") would materially help testing.
- **level-designer:** confirm gray-box room dimensions are large enough to
  meaningfully test movement/visibility at range, but small enough that this
  phase stays about networking, not exploration.

## Implementation notes (filled in by implementers, not the designer)
- **gameplay-programmer:** _pending_
- **netcode-engineer:** _pending_
- **technical-artist:** _pending_
- **level-designer:** _pending_
