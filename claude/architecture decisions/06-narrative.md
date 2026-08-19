# Narrative Bible

## Design constraint first
The GDD lists voiced narrative and cutscenes as non-goals. So this story is built to be delivered **entirely through**:
- Recipe fragments and alchemy tree node text (your progression *is* the plot)
- Environmental staging (a lab mid-experiment, a body arranged deliberately, a door barred from the inside)
- Item and reagent descriptions
- Creature design that reads as evidence

If a story beat requires a cutscene to land, it's the wrong beat. Rewrite it as something the player finds.

---

## Premise

**The Elixir of Life has already been made once. You are standing in the aftermath.**

Two centuries ago an order called **the Concord** — four alchemists, one for each discipline plus a fourth who kept the record — completed the Elixir. It worked. None of them died.

That was the problem.

The Elixir grants continuation, not wholeness. It keeps the body going long past the point where the body still knows what it was. The Concord did not die; they *kept going*, and kept going, and what they became is still down there in the ruins, still moving, still faintly recognizable as having once been someone.

Their work sites are now saturated with **the Ferment** — residual transmutation that never settled. It hangs in the air of the deep places. Prolonged exposure begins to do to you what the Elixir did to them.

You are a new alchemist. You are going down there to reconstruct their work — because the recipe still exists in fragments, scattered across their labs — and to finish it correctly.

Or to finish it at all. That's the question the game leaves open.

---

## Why this premise earns its keep

Every mechanic gets a diegetic reason:

| Mechanic | Story justification |
|---|---|
| **Extraction timer / threat escalation** | The Ferment accumulates in you. Stay too long and you start to change. The pressure isn't a game clock — it's the thing that ruined the Concord, happening to you slowly. |
| **Potion gating on creatures** | The Steeped are made of Concord alchemy. Only alchemy unmakes them. You're not fighting monsters, you're *reversing a transmutation* — which is why the right brew works and brute force doesn't. |
| **Potion gating on zones** | The Concord sealed their own work sites with alchemical wards when they realized what was happening. You're picking their locks with their own techniques. |
| **Skill progress is permanent, reagents are lost** | Knowledge stays in your head. Materials stay where you dropped them. Simple and true. |
| **Reagents are rarer deeper in** | The Ferment concentrates near the original work. The most valuable ingredients are closest to the worst thing that ever happened here. |
| **4-player co-op** | The Concord worked as four. The recipe assumes four hands. |
| **Solo play still possible** | One Concord member tried to finish it alone at the end. You can walk that road too. The game never blocks it — it just makes sure you know whose footsteps you're in. |

That last row is the good one. Solo play stays fully supported mechanically, but carries a different flavor of dread. No content gated, no lecture — just the environmental record of someone who did this alone, found where it goes.

---

## The Steeped

Your creatures. All of them are former people, and the design should never let the player forget it.

- **The Drawn** (basic melee) — the least far gone. Still moving through the motions of a job they had. One is still turning a crank that isn't attached to anything.
- **The Vessels** (ranged/harasser) — bloated with unfinished reaction. They leak. What they leak is a reagent you need, which is a genuinely uncomfortable thing to build a farming loop around, and that discomfort is the point.
- **The Sealed** (potion-gated) — a Concord member who warded *themselves* rather than be released. Immune to everything until you brew the counter-agent that undoes their own ward. The first time a player kills one, the game should feel less like a victory and more like a mercy.
- **The Ninth** (optional elite/boss) — the one who tried to finish it alone.

Design rule: no Steeped should be purely hostile-looking. Every one keeps a legible human detail — a tool still gripped, a ring, a posture. The horror is recognition, not gore.

---

## Act structure, mapped to progression tiers

**Act I — The Surface Ruins** *(starter zone, low tier)*
You find the outbuildings: storerooms, waystations, the mundane infrastructure of a working order. The tone is archaeological, not horrific. The Concord look like heroes here — you're finding their notes, their optimism, their early breakthroughs. First Steeped encounter lands hard precisely because the framing has been warm.

**Act II — The Deep Works** *(mid tier, dungeon)*
Where the real work happened. The notes turn technical, then anxious. Marginalia appears in a fourth hand — the record-keeper — noting things the other three didn't want written down. You unlock zone gates here using techniques the Concord invented to contain their own mistakes.

