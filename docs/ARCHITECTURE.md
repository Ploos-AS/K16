# K16 Architecture

## Design philosophy

K16 is a purpose-built 16-bit demo computer. Its first priority is ultimate demo-machine capability; maker friendliness comes second, architectural elegance/learnability third, and cost fourth. The architecture should expose interesting computer-engineering concepts instead of hiding them behind a modern microcontroller.

The machine is divided into cooperating subsystems:

    CPU
     |
    K16 system bus
     |
    +----------+----------+
    |          |          |
   RAM        VPU        I/O
              |
       +------+------+------+
       |      |      |      |
    Copper Blitter Audio  DMA

## Initial CPU candidate

The first CPU candidate is the WDC W65C816S.

This is a candidate, not yet a frozen architectural dependency. M1 must evaluate electrical interface, bus timing, DMA interaction, interrupt behavior, C compiler suitability, assembly ergonomics, availability, and maker-friendly packaging.

## Custom hardware

K16 should use FPGA logic for the custom hardware rather than requiring a large collection of difficult-to-source custom ICs.

The FPGA-based VPU is expected to contain video timing, palette logic, sprites, scrolling, Copper, Blitter, video DMA, audio, audio DMA, and interrupt sources.

## Revision 1 baseline I/O

K16 Rev.1 includes display output, native PS/2 keyboard, two DE-9 digital joystick ports, a real RS-232 serial port plus TTL debug UART, Ethernet, audio, and the expansion interface as baseline machine features.

These interfaces are specified in [IO.md](IO.md). They must be represented by the emulator and remain directly accessible to demo and game software.

## Maker-first principle

Component selection must prioritize:

1. Hand assembly and rework
2. Understandable schematics
3. Repairability
4. Testability
5. Long-term component availability
6. Reasonable cost

Cost optimization must not make the machine substantially harder to build or understand.

## Emulator-first development

Architectural behavior should be specified so emulator, RTL, and physical hardware can be tested against the same conformance suite.
