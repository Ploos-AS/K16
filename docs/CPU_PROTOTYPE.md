# K16 CPU Prototype — M2

## Maker-first implementation

The first CPU prototype is a deliberately inspectable board/subsystem.

### Reference blocks

    clock/reset
        |
    W65C816S (socketed)
        |
    address/bank latch + decode
        |
    +---------+---------+---------+
    |         |         |         |
   SRAM      ROM       VPU       I/O

## CPU

- socketed W65C816S in maker-friendly package
- critical CPU signals available at labelled test points/header
- no hidden MCU required for normal execution

## Clock

Use a socketed oscillator/module or similarly replaceable clock source for bring-up. Start conservatively; validate higher rates rather than making the nominal maximum a board requirement.

Clock routing shall provide a measurement point.

## Reset

Reset must provide a clean power-on reset and a physical reset control. Reset state must be observable at a labelled test point.

## Address/bank handling

The W65C816S multiplexes bank-address information onto the data/bank pins during part of the cycle. The prototype therefore requires an external latch for the bank byte.

Address decoding should use understandable glue logic or a small programmable-logic device only where that materially improves robustness. The schematic must still document the equivalent logical decode.

## SRAM

Use asynchronous SRAM for the first prototype because it provides a transparent random-access interface and straightforward timing analysis.

The exact SRAM part is not frozen until timing, package availability, capacity and maker-friendly sourcing are checked together.

## ROM

Boot ROM shall be socketed/reprogrammable where practical. Development must also allow rapid firmware iteration without repeatedly removing components.

## VPU interface

The CPU board exposes the system bus to the VPU/DMA subsystem. Bus arbitration must prevent electrical contention and expose ownership/debug state.

## Bring-up order

1. power rails
2. reset
3. clock
4. CPU bus activity
5. ROM read
6. SRAM read/write
7. UART/debug output
8. VPU register access
9. DMA arbitration
10. sustained bandwidth tests

## Qualification

The M2 hardware prototype is qualified by observable tests rather than merely booting once. Bus traces, memory tests, interrupt tests and CPU/VPU bandwidth measurements become reusable conformance evidence.
