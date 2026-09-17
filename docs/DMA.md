# K16 DMA Model — M1

DMA is a first-class K16 feature.

## Clients

The initial VPU DMA clients are:

- video fetch
- Copper
- Blitter
- audio
- general-purpose DMA

## Two-level arbitration

The FPGA first arbitrates among its internal clients. The resulting VPU request then arbitrates with the CPU for external memory access.

This avoids exposing many independent bus masters on the maker PCB.

## Priorities

The architectural priority order is:

1. display-critical video fetch
2. audio deadlines
3. Copper/raster deadlines
4. Blitter
5. general-purpose DMA

The exact cycle budget is deferred until video timing and physical RAM are selected.

## Requirements

- deterministic behavior
- no CPU/VPU electrical contention
- observable busy/request state
- emulator-visible DMA activity
- debugger trace of DMA ownership
- documented starvation limits
