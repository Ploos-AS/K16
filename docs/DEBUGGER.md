# K16 Demo Debugger — M5

## Goal

The debugger should make low-level demo development substantially easier than it was on historical machines.

## CPU view

- registers and flags
- disassembly
- breakpoints/watchpoints
- stack
- memory
- interrupts
- cycle/event position

## Raster view

- current X/Y
- frame counter
- display window
- layer state
- palette state
- raster breakpoints

A raster breakpoint can stop at a selected scanline/pixel/event.

## Copper view

- Copper PC
- current instruction
- WAIT condition
- list memory
- register-write history

## Blitter view

- active operation
- source/destination
- dimensions/stride
- progress
- DMA slots consumed

## Sprite/object view

Show objects active on the current scanline, their priorities and bandwidth cost.

## Audio view

- voice state
- sample/wavetable position
- rate
- volume/pan
- envelope
- underruns
- audio DMA use

## DMA/bus profiler

A timeline shall identify CPU, display, audio, Copper, sprite and Blitter ownership. This is a primary K16 demo-optimization tool, not optional polish.
