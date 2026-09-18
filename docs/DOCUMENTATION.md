# K16 Documentation and Learning Plan

Documentation is a product requirement, not release polish. K16 should be understandable from the PCB traces up to a complete demo.

## Reference documentation

The project will maintain source-controlled documentation for:

- hardware architecture, buses, timing, memory map and interrupts;
- complete schematics with functional walkthroughs;
- CPU and K16 assembly programming;
- assembler syntax, directives, linker and executable format;
- VPU, Copper, Blitter, sprites, audio and DMA;
- I/O, expansion, firmware and boot process;
- emulator, debugger, tracing and profiling;
- SDK and C integration;
- physical assembly, bring-up, diagnostics and repair.

Markdown is the canonical source. Versioned PDF manuals should be generated from the repository for releases.

## Assembly learning path

K16 should include a beginner-friendly course that assumes no assembly experience.

1. What a CPU actually does
2. Registers, bytes, words and hexadecimal
3. First K16 program
4. Addressing and memory
5. Loops, branches and subroutines
6. Stack and interrupts
7. 8-bit versus 16-bit 65C816 operation
8. Talking to K16 hardware through MMIO
9. Drawing the first pixels
10. Sprites and scrolling
11. Copper and raster effects
12. Blitter operations
13. Audio and DMA
14. Profiling and cycle thinking
15. Building a first complete demo

Every chapter should have runnable examples for the emulator and, where applicable, identical binaries for physical K16 hardware.

## Advanced material

A second track should cover demo-scene techniques: raster scheduling, Copper list construction, bandwidth budgeting, double buffering, sprite multiplexing, Blitter pipelines, audiovisual synchronization, cycle/bus profiling and deliberate hardware-limit exploitation.

## Documentation rule

When an architectural feature becomes stable, its programmer-visible behavior must be documented alongside its implementation and tests. A feature is not considered release-complete if users cannot discover how to use it.
