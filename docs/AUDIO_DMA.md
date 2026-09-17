# K16 Audio DMA — M4

## Principle

Audio playback continues independently of the CPU.

Each active PCM/wavetable voice maintains fetch state and obtains data through the VPU DMA arbiter.

## Priority

Audio deadlines rank immediately below display-critical video fetch and above non-critical graphics work. A Blitter job must not cause audible glitches.

## State

For each DMA-backed voice software/debugger can observe:

- current sample address
- remaining length
- loop state
- playback phase
- underrun/error state

## Bandwidth

Audio DMA usage is included in K16 bandwidth telemetry.

The emulator shall make it easy to answer:

- how much memory bandwidth audio consumes
- whether a voice ever underruns
- which DMA client delayed a fetch

## Determinism

When bandwidth is insufficient, underrun behavior is fixed and documented. Audio must not depend on host scheduling or nondeterministic buffering.
