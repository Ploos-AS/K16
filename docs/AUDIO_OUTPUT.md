# K16 Physical Audio Output — M4

## Maker-first requirement

The physical audio path must be understandable, probeable and repairable.

The logical mixer produces a digital stereo stream. The final conversion may use a dedicated DAC or a simple FPGA-driven conversion stage, selected after evaluating:

1. demo audio quality
2. maker friendliness
3. availability
4. circuit clarity
5. cost

## Required physical features

- stereo line/headphone-capable output strategy
- labelled left/right/ground test points
- documented output level
- replaceable/common analogue components where practical
- no required proprietary codec firmware

## Separation of concerns

The logical audio architecture is independent of the final DAC choice. Emulator, RTL and software can therefore stabilize before the analogue PCB is frozen.
