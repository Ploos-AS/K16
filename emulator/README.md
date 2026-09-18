# K16 Emulator

The emulator is the executable reference model for K16.

It is not merely a convenience frontend: architectural behavior is specified so emulator, RTL and physical hardware can run the same conformance cases.

## M5 scope

- W65C816S-compatible CPU integration boundary
- 24-bit memory/bus model
- memory-mapped VPU/audio register model
- raster clock
- Copper interpreter
- Blitter state machine
- audio voice/DMA state
- interrupt controller
- DMA arbitration
- debugger and deterministic trace

## Implementation strategy

The emulator is split into a deterministic **core library** and replaceable frontends.

The core owns emulated time. Host wall-clock timing must never alter architectural results.

A headless conformance frontend is allowed for CI, while an interactive frontend is the normal development environment.

## First executable target

M5.1 should boot a tiny ROM, execute CPU instructions, access RAM/register space and emit a deterministic trace. Video/audio rendering follows from the same state model.
