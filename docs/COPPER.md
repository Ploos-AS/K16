# K16 Copper — M3

## Purpose

Copper is a first-class K16 processor dedicated to precise raster-time hardware control.

It executes independently of the CPU and is intended to make raster effects easy, deterministic and composable.

## Core instruction model

The architectural core contains:

- WAIT — wait for raster position and optional masks
- MOVE — write a VPU/custom register
- SKIP — conditionally skip the following instruction
- JUMP — change Copper list position
- IRQ — request a CPU interrupt
- END — stop the list

The encoding will be kept compact and easy to generate from assembly/C tooling.

## Capabilities

Copper may update approved raster-safe registers including:

- palette entries
- display/layer base addresses
- scrolling
- sprite/object state
- display-window parameters
- selected Blitter triggers
- audio synchronization events

Unsafe system-control registers are not Copper-writable.

## Lists

Copper lists reside in DMA-visible memory. Double-buffered lists are supported by a programmable list base and frame-safe switching.

## Timing

WAIT is defined against canonical raster coordinates. Emulator and RTL must agree on when a MOVE becomes visible.

## Debugger

The debugger shall show Copper PC, current instruction, wait condition, list memory, register writes and raster position.
