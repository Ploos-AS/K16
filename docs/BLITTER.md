# K16 Blitter — M3

## Goal

The K16 Blitter is a general-purpose deterministic 2D data engine designed for demos, not just rectangle copying.

## Core operations

- COPY
- AND
- OR
- XOR
- NOT
- masked copy
- fill
- shifts
- transparent-key copy

## Addressing

The Blitter supports independent source and destination pointers, width/height, strides/modulo and direction control.

It must work efficiently with both planar and chunky graphics.

## Demo-oriented features

The architecture reserves support for:

- multiple source channels
- minterm/boolean combination
- line drawing
- pattern/fill data
- barrel shifting
- mask/first-word/last-word handling
- chained command lists

The exact implementation is staged according to FPGA cost and bandwidth, but the register model should not prevent these capabilities.

## Concurrency

Blitter work proceeds independently of the CPU and participates in VPU DMA arbitration. Display and audio deadlines have priority.

## Observability

Busy, completion, error and bandwidth state are software-readable and debugger-visible.
