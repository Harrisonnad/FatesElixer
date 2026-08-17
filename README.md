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

Phase 0 — Networked vertical slice (see [`docs/specs/milestone-0-vertical-slice.md`](claude/elixir-team/docs/specs/milestone-0-vertical-slice.md)). Interaction system, reagent node, and extraction point implemented and compiling against UE 5.8; EOS session/lobby and the gray-box test level are still outstanding.
