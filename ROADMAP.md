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
- [x] Video timing model
- [x] chunky + planar/tile framebuffer model
- [x] raster-visible palette model
- [x] sprite/object engine architecture
- [x] hardware scrolling architecture
- [x] Copper instruction architecture
- [x] Blitter operation architecture
- [x] deterministic video DMA model
- [ ] freeze register encoding after simulation/bandwidth qualification

## M4 — Audio
- [x] 8-voice baseline architecture
- [x] tracker PCM + wavetable/synth model
- [x] optional per-voice ADSR
- [x] stereo mixer and modulation architecture
- [x] deterministic audio DMA
- [x] audiovisual/Copper synchronization model
- [x] maker-friendly physical output requirements
- [ ] freeze exact sample rates/DAC after RTL and bandwidth qualification

## M5 — Emulator
- [x] deterministic core architecture
- [x] CPU integration boundary
- [x] memory/bus model specification
- [x] video/raster model specification
- [x] Copper model specification
- [x] Blitter model specification
- [x] audio model specification
- [x] demo-debugger architecture
- [x] deterministic trace architecture
- [x] buildable core skeleton + smoke test
- [x] W65C816S execution-core foundation (reset + initial opcode subset)
- [x] executable 24-bit RAM/ROM/MMIO bus foundation
- [ ] complete W65C816S instruction set and interrupt semantics
  - [x] IRQ/NMI/BRK/COP/RTI foundation
  - [x] absolute-long memory access foundation
- [x] executable raster timing + raster IRQ foundation
- [ ] executable DMA arbitration model
- [x] first CPU-driven raster palette effect integration test
- [ ] interactive frontend

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