**Act III — The Sealed Wing** *(high tier, catacombs)*
Where they went when it stopped being reversible. Barred from the inside. This is where the Sealed are. The recipe fragments here are the most valuable and the most corrupted — you can tell they were written by someone who was already changing.

**Act IV — The Temple** *(endgame, Elixir site)*
Not a lab. A place chosen for meaning rather than utility, which tells you they knew this was the last thing they'd do. Final Elixir-tier reagents. The complete recipe assembles here.

---

## The Sequence system and why it counts down

**Decision (2026-08-18):** the alchemy tree's three pathways (see `01-systems-spec.md`) are framed
as a spin on *Lord of the Mysteries*' Sequence potions — each pathway is climbed by finding,
brewing, and drinking sequence potions, ranked **9 (entry) down to 0 (height of power)**.

Counting down instead of up isn't just borrowed flavor — it's the same shape as this game's own
central horror. The Concord didn't fail by dying; they failed by continuing past the point where
continuing still meant staying themselves. Sequence 0 is the same edge, mechanically legible: the
closer a pathway gets to 0, the closer that character's fiction sits to what the Concord became —
still recognizable, still moving, further from where they started. A player chasing Sequence 0
should feel that weight the same way finding a Concord member's remains does. Power and cost rise
together, on the same axis, which is the whole point of the premise.

This doesn't require new content types — recipe fragments already are sequence-potion formulae in
everything but name. The Sequence framing gives the existing discovery loop (find fragments →
reconstruct recipe → brew → drink → unlock) a name and a number, not a new mechanic.

## The Elixir of Fate

The Concord called their completed work **the Elixir of Life** — that name belongs to them, found
in their own notes, and should stay attached to their attempt specifically. What the player
ultimately builds — whether by following the Concord's procedure exactly, altering it, or
assembling the fragments and choosing not to complete it — is **the Elixir of Fate**: not a copy of
their result, but the sum of the choices (the *fate*) that got a specific team there. The distinct
name marks that this playthrough's ending is the player's own reckoning with the same choice the
Concord already made once, not a reenactment. It's also, not coincidentally, the game's own title
resolving into an in-fiction object — the thing this game is ultimately about building.

## The ending question

You have the recipe. You have the reagents. The Concord followed this exact procedure and it worked — that's the horrifying part. It worked.

The endgame choice isn't "good ending / bad ending." It's:

1. **Complete it as written.** It will work. You've seen what working looks like.
2. **Complete it altered** — using what you learned unmaking the Sealed, reversing the flaw. Requires deeper investment across all three tree branches, which is why the tree is designed so no single player can cover it alone.
3. **Don't.** Destroy the fragments. This should be a real, supported ending and not a punishment.

For v1, the roadmap says the Elixir can ship as a recipe-completion + ending state rather than a bespoke encounter. That works fine here — arguably better. The climax of a game about alchemy should be an act of alchemy.

---

## Tone guardrails

- **Melancholy, not gory.** The register is a sealed room and a note that trails off, not viscera.
- **The Concord are not villains.** They were careful, competent people who were wrong. If a player finishes the game hating them, the writing failed.
- **No exposition dumps.** Max ~40 words per found note. The player assembles it; you don't hand it over.
- **The Ferment is never explained scientifically.** It's alchemy. Rules and consequences, no chemistry lecture.
- **Resist naming everything.** A few proper nouns land; twenty become noise. The Concord, the Ferment, the Steeped, the Sealed. That's nearly enough.

---

## What this changes in the other docs

- `01-systems-spec.md` — the three tree branches now map to the Concord's three disciplines; the fourth seat (record-keeper) is the player's role, which is a clean justification for why *you* are the one assembling fragments
- `02-asset-map.md` — zone tiers now have narrative identity: outbuildings → deep works → sealed wing → temple. Mountain Temple as the Elixir site is doing real thematic work, not just difficulty gating
- `03-roadmap.md` — no phase changes. Narrative delivery is text and staging, both of which ride along with Phase 4's zone build rather than needing a phase of their own
