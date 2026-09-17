# K16 Memory Map — M1

K16 uses the W65C816S 24-bit logical address space: 16 MiB total.

## Revision 1 logical map

| Range | Size | Purpose |
| --- | ---: | --- |
| 00:0000-00:7FFF | 32 KiB | system/work RAM and convenient bank-0 workspace |
| 00:8000-00:BFFF | 16 KiB | memory-mapped I/O window |
| 00:C000-00:FFFF | 16 KiB | boot/system ROM window and vectors |
| 01:0000-1F:FFFF | 1.94 MiB | base RAM aperture |
| 20:0000-3F:FFFF | 2 MiB | VPU-visible graphics/audio workspace |
| 40:0000-7F:FFFF | 4 MiB | expansion/reserved |
| 80:0000-FF:FFFF | 8 MiB | future expansion/reserved |

This is an architectural map, not a requirement to populate all address ranges with physical memory in the first PCB.

## I/O window

Bank 00, 8000-BFFF is divided into coarse blocks:

| Range | Block |
| --- | --- |
| 8000-83FF | system control, timers, IRQ |
| 8400-87FF | video |
| 8800-8BFF | Copper |
| 8C00-8FFF | Blitter |
| 9000-93FF | audio |
| 9400-97FF | DMA |
| 9800-9BFF | storage |
| 9C00-9FFF | serial/input |
| A000-AFFF | expansion I/O |
| B000-BFFF | reserved |

Register-level layouts are intentionally deferred to subsystem milestones.

## Rules

- Custom hardware is memory mapped.
- The CPU and DMA engines share a documented memory model.
- Reserved ranges remain reserved until formally allocated.
- Emulator and RTL use the same canonical map.
