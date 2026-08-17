# How this team works

This project is set up as a small agent "studio": one orchestrator
(`producer`) and seven specialists, each defined in `.claude/agents/`. This
doc explains the pipeline, why it's shaped this way, and how to actually
drive it day to day.

## The roster

```
producer            — phase sequencing (03-roadmap.md), delegation, gates, status
  └── game-designer        — alchemy tree, potion gating, extraction pressure, feature specs
  └── gameplay-programmer  — C++ interaction/combat/brewing/extraction systems
  └── netcode-engineer     — replication, RPCs, 4-player session/lobby
  └── technical-artist     — UMG HUD, Niagara VFX, materials
  └── level-designer       — zone blockouts, extraction points, creature placement
  └── qa-playtester        — spec-driven testing, bug reports, balance notes
  └── narrative-writer     — environmental storytelling, item/note copy, UI text
```

## Phases run strictly sequentially — one at a time

This is a solo developer working with a team of AI agents, not a studio
where extra headcount buys real parallelism across phases. **Only one phase
of `03-roadmap.md` is ever open at a time.** No agent starts any work —
prep, prototyping, "getting ahead" — that belongs to Phase N+1 while Phase N
is still open. A phase is not open until `producer` has formally closed the
previous one (every story in it has cleared the full gate chain below).
Build on a finished foundation, not a parallel guess at one. The "tandem"
part of this workflow (see the roster table further down) is about multiple
agents working together *inside* the current phase, never about running
ahead into the next one.

## The pipeline — story → tickets → build → three gates

Every feature in the current phase moves through the same chain, modeled on
a real dev team's ticket flow: a designer writes the story, a PM breaks it
into buildable pieces, a developer builds each piece, a tester checks it,
the designer reviews it against what they actually asked for, and the PM
gives the final sign-off against the roadmap. Nothing skips a step.

```
 ┌────────────┐   ┌───────────────┐   ┌───────────┐   ┌─────────┐   ┌──────────────┐   ┌─────────────┐
 │   Story    │──▶│    Tickets    │──▶│   Build   │──▶│ QA gate │──▶│ Design review│──▶│   PM gate   │
 │game-designer│   │   producer    │   │(fan-out,  │   │qa-      │   │game-designer │   │  producer   │
 │writes spec  │   │breaks spec    │   │ tandem    │   │playtester│  │checks it     │   │roadmap +    │
 │in docs/specs│   │into small,    │   │ within    │   │tests vs. │  │against what  │   │cut-line     │
 │(the story)  │   │achievable     │   │ this phase│   │spec's AC │  │the story     │   │alignment;   │
 │             │   │tickets        │   │ only)     │   │         │   │actually asked│   │only gate     │
 │             │   │(TodoWrite)    │   │           │   │         │   │for            │   │that closes  │
 └────────────┘   └───────────────┘   └───────────┘   └─────────┘   └──────┬───────┘   └──────┬──────┘
                                              ▲                             │                  │
                                              └─────── sent back if ────────┘                  │
                                                        it doesn't meet AC                      │
                                                                                     story done ─┘
                                                                            (phase closes once every
                                                                             story in it is done)
```

**Step 1 — Story.** `game-designer` writes the spec in `docs/specs/` using
`docs/templates/feature-spec-template.md` — framed as a player-facing want
("I want to connect to a gray box with 4 other players"), not an
implementation plan. This is the same spec-writing job `game-designer` has
always had; the framing is just explicit now.

**Step 2 — Tickets.** `producer` breaks the approved story into small,
independently-buildable tickets, tracked in `TodoWrite` — e.g. for the
gray-box story: "stand up an EOS session," "replicate player spawn/movement
for up to 4 clients," "one reagent node that replicates pickup," "one
extraction point that ends the run for everyone." This is a new, explicit
`producer` responsibility, not something implementers infer for themselves.

**Step 3 — Build.** The relevant implementer agent(s) for *this phase only*
(check the roster table below) build their tickets. Independent tickets can
run in the same parallel `Agent` batch; dependent ones get sequenced —
`netcode-engineer` reviewing a spec's shared-state questions at the same
time as `gameplay-programmer` builds against it, not after, per
`05-risk-register.md`'s warning that networking is the discipline most
likely to consume the project if it's looped in late.

