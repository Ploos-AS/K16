# K16 Emulator Architecture — M5

## Purpose

K16 should be enjoyable to demo-code before physical hardware exists.

The emulator is therefore developed before RTL/PCB and acts as the software-visible reference implementation.

## Core model

    frontend
       |
    debugger
       |
    deterministic K16 core
      / |  |  |  \
    CPU bus VPU audio IRQ/DMA

All architectural time advances from emulated clocks/events.

## CPU boundary

The CPU implementation is behind a narrow interface so the W65C816S model can be replaced if the M2 bandwidth gate selects another processor.

The rest of K16 must not depend on emulator-specific CPU internals.

## Memory

The emulator implements the canonical 24-bit memory map and distinguishes:

- RAM
- ROM
- MMIO
- unmapped/reserved regions

All bus-visible accesses can be traced.

## Determinism

Given identical ROM, RAM image, inputs and seed/state, a run must produce identical architectural traces.

Host rendering and audio buffering are outside that guarantee and may not feed nondeterministic state back into the machine.

## CI

A headless runner is explicitly supported for conformance tests, regression tests and trace comparison. This does not replace the interactive emulator.
