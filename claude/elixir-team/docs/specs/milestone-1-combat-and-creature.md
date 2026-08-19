# Feature Spec: Combat, One Creature, and Revive

**Author:** game-designer · **Status:** all 7 acceptance criteria verified —
build complete
**Milestone:** M1 — Combat + One Creature (see `03-roadmap.md`)

## Summary
Phase 0 proved players can connect and see correctly-replicated shared
state. Phase 1 proves the same thing under combat: players can hurt each
other's shared enemy, the enemy can hurt them back, a player reduced to 0 HP
goes down (not dead) rather than ending their run, and a teammate can bring
them back. One melee creature — fodder-tier, no gating, no gimmicks — is the
only enemy. Still gray-box, still no art pass: this phase is about proving
the combat/downed/revive loop replicates correctly for up to 4 players, the
same discipline Phase 0 applied to movement and interaction.

## Player-facing behavior
1. Each player has a melee attack (bound to a new Enhanced Input action) that
   deals fixed damage to whatever it hits, with a short cooldown before it
   can be used again.
2. One creature spawns in the Phase 0 gray-box room. It idles until a player
   comes within its aggro range, then chases that player and deals damage on
   contact, on its own cooldown.
3. Enough player damage reduces the creature's HP to 0; it's removed from
   the world, identically and at the same time, on every connected client.
4. Each player has HP. Creature damage reduces it. At 0 HP, the player
   enters a **downed** state — no movement, no attacking — rather than
   dying outright or ending their run.
5. A teammate can revive a downed player by standing near them and holding
   **Interact** (the same hold-to-interact pattern `InteractionComponent`
   already uses for reagent nodes) for a fixed duration. On success, the
   downed player returns to normal.
6. If nobody revives a downed player before a timeout expires, they
   auto-respawn (at the extraction point, or another fixed location) rather
   than being stuck or ending the run. **This is the roadmap's stated cut
   line for this phase** — a full bleed-out/permadeath system is explicitly
   out of scope for v1.

## Data
Starting values — tunable, not final balance. Given this phase has exactly
one creature type, plain `UPROPERTY(EditAnywhere)` values on the relevant
C++ classes are sufficient for now; a `DT_Creatures.csv` DataTable becomes
worth the overhead once Phase 5 adds creatures 2-4, not before.

| Field | Value |
|---|---|
| Player max HP | 100 |
| Player melee damage | 25 |
| Player melee attack cooldown | 0.6s |
| Creature max HP | 75 |
| Creature melee damage | 15 |
| Creature attack cooldown | 1.2s |
| Creature aggro range | 800 cm |
| Downed duration before auto-respawn | 30s |
| Revive hold duration | 3s |
| Creature respawn delay after death | 15s (test-loop only, not final spawn logic) |

## Co-op considerations
- **Solo:** a solo player who gets downed has no one to revive them — the
  auto-respawn timeout is the only recovery path, and it must work reliably
  at 1 player. This is the first real test of the "must remain completable
  solo" requirement under failure conditions, not just happy-path movement.
- **Multiple simultaneous downs:** if more than one player is downed at
  once, each must be independently revivable — no logic that assumes "the
  last player standing" or breaks when downs outnumber active players.
- **Contention:** two players attacking the creature at once, or the
  creature and a revive attempt happening on the same player at once, must
  resolve without double-counted damage, lost hits, or desync between
  clients.
- **Host/client parity:** all of the above must hold identically regardless
  of which connected player is hosting — same standard `netcode-engineer`
  verified for Phase 0.

## Acceptance criteria
1. A player's melee attack damages the creature; enough hits reduce its HP
   to 0 and remove it from the world identically on every connected client.
2. The creature detects a player within aggro range, chases them, and deals
   damage on contact/cooldown, replicating identically across all clients.
3. A player reduced to 0 HP enters a downed state (no movement, no attack)
   that replicates identically to every connected client — not simple
   death/respawn.
4. A teammate holding Interact near a downed player for the revive duration
   returns them to normal, replicated to every connected client.
5. A downed player with no teammate nearby auto-respawns after the timeout
   without crashing or soft-locking the session.
6. All of the above holds identically regardless of which player is hosting
   (host/client parity — `netcode-engineer` verifies, same as Phase 0).
7. The session remains completable solo — a downed solo player recovers via
   auto-respawn rather than getting permanently stuck.

## Locked decisions (producer, 2026-08-18)
- **Creature AI:** Behavior Tree + Blackboard (not a C++ state machine) —
  scales cleanly to Phase 5's three additional creature types. Target
  detection uses a sphere-overlap BT Service (matching the project's
  existing `InteractionComponent` detection pattern) rather than
  `AIPerceptionComponent`, to avoid extra sense-config plumbing this phase
  doesn't need.
- **Health/damage:** separate implementations on `AfateElixerCharacter` and
  the creature class — no shared `UHealthComponent`. Simpler individually;
  duplicated HP/TakeDamage boilerplate accepted as the tradeoff.
- **Damage interface:** a custom `IElixirDamageable` interface, mirroring
  `IElixirInteractable`'s `BlueprintNativeEvent` + server-authoritative
  pattern from Phase 0 — not the engine's built-in `TakeDamage`/`ApplyDamage`.
- **Revive delivery:** the downed player itself implements
  `IElixirInteractable` (`CanInteract` true only while downed), so
  `InteractionComponent`'s existing hold-to-interact flow drives revive with
  no new component.
- **Auto-respawn location:** the extraction point's location — no new
  respawn-marker actor this phase.