**Step 4 — QA gate.** `qa-playtester` tests the built feature against the
story's acceptance criteria line by line and logs findings with
`docs/templates/bug-report-template.md`. Functional defects go straight
back to the relevant implementer — this gate is objective (does it do what
the spec's AC says), not a judgment call on whether it's the *right* thing.

**Step 5 — Design review gate.** Once QA passes it, `game-designer` reviews
the finished feature against the story's original intent and acceptance
criteria — the check QA can't do, since QA verifies against the letter of
the spec and `game-designer` verifies against what they actually meant.
**If it doesn't meet the story, `game-designer` sends it back to Step 3**
with specific notes, the same as a real designer rejecting a build that
technically passes tests but isn't the feature they asked for.

**Step 6 — PM gate.** `producer` is the last gate, full stop. Before a story
is called done, `producer` confirms it actually keeps the phase aligned with
`03-roadmap.md` — the phase's must-ship line, its cut lines, and the GDD's
non-goals. Only once every story in the current phase has cleared all three
gates does `producer` declare the phase done and open the next one.

## Roster by phase — who's active, who's idle

Within the current (and only open) phase, the relevant specialists move
**in tandem** — this table just says who has real work in a given phase.
`producer` uses it to decide who to spawn during Step 3 (Build) — the rule
is: **spawn an agent only when it has real work in the current phase.**
Don't loop an idle specialist in "to keep them posted"; report status to
the user instead, and bring that specialist in explicitly when their phase
opens.

| Phase | Active | Partial (narrow task only) | Idle this phase |
|---|---|---|---|
| **0** — Networked vertical slice | `gameplay-programmer`, `netcode-engineer` | `level-designer` (gray-box room only), `qa-playtester` (gate only, end of phase) | `technical-artist` (explicitly no art pass), `narrative-writer` (nothing to write yet), `game-designer` (spec already locked; returns only for open questions) |
| **1** — Combat + one creature | `gameplay-programmer`, `netcode-engineer`, `game-designer` (combat/creature balance) | `level-designer` (test space), `qa-playtester` (gate) | `technical-artist` (placeholder feedback at most), `narrative-writer` (no fiction needed for a test creature) |
| **2** — Alchemy hub + brewing | `game-designer` (skill tree spec), `gameplay-programmer` (hub/brewing logic), `technical-artist` (hub UI), `netcode-engineer` (persistence) | `level-designer` (hub layout), `narrative-writer` (first recipe fragments — enters the pipeline here) | — |
| **3** — Potion gating (protect this phase above all else) | `game-designer` (the hook itself), `gameplay-programmer` (gate hooks), `netcode-engineer` (server-validated checks, no client bypass), `qa-playtester` (heavy — gate-bypass exploit hunting is top priority) | `narrative-writer` (Sealed fiction/gate justification), `technical-artist` (gate indicator UI), `level-designer` (minor) | — |
| **4** — Zone build with real assets | `level-designer` (heavy — tile library, zone assembly), `technical-artist` (materials/dressing), `narrative-writer` (four-act zone storytelling), `game-designer` (reagent tables) | `gameplay-programmer` (reagent hookups), `netcode-engineer` (zone-instance replication), `qa-playtester` (gate) | — |
| **5** — Extraction pressure + remaining creatures | `game-designer` (threat escalation, creatures 3/4), `gameplay-programmer`, `netcode-engineer` (replicate threat state), `narrative-writer` (Vessels/Drawn fiction), `technical-artist` (threat VFX/HUD), `level-designer` (creature placement) | `qa-playtester` (heavy, gate) | — |
| **6** — Elixir endgame | `game-designer` (completion state, three-way ending), `narrative-writer` (heaviest phase for this role — ending payoff), `technical-artist` (ending screen), `gameplay-programmer` (completion logic) | `netcode-engineer` (minor), `level-designer` (Temple zone), `qa-playtester` (gate) | — |
| **7** — Polish + netcode hardening | `netcode-engineer` (heaviest — disconnect handling, desync, host migration edge cases), `qa-playtester` (heaviest — full regression) | `technical-artist` (polish pass), `gameplay-programmer` (bug fixes), `level-designer` (onboarding flow for gating) | `game-designer`, `narrative-writer` (only if QA surfaces a balance/copy issue) |

`producer` is active every phase (sequencing, gates, status) and isn't
listed per-row. When a phase's "Active" column lists agents whose work is
genuinely independent (e.g. Phase 4's `level-designer` and `technical-artist`
building on the same locked spec), spawn them in the same parallel batch.
When one depends on another's output within a phase (e.g. Phase 2's
`technical-artist` needs `gameplay-programmer`'s hub component interfaces
first), sequence them — tandem means "everyone who has work moves together,"
not "everyone fires at once regardless of dependency."

