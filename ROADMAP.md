# K16 Roadmap

## M0 — Architecture foundation
- [x] Establish repository
- [x] Define maker-first design principle
- [x] Define initial CPU candidate
- [x] Define major hardware blocks
- [x] Establish documentation structure
- [x] Freeze architectural requirements

## M1 — System architecture
- [x] CPU and bus specification
- [x] Address-space model
- [x] Memory map
- [x] Interrupt model
- [x] DMA ownership/arbitration
- [x] Expansion bus
- [x] Timing model

## M2 — CPU platform
- [x] W65C816S evaluation
- [x] CPU support circuitry
- [x] clock/reset design
- [x] SRAM interface
- [x] prototype CPU board architecture
- [ ] physical prototype qualification and bandwidth gate

## M3 — VPU architecture
- [ ] Video timing
- [ ] framebuffer modes
- [ ] palette
- [ ] sprites
- [ ] scrolling
- [ ] Copper
- [ ] Blitter
- [ ] video DMA

## M4 — Audio
- [ ] voice architecture
- [ ] wavetable/sample playback
- [ ] envelopes
- [ ] stereo mixer
- [ ] audio DMA
- [ ] physical audio output

## M5 — Emulator
- [ ] CPU emulation
- [ ] memory/bus model
- [ ] video emulation
- [ ] Copper emulation
- [ ] Blitter emulation
- [ ] audio emulation
- [ ] debugger
- [ ] trace facilities

## M6 — Toolchain
- [ ] assembler
- [ ] linker
- [ ] C compiler integration
- [ ] SDK
- [ ] standard library
- [ ] debugger integration

## M7 — FPGA prototype
- [ ] VPU RTL
- [ ] simulation testbench
- [ ] FPGA target
- [ ] emulator/RTL conformance tests
- [ ] development-board prototype

## M8 — Maker PCB
- [ ] schematic
- [ ] PCB
- [ ] through-hole strategy
- [ ] sockets
- [ ] test points
- [ ] silkscreen documentation
- [ ] bring-up procedure
- [ ] BOM

## M9 — Demo platform
- [ ] demo framework
- [ ] raster effects
- [ ] sprite demos
- [ ] blitter demos
- [ ] audio demos
- [ ] Copper demos
- [ ] combined demos

## M10 — K16 revision 1
- [ ] manufacture
- [ ] assembly
- [ ] hardware qualification
- [ ] software qualification
- [ ] public build documentation
