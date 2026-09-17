# K16 Timing Model — M1

K16 is intended for demo coding, so deterministic timing is an architectural feature.

## Clock domains

The design may use separate CPU and VPU/video clock domains. Crossing between them must be explicit and deterministic from software's perspective.

## CPU

The W65C816S reference design targets a conservative maker-friendly clock first. Final revision-1 frequency is selected during M2 after SRAM, glue logic and PCB timing validation.

## Raster time

The VPU exposes current raster line and horizontal position to software and the debugger.

Copper WAIT operations and raster interrupts are defined against this canonical raster clock, not host-emulator wall time.

## Conformance

The emulator must model architecturally visible timing closely enough for demos relying on:

- raster position
- Copper events
- DMA contention
- interrupts
- Blitter completion
- audio deadlines

Cycle-level details that depend on the final memory implementation remain provisional until M2/M3.
