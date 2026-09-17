# K16 Design Priorities

Every architectural and implementation decision is evaluated in this order:

1. **Ultimate demo-machine capability**
2. **Maker friendliness**
3. **Architectural elegance and learnability**
4. **Cost**

## Demo-machine capability

K16 exists to make exceptional real-time demos in assembly and C. The architecture should encourage raster effects, high-throughput graphics, hardware-assisted compositing, tracker/sample audio, precise synchronization, and creative abuse of documented hardware behavior.

Historical authenticity is not a goal when it reduces creative capability.

## Maker friendliness

Where two approaches offer comparable demo capability, prefer the one that is easier to hand assemble, inspect, probe, repair and modify.

## No artificial retro limits

K16 may use modern FPGA resources, memory capacities and interfaces when they create useful demo capabilities. The machine should retain deterministic, understandable low-level behavior rather than arbitrary historical limitations.

## Coprocessor-first performance

The CPU orchestrates the machine; it should not be required to brute-force every pixel or sample. Video, Copper, Blitter, audio and DMA are independent engines and must remain powerful even if the initial CPU is modest.

## Benchmark question

For every major feature ask:

> Does this make K16 a better machine for creating demos?

If not, it needs another strong reason to exist.
