# W65C816S opcode coverage

This matrix tracks **emulator implementation coverage**, not final conformance.

- `I` = opcode has an execution case in `emulator/src/cpu.c`
- `—` = no execution case yet
- Implemented does **not** imply cycle-exact or fully qualified behavior.
- Addressing, emulation/native-mode edge cases, flags, decimal behavior, interrupts and timing still require conformance qualification.

Current execution cases: **223/256 (87.1%)**.

| | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D | E | F |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 0x | I | I | I | I | I | I | I | I | I | I | I | I | I | I | I | I |
| 1x | I | I | I | I | I | I | I | I | I | I | I | I | I | I | I | I |
| 2x | I | I | I | I | I | I | I | I | I | I | I | I | I | I | I | I |
| 3x | I | I | I | I | I | I | I | I | I | I | I | I | I | I | I | I |
| 4x | I | I | — | I | — | I | I | I | I | I | I | I | I | I | I | I |
| 5x | I | I | I | I | — | I | I | I | I | I | I | I | I | I | I | I |
| 6x | I | I | — | I | — | I | I | I | I | I | I | I | I | I | I | I |
| 7x | I | I | I | I | — | I | I | I | I | I | I | I | I | I | I | I |
| 8x | I | I | I | I | — | I | — | I | I | I | I | I | I | I | I | I |
| 9x | I | I | I | I | — | — | — | I | I | — | I | — | — | I | — | — |
| Ax | I | I | I | I | — | I | — | I | I | I | I | I | I | I | I | I |
| Bx | I | I | I | I | — | — | — | I | I | I | I | — | — | I | — | — |
| Cx | I | I | I | I | — | I | I | I | I | I | I | — | — | I | I | I |
| Dx | I | I | I | I | — | I | I | I | I | I | I | I | I | I | I | I |
| Ex | I | I | I | I | — | I | I | I | I | I | I | I | — | I | I | I |
| Fx | I | I | I | I | — | I | I | I | I | I | I | I | — | I | I | I |

## Qualification levels

Coverage should eventually be tracked at four levels:

1. **Implemented** — opcode dispatch exists.
2. **Semantic tests** — representative register/memory/flag tests pass.
3. **Mode/addressing conformance** — native/emulation, M/X widths, bank/page/direct-page and wrap behavior verified.
4. **Timing conformance** — base cycles and applicable penalties verified against the W65C816S reference.

The M5 CPU milestone is complete only when all 256 documented opcodes reach the required semantic/conformance level, not merely when this table contains 256 `I` entries.
