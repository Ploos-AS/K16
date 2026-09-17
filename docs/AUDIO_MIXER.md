# K16 Audio Mixer — M4

## Stereo mixer

Every voice feeds a deterministic stereo mixer.

Each voice has independent:

- volume
- pan
- mute/enable

The mixer accumulates with sufficient internal precision to avoid accidental wraparound during normal use, then applies a documented output conversion.

## Master stage

The architecture provides:

- master left/right level
- global mute
- clipping/peak status
- optional simple master filtering

## Modulation routing

K16 reserves a small, understandable modulation matrix rather than a general DSP graph.

Potential sources include:

- ADSR
- another voice
- low-frequency oscillator
- audio/raster event counter

Potential destinations include:

- volume
- playback rate
- pan
- wavetable phase/parameter

The initial hardware implementation may expose a subset, but the register architecture shall leave clean expansion room.

## Ring modulation

Pairwise ring/amplitude modulation is an explicit architectural goal because it creates substantial demo-synth capability with relatively simple hardware.

## Filtering

A simple deterministic filter path is reserved for M4 implementation qualification. Filters must have documented fixed-point behavior so emulator and RTL produce matching results.