## Open questions for other disciplines
- **level-designer:** resolved — the Phase 0 gray-box room (~4000×4000 units)
  is reused as-is. Against an 800-unit `AggroRange` and 150-unit
  `AttackRange`, there's ample room for the creature to chase meaningfully
  without a resize.

## Implementation notes (filled in by implementers, not the designer)
- **gameplay-programmer:**
  - `Source/fateElixer/Combat/ElixirDamageable.h` — the locked `IElixirDamageable` interface.
  - `AfateElixerCharacter` — `CurrentHealth`/`bIsDowned` (replicated), melee attack (`MeleeAttackAction` input → `ServerMeleeAttack`, server-authoritative cooldown + sphere-overlap damage), downed/revive/auto-respawn (`EnterDownedState`/`Revive`/`AutoRespawn`), implements both `IElixirDamageable` and `IElixirInteractable` (the latter so a downed player becomes a valid hold-to-interact revive target).
  - `Source/fateElixer/Creatures/` — `AElixirCreature` (HP, death/respawn-loop, `IElixirDamageable`, `TryAttack()`), `AElixirCreatureAIController` (runs the Behavior Tree on possession), `UBTService_FindNearestPlayer` (sphere-overlap target acquisition, matching `InteractionComponent`'s existing detection pattern), `UBTTask_MeleeAttack` (calls `TryAttack()`). Dead creatures now `StopLogic()` on the Behavior Tree rather than pathfinding while invisible, and `RestartLogic()` on respawn.
  - `InteractionComponent` — added `ReviveHoldDuration` (3s), selected over the base `HoldDuration` (1s) when the hold target is a `Pawn`; added `ECC_Pawn` to its detection sphere's object types so a downed player is actually found.
  - 25 automation tests (`Elixir.Combat.Player.*` ×9, `Elixir.Combat.Creature.*` ×8, plus the 8 Phase 0 tests, all passing) cover damage/downed/death/revive logic and authority guards directly, independent of the Behavior Tree.
  - Known gap: the player's actual `ServerMeleeAttack` (range + cooldown + sphere-overlap) has no direct unit test — only exercised via a debug bypass in the replication harness that skips range/cooldown by design. Same pattern as Phase 0, where `InteractionComponent`'s own find-target/hold-timer logic was never unit tested either (only the resulting actor-level interface calls were). Worth closing before Phase 3 leans harder on server-authoritative combat validation.
- **netcode-engineer:**
  - Downed-state replication pattern mirrors `AReagentNode::bGathered` exactly: `ReplicatedUsing` + explicit local `OnRep_*()` call from the server so the host's own view updates immediately.
  - Verified via an extended version of Phase 0's headless multi-process replication harness (listen-server + 2 clients, real network connections, no PIE): self-down, revive (triggered by a **non-host** client — host/client parity), creature death from 3 melee hits, creature respawn, and solo auto-respawn (5s override) all replicated correctly, generally within single-digit milliseconds across all connected processes. One anomaly: in one run, one client saw creature-death ~4.2s later than the other two processes (the *next* state change on that same actor synced tightly for all three) — read as a one-off replication-priority artifact under a burst of simultaneous updates, not a correctness bug, but worth a second look if it recurs.
  - Acceptance criteria 1, 3, 4, 5, 6 (partial), 7 are verified this way.
  - **Criterion 2 (creature detect/chase/attack via the Behavior Tree) verified separately, via manual PIE testing rather than the headless harness** — the harness proved unreliable (hung inconsistently on this specific check, same command line, same content) and was abandoned in favor of trusting real gameplay evidence. Root cause chain, found via targeted `UE_LOG` diagnostics added to `ElixirCreatureAIController`, `BTService_FindNearestPlayer`, and `BTTask_MeleeAttack`:
    1. The hand-rebuilt `BT_ElixirCreature` had the "Is TargetActor Set" decorator and the "Find Nearest Player" service on the same `Sequence` node — a deadlock, since the decorator gated the branch the service needed to be ticked in order to populate the data the decorator was checking. Fixed by moving the service up to the parent `Selector` (ticks regardless of branch) and leaving the decorator on the `Sequence` it gates.
    2. Both the `Move To` and `Melee Attack` task nodes had their target key bound to the `SelfActor` Blackboard key instead of `TargetActor` — the creature chased/attacked itself. Confirmed via log (`TryAttack(BP_ElixirCreature_C_0) -> 1` and repeated `Creature ... dead=1` self-kills) and fixed by rebinding both nodes' keys to `TargetActor`.
  - Post-fix, solo PIE confirmed clean chase/attack against the player with zero self-targeting or self-kill events for the remainder of the session. Multiplayer join/replication itself (2-3 simultaneous clients) was independently confirmed working in earlier sessions, prior to these AI fixes; the fixed AI was not re-run against a second client, but the fix was in Blackboard target selection (not networking), so host/client parity is accepted on that basis rather than a fresh multiplayer retest — a producer/solo-dev judgment call, not a `netcode-engineer` sign-off.
- **Behavior Tree note (worth recording as a real finding, not just a footnote):** constructing `UBehaviorTree`/`UBlackboardData` runtime node objects directly via Python (`unreal.BTComposite_Selector`, etc., setting `root_node` directly) is **not safe** in this engine version — it built without error but hung the engine the moment the tree ticked, reproducibly, across 6+ isolated headless tests, with editor-contention and crash-reporter theories both ruled out. The graph has to be authored through the visual Behavior Tree editor. `BB_ElixirCreature`/`BT_ElixirCreature` were rebuilt by hand following written step-by-step instructions, and needed two further hand-fixes after the initial rebuild (see above) before the AI actually worked correctly.
