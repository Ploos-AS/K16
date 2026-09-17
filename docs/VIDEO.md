# K16 Video Architecture — M3

## Goal

K16 video exists to maximize creative demo capability while retaining deterministic, understandable hardware.

The VPU supports both **chunky** and **planar/tile-oriented** workflows. K16 will not force demo authors into one historical graphics model.

## Reference timing

The initial reference display family is based around 320x200-class low resolution with higher-resolution modes derived from the same timing architecture. Exact output timings and physical video connector remain implementation decisions.

Architectural requirements:

- progressive low-resolution demo mode
- higher-resolution text/UI mode
- programmable display window
- overscan capability where the output timing permits it
- readable raster X/Y counters
- line and frame interrupts
- deterministic frame/raster timing

## Pixel formats

The VPU shall support at minimum:

- indexed chunky pixels
- planar/bitplane graphics
- tile maps
- sprite/object layers

True-colour modes may be added where FPGA/memory bandwidth allows without weakening deterministic demo features.

## Palette

Palette writes are raster-visible and Copper-addressable.

The architecture shall support enough palette precision for gradients, fades and palette cycling without forcing software to precompute reduced-colour tables.

## Scrolling

Hardware scrolling supports:

- pixel-level X/Y scrolling
- independent layer scroll positions
- wraparound
- line/raster changes through Copper

## Double buffering

The display base address is programmable and designed for page flipping. Buffer swaps can be synchronized to frame/raster events.

## Debug visibility

The emulator/debugger must expose:

- current raster position
- active display mode
- layer state
- palette
- sprite state
- video DMA requests
- bandwidth use
