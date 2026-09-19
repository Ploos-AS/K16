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
  - [x] decimal-mode ADC/SBC foundation (8/16-bit)
  - [x] establish 256-opcode implementation coverage matrix (89/256 execution cases at M5.18)
  - [x] branch/jump control-flow foundation (M5.19)
  - [x] refresh opcode coverage: 100/256 execution cases after M5.19
  - [x] complete ADC addressing-family foundation (M5.20)
  - [x] refresh opcode coverage: 114/256 execution cases after M5.20
  - [x] complete SBC addressing-family foundation (M5.21)
  - [x] refresh opcode coverage: 128/256 execution cases after M5.21
  - [x] complete CMP addressing-family foundation (M5.22)
  - [x] refresh opcode coverage: 142/256 execution cases after M5.22
  - [x] complete AND addressing-family foundation (M5.23)
  - [x] refresh opcode coverage: 156/256 execution cases after M5.23
  - [x] complete ORA addressing-family foundation (M5.24)
  - [x] refresh opcode coverage: 170/256 execution cases after M5.24
  - [x] complete EOR addressing-family foundation (M5.25)
  - [x] refresh opcode coverage: 184/256 execution cases after M5.25
  - [x] complete ASL/LSR/ROL/ROR accumulator + memory foundation (M5.26)
  - [x] refresh opcode coverage: 204/256 execution cases after M5.26
  - [x] complete INC/DEC accumulator + memory foundation (M5.27)
  - [x] refresh opcode coverage: 214/256 execution cases after M5.27
  - [x] complete BIT/TRB/TSB foundation (M5.28)
  - [x] refresh opcode coverage: 223/256 execution cases after M5.28
  - [x] complete PEA/PEI/PER + TXY/TYX foundation (M5.29)
  - [x] refresh opcode coverage: 228/256 execution cases after M5.29
  - [x] complete remaining LDX/LDY/STX/STY + STZ foundation (M5.30)
  - [x] refresh opcode coverage: 242/256 execution cases after M5.30
  - [x] close remaining memory/compare/control-flow foundation gaps (M5.31)
  - [x] refresh opcode coverage: 254/256 execution cases after M5.31
  - [x] implement MVN/MVP block-move foundation (M5.32)
  - [x] reach 256/256 opcode execution-case coverage (M5.32)
  - [ ] complete opcode implementation coverage
  - [x] M5.33 distinguish WAI wait state from STP stopped state
  - [x] M5.33 add WAI wake / STP reset-only regression coverage
  - [x] M5.34 add REP/SEP/XCE mode-transition conformance regression coverage
  - [x] M5.35 add direct-page/indexed/indirect/stack-relative address-wrap regression coverage
  - [x] M5.36 add long/long-indexed bank-crossing and 24-bit wrap conformance coverage
  - [x] M5.37 add IRQ/NMI/BRK/COP/RTI stack-frame and vector conformance coverage
  - [x] M5.38 add interrupt masking, NMI priority and WAI interaction regression coverage
  - [x] M5.39 add emulation/native stack-wrap, width and PLP transition conformance coverage
  - [x] M5.40 add JSR/JSL/RTS/RTL and JMP/JML bank/return conformance coverage
  - [ ] complete semantic/mode/addressing conformance
  - [ ] complete cycle/timing conformance
- [x] executable raster timing + raster IRQ foundation
- [x] executable deterministic DMA priority arbiter
- [x] executable Copper WAIT/MOVE/JUMP foundation
- [x] first Copper-driven multi-raster palette test
- [x] M5.7 integrate CPU + VPU + Copper + DMA on shared machine clock
- [x] first CPU-driven raster palette effect integration test
- [ ] interactive frontend
- [ ] package a user-facing K16 Emulator for software, OS, game and demo development
- [ ] integrated emulator debugger UI (CPU/registers/memory/disassembly/breakpoints)
- [ ] emulator distribution and reproducible release builds

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
- [ ] MIDI controller model with deterministic 31.25 kbit/s timing
- [ ] MIDI IN receive path + IRQ integration
- [ ] MIDI OUT transmit path + IRQ integration
- [ ] MIDI THRU behavior model
- [ ] host MIDI backend for emulator
- [ ] USB host-controller model for 2x USB-A host ports
- [ ] USB-C device/service-controller model
- [ ] SD-card controller model and boot/storage path
- [ ] eMMC controller model and persistent system-storage path
- [ ] USB mass-storage model and block-device abstraction
- [ ] debugger-visible MIDI state

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
- [ ] MIDI UART/timing RTL and emulator/RTL conformance

## M7.5 — K16 hardware simulator
- [ ] define simulator scope and conformance contract
- [ ] cycle-aware CPU/bus model
- [ ] cycle-aware memory and MMIO transactions
- [ ] deterministic DMA arbitration simulation
- [ ] VPU raster and video-DMA timing simulation
- [ ] Copper timing and bus-interaction simulation
- [ ] Blitter timing and bus-contention simulation
- [ ] audio DMA and audiovisual synchronization simulation
- [ ] interrupt latency and priority simulation
- [ ] MIDI serial timing, IRQ and IN/OUT/THRU simulation
- [ ] USB host/device transaction and interrupt simulation
- [ ] SD-card command/data timing simulation
- [ ] eMMC persistent-storage and error-path simulation
- [ ] storage-controller differential tests against emulator and RTL
- [ ] FPGA/RTL co-simulation and trace comparison
- [ ] emulator/simulator differential conformance tests
- [ ] hardware/simulator trace comparison once physical prototypes exist
- [ ] interactive waveform, bus and device-state inspection
- [ ] educational single-step view of CPU, bus, DMA and custom-chip activity
- [ ] deterministic simulator scenarios suitable for documentation and courses
- [ ] establish simulator as K16 hardware golden-reference model

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
- [ ] USB host power, protection and ESD design
- [ ] USB-C power/data protection and service/debug circuitry
- [ ] SD-card power switching and ESD protection
- [ ] eMMC power, boot and signal-integrity design
- [ ] 5-pin DIN MIDI IN with standards-compliant isolated input
- [ ] 5-pin DIN MIDI OUT with standards-compliant output driver
- [ ] 5-pin DIN MIDI THRU derived from MIDI IN
- [ ] 2x USB-A host ports
- [ ] USB-C device/service port
- [ ] full-size SD-card slot
- [ ] eMMC interface and onboard persistent storage

## M9 — Demo platform
- [ ] demo framework
- [ ] raster effects
- [ ] sprite demos
- [ ] blitter demos
- [ ] audio demos
- [ ] MIDI IN/OUT/THRU and synth-control demo
- [ ] USB keyboard/gamepad and storage demo
- [ ] SD boot and filesystem demo
- [ ] eMMC system-storage and recovery demo
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
- [ ] MIDI programming and electrical-interface guide
- [ ] USB host/device programming guide
- [ ] SD/eMMC storage programming and filesystem guide
- [ ] schematic walkthrough and maker bring-up guide
- [ ] emulator/debugger manual
- [ ] simulator/conformance manual
- [ ] worked demo-programming examples
- [ ] beginner K16 assembly course
- [ ] advanced demo-coding course
- [ ] simulator-based hardware architecture exercises
- [ ] generate versioned PDF manuals from repository documentation

## M10 — K16 revision 1
- [ ] manufacture
- [ ] assembly
- [ ] hardware qualification
- [ ] software qualification
- [ ] public build documentation
