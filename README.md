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

**M4 — Audio architecture (logical model)**

The video and audio coprocessor architecture is now defined at the logical level. Audio adds an 8-voice baseline with tracker-friendly PCM, wavetable/synth capability, ADSR, stereo mixing, modulation, deterministic DMA, and hardware audiovisual synchronization. Exact sample rates, DAC and physical bandwidth limits remain subject to RTL/prototype qualification.

## Non-goals

K16 is not intended to be a commercial PC replacement or a highly integrated modern embedded computer. Its purpose is to make computer architecture, low-level programming, custom hardware, and demo coding tangible.

## License

Hardware design materials — including schematics, PCB layouts, manufacturing files, and HDL/RTL that describes hardware — are licensed under the **CERN Open Hardware Licence Version 2 - Permissive (CERN-OHL-P-2.0)**. See [LICENSE-HARDWARE](LICENSE-HARDWARE).

Software — including firmware, drivers, host tools, emulators, assemblers, compilers, utilities, and other executable code unless explicitly stated otherwise — is licensed under the **MIT License**. See [LICENSE-SOFTWARE](LICENSE-SOFTWARE).

Files that incorporate third-party material remain subject to their respective licences and notices.
