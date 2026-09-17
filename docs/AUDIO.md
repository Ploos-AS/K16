# K16 Audio Architecture — M4

## Mission

K16 audio is a first-class demo coprocessor. It combines tracker-friendly sample playback with lightweight synthesis so musicians and coders can create rich sound without consuming the CPU.

## Core architecture

Revision 1 targets **8 independent hardware voices** as the baseline programming model. Each voice provides:

- PCM sample playback
- programmable sample start/length
- loop start/length
- fixed-point playback rate
- independent volume
- stereo pan
- per-voice enable/status
- DMA-backed sample fetch
- optional interrupt/event on end or loop

The logical register model reserves room for more physical voices if FPGA and memory bandwidth permit.

## Sample formats

The baseline architecture supports signed 8-bit PCM because it is simple, compact and tracker-friendly.

The architecture also reserves signed 16-bit PCM as a high-quality mode. Its Rev.1 availability is subject to DMA/memory qualification.

## Synthesis

A voice may optionally use generated/wavetable data instead of ordinary PCM memory.

The logical architecture reserves:

- wavetable oscillator mode
- saw
- square/pulse
- triangle
- noise
- programmable pulse width where applicable

Synthesis features must not make basic sample playback complicated.

## Envelopes

Hardware ADSR is available as an optional per-voice modulation source. Software may bypass it for tracker-style direct volume control.

## Demo philosophy

Audio hardware must remain deterministic, directly programmable and intentionally exploitable. There is no opaque audio middleware between software and the voices.
