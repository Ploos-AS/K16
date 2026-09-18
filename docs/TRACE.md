# K16 Deterministic Trace — M5

## Purpose

Trace is the common evidence format for emulator, RTL and eventually hardware qualification.

## Events

The trace model includes:

- CPU instruction boundaries
- memory reads/writes
- MMIO accesses
- interrupts
- raster/frame events
- Copper instructions/register writes
- Blitter start/completion
- audio events
- DMA ownership transitions

## Format

The canonical machine-readable trace format will be line-oriented and versioned. Each event contains an architectural timestamp and stable event type.

Human-readable formatting is a presentation layer and must not define semantics.

## Comparison

Conformance tooling can compare traces at several levels:

1. CPU/memory
2. MMIO
3. raster/Copper
4. DMA
5. full architectural event stream

This allows implementation differences that do not affect the tested layer to be isolated.

## Hardware

Physical hardware cannot necessarily expose every internal event. FPGA debug builds should therefore provide a trace/debug bridge for selected architectural events.
