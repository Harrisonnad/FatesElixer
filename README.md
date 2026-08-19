# Fate's Elixer

A 4-player PvE co-op extraction game built in Unreal Engine 5.8, where alchemy is the progression system — potions aren't just buffs, they're keys that unlock zones, make specific creatures killable, or open new routes. Co-op from day one. Long-term goal: brewing the Elixir of Life.

## Start here

- [`claude/architecture decisions/00-GDD.md`](claude/architecture%20decisions/00-GDD.md) — pillars, scope boundary, non-goals, and the project's locked decisions (engine, perspective, networking backend).
- [`claude/architecture decisions/03-roadmap.md`](claude/architecture%20decisions/03-roadmap.md) — the 8-phase build order (Phase 0–7), with cut lines per phase.
- [`claude/architecture decisions/05-risk-register.md`](claude/architecture%20decisions/05-risk-register.md) — what's most likely to kill this project, and the early warning signs for each.
- [`claude/elixir-team/CLAUDE.md`](claude/elixir-team/CLAUDE.md) — the standing brief for the AI agent team driving development, and how work moves through the pipeline (story → tickets → build → QA gate → design review gate → PM gate).

## Repo layout

```
claude/
  architecture decisions/   design docs — GDD, systems spec, asset map, roadmap, backlog, risk register, narrative bible
  elixir-team/               Claude Code agent team (producer, game-designer, gameplay-programmer,
                              netcode-engineer, technical-artist, level-designer, qa-playtester, narrative-writer)
                              plus docs/WORKFLOW.md, docs/specs/, docs/templates/
fateElixer/                 the Unreal Engine project (C++, third-person)
```

## Key decisions (see the GDD's decisions log for the full record)

- Engine: Unreal Engine 5.8 (upgraded from 5.5.4 to get Epic's official editor MCP bridge), C++ project, third-person perspective.
- Co-op from day one — no single-player-first phase.
- Networking backend: Epic Online Services (EOS), server-authoritative listen-server (peer-hosted, one of the 4 players hosts).
- Phases run strictly sequentially — only one of the 8 roadmap phases is ever open at a time.
- Unreal Editor MCP bridge: Epic's official `ModelContextProtocol` plugin, enabled whenever the editor is running.

## Status

**Phase 0 — Networked vertical slice: complete** (see [`docs/specs/milestone-0-vertical-slice.md`](claude/elixir-team/docs/specs/milestone-0-vertical-slice.md)). Interaction system, `ReagentNode`, `ExtractionPoint`, `IA_Interact` input wiring, and the `L_Phase0_VerticalSlice` gray-box level are all built. Cleared all three gates:

- **QA gate**: 8 C++ automation tests pass (interaction gating, server-authority guards, idempotency). Custom replication logic (reagent gather, extraction-ends-run) additionally proven via a headless multi-process test harness (`Tests/ReplicationHarness/` pattern — see `-ElixirAutoInteractReagent`/`-ElixirAutoInteractExtraction` command-line switches on `AfateElixerCharacter`) running a real listen-server host plus two client processes: both interactions, triggered by a non-host client, replicated to all three processes within ~12ms — proving host/client parity on a live network connection, not just in-process logic. Not yet tested: exactly 4 concurrent players, and visual movement sync (built-in engine replication, lower risk).
- **Design review gate**: passed — gray-box only, no art/content scope creep, proves plumbing as intended.
- **PM gate**: passed — on roadmap, no cut-line violations. EOS session/lobby (real Epic Developer Portal credentials) is logged as a deferred fast-follow ticket rather than a Phase 0 blocker, since the milestone's actual acceptance criteria (correctly-replicated shared state, host/client parity) are about the custom gameplay code, not the transport/matchmaking layer.

**Phase 1 — Combat + one creature: complete** (see [`docs/specs/milestone-1-combat-and-creature.md`](claude/elixir-team/docs/specs/milestone-1-combat-and-creature.md)). Player melee attack, HP/downed/revive/auto-respawn, and one melee creature (`AElixirCreature` + Behavior Tree AI: sphere-overlap aggro, chase, attack) are all built. Cleared all three gates:

- **QA gate**: 26 C++ automation tests pass (9 player combat, 8 creature, 8 Phase 0, plus 1 carried-over). Custom replication (downed state, revive, creature death/respawn, solo auto-respawn) proven via the same headless multi-process harness pattern as Phase 0 — listen-server + 2 clients, covering revive triggered by a non-host client (host/client parity). Creature detect/chase/attack (criterion 2) was verified separately via manual PIE testing rather than the harness, after the harness proved unreliable for this specific check; two real bugs were found and fixed this way — a Behavior Tree decorator/service deadlock, and both the Move To and Melee Attack task nodes being bound to the wrong Blackboard key (`SelfActor` instead of `TargetActor`, which had the creature attacking itself). Host/client parity for the fixed AI is accepted on the fix's nature (Blackboard target selection, not networking) rather than a fresh multiplayer retest — a producer/solo-dev judgment call, not a `netcode-engineer` sign-off. Known gap carried forward: the player's `ServerMeleeAttack` (range/cooldown/overlap) has no direct unit test, same class of gap as Phase 0's `InteractionComponent` — worth closing before Phase 3.
- **Design review gate**: passed — matches the story's actual intent (combat/downed/revive loop proven under replication for up to 4 players), still gray-box, no art/content scope creep. The downed/revive system was built in full; the spec's cut line (falling back to respawn-at-extraction) was never needed.
- **PM gate**: passed — on roadmap, no cut-line violations, no scope creep into Phase 2. Diagnostic `UE_LOG` statements added to the AI classes during debugging were left in place (cheap, useful for future creature-AI work in Phase 5).

Phase 2 (Alchemy hub + brewing) is next, per the strictly-sequential roadmap.
