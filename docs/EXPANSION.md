# K16 Expansion Bus — M1

K16 shall provide a maker-friendly expansion connector.

## Goals

- large-pitch, readily obtainable connector
- labelled signals
- 5 V/3.3 V strategy documented before hardware release
- buffered/protected signals where appropriate
- easy breadboard/prototyping adapter
- expansion I/O and memory windows reserved in the canonical map

## Logical resources

Expansion devices may use:

- bank 00 A000-AFFF for I/O
- banks 40-7F for memory/apertures
- IRQ
- optional DMA capability in a later revision

## Safety

No expansion card shall be expected to infer voltage compatibility. Pinout, current limits and logic levels must be explicit in the hardware specification.

The physical connector and electrical pinout are deferred to the PCB milestone.