## Running it

In practice, you drive this by invoking subagents with the `Agent` tool. A
phase kickoff looks like:

1. Tell `producer` the phase goal ("open Phase 0, the networked vertical
   slice"). `producer` confirms no other phase is currently open before
   proceeding.
2. `producer` checks `docs/specs/` — `docs/specs/milestone-0-vertical-slice.md`
   in this repo is a **starter draft story** for exactly Phase 0, written
   so you have something concrete to review/edit rather than starting from
   a blank page. Approve it, edit it, or ask `game-designer` to revise it —
   this is Step 1 (Story) of the pipeline above.
3. `producer` breaks the approved story into tickets (Step 2), then spawns
   the implementer(s) this phase's roster row calls for (Step 3) — e.g. for
   Phase 0, that's `gameplay-programmer` and `netcode-engineer` only;
   `technical-artist` and `narrative-writer` stay idle since the roster
   table says they have nothing to do this phase.
4. `producer` spawns `qa-playtester` against the story's acceptance criteria
   (Step 4). Any failures go back to Step 3.
5. Once QA passes, `producer` loops back to `game-designer` for the design
   review gate (Step 5). If `game-designer` sends it back, it returns to
   Step 3 with their notes.
6. `producer` performs the final PM gate (Step 6) — roadmap/cut-line
   alignment — and only then marks the story done.
7. Once every story in the phase is done, `producer` declares the phase
   closed and reports back to you: what's built, what's tested, and that
   the next phase (and only the next one) is now eligible to open.

You can also invoke any specialist directly for a narrow task (e.g. "have
`game-designer` design the second potion gate") without going through
`producer` — the full six-step gate chain is for phase-level story work,
not every small ask.

## Editor access gap (read before assuming an agent "built" something)

No Unreal Editor MCP/automation bridge is connected in this setup — the
MCP connector registry currently has nothing for Unreal Engine. That means
`technical-artist` and `level-designer` cannot open the live Editor to place
actors, drag Blueprint nodes, or paint a Niagara graph (those are binary
`.uasset`/`.umap` files, not text). What they produce instead:

- Precise specs a human applies in-editor in minutes.
- Editor Utility Python scripts, where Unreal's Python API can create/edit
  assets headlessly (works for a meaningful subset of tasks — DataTables,
  simple actor placement via script, some asset creation — but not
  everything).
- C++ (from `gameplay-programmer`/`netcode-engineer`) and data (CSV/struct)
  changes, which this environment can write and — if the engine/toolchain
  is available via `Bash` — actually compile and verify.

If you later run this same agent team **on your own computer** with the
Unreal Editor open (via the Claude desktop app's device bridge, or a
community Unreal MCP plugin if you set one up), `technical-artist` and
`level-designer` could be upgraded to act directly in the editor instead of
producing specs for you to apply. Nothing in this setup requires that to be
useful today, but it's the natural next step once the vertical slice proves
the pipeline out.

## Conventions recap
- Specs: `docs/specs/<feature-name>.md` (template: `docs/templates/feature-spec-template.md`)
- Bugs: `docs/bugs/<yyyy-mm-dd>-<slug>.md` (template: `docs/templates/bug-report-template.md`)
- Potion/reagent/creature data: `Content/Data/DT_Potions.csv`, `DT_Reagents.csv`, `DT_Creatures.csv`
- Asset naming: `BP_`, `WBP_`, `NS_`, `M_`/`MI_`, `DT_`, `S_`
