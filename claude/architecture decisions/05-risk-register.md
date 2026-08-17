# Risk Register

The things most likely to kill this project, with early warning signs. Re-read this at the end of every phase.

## Risk 1 — Networking consumes the project
**Likelihood: high. Impact: fatal.**
Replicated 4-player co-op is the hardest thing in this design.

*Warning signs:* Phase 0 runs past 8 weeks. You're debugging desync more than you're building features. Simple additions require large replication rewrites.

*Mitigation:* Phase 0 exists specifically to surface this early, before art and content are sunk. Use Unreal's built-in replication rather than rolling anything custom. Keep authority server-side and simple.

*Kill/pivot criteria:* If Phase 0 isn't working after ~10 weeks, seriously evaluate pivoting to single-player. The alchemy-gating hook works solo. Deciding this at week 10 is cheap; deciding it at week 30 is not.

## Risk 2 — The creature/character asset gap
**Status: RESOLVED (2026-08-16).** Character/creature assets have been sourced ahead of Phase 1, per the mitigation below.

**Likelihood: certain. Impact: high.** *(as originally assessed, pre-resolution)*
The Hivemind bundle is environments only. Characters, creatures, animation, and VFX were all still to source as of the original GDD pass.

*Warning signs:* You're at Phase 1 still without a player character or creature to test combat against.

*Mitigation:* Resolve this before Phase 1 starts, not during. Budget real money or real time for it. — Done; update `02-asset-map.md` with the specific sourced pack(s)/pipeline when convenient.

## Risk 3 — Content volume for an extraction game
**Likelihood: medium-high. Impact: medium.**
Extraction players burn content fast. Two zones and four creatures is thin.

*Mitigation:* Build the modular tile library (Phase 4) so zones can be recombined cheaply. Design for replayability through *gating variation* — the same zone plays differently depending on which potions you brought — rather than raw map count.

## Risk 4 — Small player base makes co-op unplayable
**Likelihood: medium. Impact: high.**
A 4-player-required game with 30 concurrent players is a dead game.

*Mitigation:* Make the game completable solo/duo from the start (see systems spec). Don't gate content behind full-team requirements.

## Risk 5 — Scope creep via "one more potion"
**Likelihood: high. Impact: medium.**
The alchemy tree is infinitely extensible, which makes it infinitely tempting.

*Mitigation:* The gating rule in the systems spec — if a potion isn't a zone, creature, or route gate, it goes in the backlog. No exceptions in v1.

## Risk 6 — Solo burnout on a 30+ week project
**Likelihood: medium. Impact: fatal.**
This is a materially longer project than the farming sim concept.

*Mitigation:* Phase boundaries are deliberate checkpoints. Something playable and demoable should exist at the end of every phase from Phase 1 onward. If three consecutive phases pass with nothing shareable, that's a warning sign in itself.
