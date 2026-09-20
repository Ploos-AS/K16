# K16

**A maker-first 16-bit computer for learning, C/assembly programming, and demo coding.**

K16 is a hardware and software computer project inspired by classic machines such as the Amiga, C64, and Atari ST: a capable CPU working alongside dedicated hardware for graphics, audio, DMA, and timing.

> **Maker friendly first, cost second.**

## Goals

- 16-bit CPU architecture with a strong assembly programming model
- First CPU candidate: WDC W65C816S
- Maker-friendly PCB and through-hole construction wherever practical
- Socketed replaceable components where practical
- FPGA-based custom video/audio/DMA hardware
- Copper-style raster sequencer
- Hardware blitter
- Sprites, scrolling, palettes, and raster effects
- Multi-channel hardware audio and sample DMA
- Interrupts and DMA as first-class architectural features
- SD/storage and serial I/O
- Emulator before the first production PCB
- Native assembler, linker, C toolchain, SDK, debugger, and demo examples
- Identical software targets for emulator, FPGA, and physical hardware
- Complete documentation of the architecture and schematics

## Structure

- docs — architecture and design documentation
- hardware — schematics and PCB designs
- fpga — custom hardware RTL
- emulator — K16 emulator
- assembler — assembler and linker tooling
- compiler — C toolchain integration
- sdk — headers, libraries, examples, and build tools
- firmware — low-level firmware
- demos — demo-scene examples
- tests — architectural and hardware verification

## Status

**M5.51 — Emulator CPU conformance audit**

The emulator now has execution cases for all 256 W65C816S opcodes. Current qualification work is moving beyond opcode presence into semantic, native/emulation-mode, width, addressing, interrupt, and timing conformance. M5.33–M5.50 add focused regression coverage across execution modes, addressing boundaries, interrupts, arithmetic, stack/control flow, block moves, and status semantics. M5.51 confirms all 256 opcode switch cases are present and separates remaining semantic and timing qualification into explicit audit gates; full CPU conformance and cycle-accurate timing remain in progress.

## Non-goals

K16 is not intended to be a commercial PC replacement or a highly integrated modern embedded computer. Its purpose is to make computer architecture, low-level programming, custom hardware, and demo coding tangible.

## Manufacturing

For fabrication files, release-package conventions, manufacturer choices, and funding/affiliate disclosure, see [MANUFACTURING.md](MANUFACTURING.md). Released hardware remains vendor-neutral and may be manufactured by any suitable PCB manufacturer. For project-specific PCB ordering options, see [ORDERING.md](ORDERING.md).

## License

Hardware design materials — including schematics, PCB layouts, manufacturing files, and HDL/RTL that describes hardware — are licensed under the **CERN Open Hardware Licence Version 2 - Permissive (CERN-OHL-P-2.0)**. See [LICENSE-HARDWARE](LICENSE-HARDWARE).

Software — including firmware, drivers, host tools, emulators, assemblers, compilers, utilities, and other executable code unless explicitly stated otherwise — is licensed under the **MIT License**. See [LICENSE-SOFTWARE](LICENSE-SOFTWARE).

Files that incorporate third-party material remain subject to their respective licences and notices.
