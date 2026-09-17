# K16 Sprite/Object Engine — M3

## Philosophy

Sprites are a creative demo primitive, not merely a compatibility feature.

## Architecture

The VPU provides a hardware object engine with:

- many concurrently active objects
- programmable X/Y position
- palette selection
- priority
- horizontal/vertical flip
- clipping
- per-object enable
- raster-visible updates
- DMA-fetched object data

The implementation should favor a scanline object pipeline rather than imposing an unnecessarily small historical sprite count.

## Limits

Exact object count, width and per-line budget are intentionally expressed as a **bandwidth budget** until FPGA and memory qualification. Limits must be deterministic and queryable by software.

## Demo features

M3 reserves architectural room for:

- sprite multiplexing through Copper
- per-line object changes
- collision/status information
- optional scaling in a later VPU revision

No silent frame dropping or nondeterministic object scheduling is permitted.
