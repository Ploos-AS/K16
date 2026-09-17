# K16 CPU Platform — M2

## Decision status

**W65C816S remains the reference physical CPU for the first prototype, but is not yet irrevocably frozen for K16 Rev.1.**

K16's primary product requirement is to be the ultimate purpose-built demo machine. Maker friendliness is second; cost is third. Any CPU choice must be judged against that order.

## Why prototype the W65C816S

The part gives K16 a maker-friendly physical CPU, a 24-bit address space, deterministic external bus behavior, bus-control features suitable for DMA, and an assembly-oriented programming model.

It is especially attractive for a hand-built prototype because the CPU can remain a visible, socketed component rather than disappearing into the FPGA.

## Demo-machine concern

The external data bus is 8 bits. Native 16-bit operands therefore require multiple bus transfers. That can become a material bottleneck for a machine intended to feed a powerful Blitter, Copper, audio engine and video system.

K16 therefore treats M2 as a measurement gate:

- prototype the W65C816S cleanly;
- build timing/bandwidth models;
- keep CPU-independent VPU interfaces;
- do not weaken the VPU to accommodate the CPU;
- replace the CPU before Rev.1 if it limits the demo-machine goal.

## CPU alternatives

A historical 68000-class CPU is architecturally attractive because of its 16-bit external data path and excellent assembly model, but original MC68000-family production availability is unsuitable as the primary maker BOM strategy.

A custom FPGA CPU remains a future option, but putting the main CPU inside the FPGA works against K16's goal of a visible, socketable, understandable physical processor.

Therefore the reference prototype stays W65C816S while the architecture remains deliberately CPU-decoupled.

## Software requirements

The CPU platform must support:

- first-class assembly;
- practical C;
- deterministic interrupts;
- memory-mapped hardware;
- efficient coprocessor command submission;
- fast bulk memory operations;
- profiling in the emulator;
- cycle/bus tracing.

## Gate to freeze

W65C816S may be frozen for Rev.1 only after the prototype demonstrates that CPU-to-VPU command bandwidth and memory behavior do not materially constrain representative K16 demos.
