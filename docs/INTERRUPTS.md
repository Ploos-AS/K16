# K16 Interrupt Model — M1

## CPU lines

K16 uses both W65C816S interrupt classes:

- NMI for timing-critical system events
- IRQ for maskable peripheral events

## Reference policy

Vertical blank/raster timing may use NMI when explicitly enabled. Normal VPU, Blitter, audio, DMA, storage, serial, timer and expansion events use IRQ.

An FPGA/system interrupt controller provides:

- pending bits
- enable/mask bits
- acknowledge semantics
- stable source numbering

Software must be able to identify every active source without timing-dependent probing.

## Determinism

Interrupt behavior is part of emulator/RTL conformance. Source assertion, acknowledgement and clearing semantics must be documented before M3/M4 subsystem implementation.

## M5.8 executable source allocation

The emulator foundation reserves stable maskable IRQ bits:

| Bit | Source |
| --- | --- |
| 0 | keyboard |
| 1 | controller/mouse |
| 2 | UART RX |
| 3 | UART TX |
| 4 | Ethernet RX |
| 5 | Ethernet TX |

Bank-0 registers 8000-8005 expose pending, enable and acknowledge masks. This allocation is provisional until the complete interrupt map is frozen, but emulator and RTL must use the same semantics.
