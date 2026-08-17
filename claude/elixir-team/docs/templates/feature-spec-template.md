# Feature Spec: <name>

**Author:** game-designer · **Status:** draft / in review / approved
**Milestone:** <e.g. M0 — Vertical Slice>

## Summary
One paragraph: what this feature is and why it matters to the core loop.

## Player-facing behavior
Concrete, numbered description of what happens from the player's point of
view. Use real numbers (seconds, distances, counts) — no "short" or "a bit."

## Data
Any recipe/ingredient/timer values this feature needs, in table form —
this becomes the DataTable/CSV schema `gameplay-programmer` builds against.

## Co-op considerations
- What happens at 1 player (solo) and 2 players (duo) vs. a full 4-player
  squad? Solo/duo completability is a hard requirement, not a nice-to-have.
- What happens if two players try to do the conflicting thing at once?
- Is there a solo-recoverable failure path, or can one mistake block the team?

## Acceptance criteria
Numbered, testable list `qa-playtester` will check against directly.
1. ...
2. ...

## Open questions for other disciplines
- **netcode-engineer:** ...
- **technical-artist:** ...
- **level-designer:** ...

## Implementation notes (filled in by implementers, not the designer)
- **gameplay-programmer:** classes/components created, where they live.
- **netcode-engineer:** replication approach taken.
- **technical-artist:** assets/specs produced, ready-to-run vs. spec-only.
- **level-designer:** layout decisions, ready-to-run vs. spec-only.
