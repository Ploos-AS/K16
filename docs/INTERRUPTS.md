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
