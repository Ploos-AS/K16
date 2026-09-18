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
  - [x] SBC/CMP/CPX/CPY immediate foundation
  - [x] INX/DEX/INY/DEY foundation
  - [x] direct-page LDA/STA foundation
  - [x] absolute LDX/LDY/STX/STY foundation
  - [x] absolute-indexed LDA/STA foundation
  - [x] direct-page indirect LDA/STA foundation
  - [x] indexed-indirect and indirect-indexed LDA/STA foundation
  - [x] direct-page long-indirect LDA/STA foundation
  - [x] direct-page long-indirect indexed-Y LDA/STA foundation
  - [x] stack-relative LDA/STA foundation
  - [x] stack-relative indirect indexed-Y LDA/STA foundation
  - [x] TAX/TAY/TXA/TYA/TSX/TXS transfer foundation
  - [x] PHX/PLX/PHY/PLY stack foundation
  - [x] PHP/PLP and PHB/PLB foundation
  - [x] emulation-mode M/X enforcement foundation
  - [x] index high-byte clearing on 8-bit transition
  - [x] width-aware PHA/PLA foundation
  - [x] CLV/CLD/SED status foundation
  - [x] TCS/TSC/TCD/TDC/XBA transfer foundation
  - [x] PHD/PLD/PHK stack foundation
- [x] executable raster timing + raster IRQ foundation
- [x] executable deterministic DMA priority arbiter
- [x] executable Copper WAIT/MOVE/JUMP foundation
- [x] first Copper-driven multi-raster palette test
- [x] M5.7 integrate CPU + VPU + Copper + DMA on shared machine clock
- [x] first CPU-driven raster palette effect integration test
- [ ] interactive frontend

## M5.8 — Baseline I/O emulation
- [x] Freeze Rev.1 baseline I/O requirements
- [x] host display framebuffer frontend foundation
- [x] PS/2 keyboard controller model foundation
- [x] dual DE-9 joystick + quadrature mouse model foundation
- [x] UART/RS-232 controller model foundation
- [x] Ethernet controller model foundation
- [x] shared I/O interrupt-controller foundation
- [x] Ethernet RX/TX memory DMA foundation
- [x] peripheral event wiring into shared IRQ controller
- [x] cycle-arbitrated Ethernet I/O DMA foundation
- [x] debugger-visible I/O state foundation

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
- [ ] display output circuitry/connectors
- [ ] PS/2 keyboard port
- [ ] 2x DE-9 multifunction joystick/mouse ports
- [ ] RS-232 DE-9 + TTL debug UART
- [ ] Ethernet PHY/controller + connector

## M9 — Demo platform
- [ ] demo framework
- [ ] raster effects
- [ ] sprite demos
- [ ] blitter demos
- [ ] audio demos
- [ ] Copper demos
- [ ] combined demos
- [x] define showcase demo suite
- [ ] Raster Storm showcase
- [ ] Sprite City showcase
- [ ] Blitter Lab showcase
- [ ] Tracker Eight showcase
- [ ] Copper Orchestra showcase
- [ ] K16 Megademo Rev.1 showcase

## M9.5 — Documentation and learning
- [ ] complete hardware reference manual
- [ ] complete programmer's reference
- [ ] assembler and instruction reference
- [ ] VPU/Copper/Blitter/audio programming guides
- [ ] schematic walkthrough and maker bring-up guide
- [ ] emulator/debugger manual
- [ ] worked demo-programming examples
- [ ] beginner K16 assembly course
- [ ] advanced demo-coding course
- [ ] generate versioned PDF manuals from repository documentation

## M10 — K16 revision 1
- [ ] manufacture
- [ ] assembly
- [ ] hardware qualification
- [ ] software qualification
- [ ] public build documentation
