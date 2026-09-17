# K16 System Bus — M1

## Principles

K16 uses the W65C816S bus as the reference CPU-side bus for revision 1. The board keeps important signals visible and testable rather than hiding the machine behind a bridge MCU.

The bus specification is deliberately simple enough to inspect with an oscilloscope or logic analyser.

## CPU-side signals

The reference interface exposes:

- A0-A15 address
- multiplexed bank/address information needed for the 24-bit address space
- D0-D7 data
- R/W
- PHI2
- RESET
- IRQ
- NMI
- RDY
- BE
- VDA/VPA

Exact glue logic and latching are an M2 electrical-design decision.

## Bus masters

Revision 1 has two logical bus-master classes:

1. CPU
2. VPU DMA

The VPU internally arbitrates video, Copper, Blitter and audio DMA before requesting the external memory bus.

This keeps PCB-level arbitration understandable while allowing the FPGA to run several coprocessors concurrently.

## Arbitration

CPU ownership is the default. VPU DMA requests deterministic transfer windows. The implementation must use documented W65C816S bus-control semantics and must not rely on electrically unsafe contention.

M2 will freeze cycle-level arbitration after bench/prototype validation.

## Debugging

Critical bus signals shall have labelled test points or accessible headers where practical.
