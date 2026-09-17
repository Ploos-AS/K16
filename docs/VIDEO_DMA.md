# K16 Video DMA — M3

## Principle

Memory bandwidth is a programmable creative resource.

The VPU arbitrates its own clients before presenting a single external DMA request to the system bus.

## Priority classes

1. display-critical fetch
2. audio deadlines
3. Copper deadlines
4. sprite/object fetch
5. Blitter
6. general DMA

Priorities may be refined, but visible output must remain deterministic.

## Bandwidth accounting

The architecture shall expose counters or debugger telemetry for:

- display fetch usage
- sprite usage
- Copper usage
- Blitter usage
- idle slots
- CPU stalls caused by VPU ownership

This lets demo programmers optimize effects rather than guessing.

## Graceful overload

If a programmed mode exceeds available bandwidth, behavior must be deterministic and documented. The hardware must never resolve overload through nondeterministic frame scheduling.

## Memory independence

Register interfaces are defined independently from the final SRAM/SDRAM choice so the emulator and software model can stabilize before the maker PCB memory implementation is frozen.
