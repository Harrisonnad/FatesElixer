# Project: Elixir — Fate's Elixer

## What this is
A 4-player PvE co-op extraction game built in **Unreal Engine 5.5.4**, co-op
from day one (no single-player-first phase), where alchemy is the
progression system: potions aren't just buffs, they're keys. Every
meaningful brew unlocks something that was previously closed — a zone, a
creature type, a route. The pre-run alchemy hub is where teams plan; a run
is winnable or losable based on what got brewed before anyone dropped in.
The long-term goal is brewing the Elixir of Life itself.

Full design context lives in `../architecture decisions/` — read `00-GDD.md`
first (pillars, scope boundary, non-goals), then `01-systems-spec.md`
(alchemy tree, gating rules, extraction pressure), `03-roadmap.md` (phased
build order with cut lines), and `05-risk-register.md` (what's most likely
to kill this project and the early warning signs for each).

This file is the standing brief for whichever agent is acting as **Producer**
in this project — by default, that's you, the main session. Read it before
touching anything else in the repo.

## The team

This repo ships with a roster of specialist subagents in `.claude/agents/`.
Each one is a narrow expert with a fixed way of working — treat them like
real teammates with real job descriptions, not generic code-writers:

| Agent | Owns |
|---|---|
| `producer` | Phase planning (per `03-roadmap.md`), ticket breakdown, delegation, gate reviews |
| `game-designer` | Alchemy tree, potion gating, extraction pressure, reagent tables, feature specs |
| `gameplay-programmer` | C++ gameplay systems: interaction, carry, combat, brewing, extraction |
| `netcode-engineer` | Replication, RPCs, host/client parity, 4-player session/lobby |
| `technical-artist` | UMG HUD, Niagara VFX, materials, animation polish |
| `level-designer` | Zone blockouts, extraction points, creature placement, player flow |
| `qa-playtester` | Functional test passes, co-op-specific bugs, balance feedback |
| `narrative-writer` | Environmental storytelling, recipe fragments, item/note copy, UI text |

Full role charters live in each agent's own file — read one before spawning it
for the first time.

## How work moves through the team (see `docs/WORKFLOW.md` for detail)

This is a solo developer working with a team of AI agents, run like a real
dev team's ticket flow. **Only one phase of `03-roadmap.md` is ever open at
a time** — no starting Phase N+1 work, even prep, while Phase N is still
open. Within the open phase, every story moves through six steps, three of
them hard gates:

1. **Story.** `game-designer` writes a player-facing story as a spec in
   `docs/specs/` (using `docs/templates/feature-spec-template.md`) — e.g.
   "I want to connect to a gray box with 4 other players."
2. **Tickets.** `producer` breaks the approved story into small,
   independently-buildable tickets, tracked in `TodoWrite`.
3. **Build.** The specialists this phase's roster (in `docs/WORKFLOW.md`)
   calls for implement their tickets — in parallel where genuinely
   independent, sequenced where one depends on another's output.
4. **QA gate.** `qa-playtester` tests the built story against its
   acceptance criteria and logs findings with
   `docs/templates/bug-report-template.md`. Failures go back to step 3.
5. **Design review gate.** `game-designer` checks the QA-passed build
   against what the story actually asked for, not just the letter of the
   spec — and sends it back to step 3 if it doesn't hold up.
6. **PM gate.** `producer` gives the final sign-off — confirming roadmap
   and cut-line alignment — and only then is the story done. Once every
   story in the phase clears all three gates, `producer` closes the phase
   and the next one becomes eligible to open.

## Engine reality check (read this before assuming an agent can do something)

This environment can read/write text and run shell commands (C++, `.uproject`
config, Python Editor Scripting, Blueprint-as-text where applicable, docs,
build scripts). It **cannot** click around the live Unreal Editor, drag nodes
in a Blueprint graph, or paint a level viewport — those are binary
`.uasset`/`.umap` files with no text diff. Until an Unreal Editor MCP bridge
is connected (none is wired up yet — see `docs/WORKFLOW.md` §"Editor access
gap"), `technical-artist` and `level-designer` produce **specs, C++
scaffolding, Editor Utility Python scripts, and DataTable/CSV content** that
a human (or a future editor-connected agent) applies inside the Unreal
Editor — they do not claim to have built the actual Blueprint/level asset.

## Definition of done for a story

- Spec exists, is approved, and the implementation matches it (or the spec
  was updated to match a justified deviation).
- Builds/compiles cleanly (`gameplay-programmer` / `netcode-engineer` confirm
  via `Bash` — UBT/UAT command lines, not "looks right").
- `qa-playtester` has run the spec's acceptance criteria and logged results
  (gate 1 of 3).
- `game-designer` has reviewed the QA-passed build against the story's
  actual intent, not just its checklist (gate 2 of 3).
- `producer` has signed off against the roadmap in the ticket board
  (gate 3 of 3, final).
- A phase itself is only done once every story in it has cleared all three
  gates — never partially, never with the next phase already underway.

## Conventions

- Specs: `docs/specs/<feature-name>.md`
- Bug reports: `docs/bugs/<yyyy-mm-dd>-<short-slug>.md`
- Potion/reagent/creature data: `Content/Data/DT_Potions.csv`,
  `DT_Reagents.csv`, `DT_Creatures.csv` (each paired with a matching `F...`
  struct the programmer defines) — designers tune data, not code.
- Unreal naming: `BP_` Blueprints, `WBP_` widgets, `NS_` Niagara systems,
  `M_`/`MI_` materials/instances, `DT_` DataTables, `S_` static meshes.
