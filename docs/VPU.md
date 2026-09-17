# K16 VPU — M3 Architecture

## Mission

The VPU is K16's creative engine: video timing, layers, sprites, Copper, Blitter and DMA operate concurrently so the CPU can orchestrate rather than brute-force the display.

## Architectural blocks

    Raster/timing
        |
    Compositor <--- sprites/objects
      ^   ^
      |   +--- tile/planar layers
      +------- chunky layers

    Copper ----> VPU registers
    Blitter ---> shared memory
    DMA arbiter -> system memory bus

## Design rules

- deterministic behavior beats hidden automation
- every important state is observable
- raster-time register changes are intentional features
- chunky and planar workflows coexist
- coprocessors continue while the CPU executes
- emulator, RTL and hardware share one specification
- VPU capability is not reduced merely to match a modest CPU

## Output

Physical HDMI/DVI/VGA-style output choice is deliberately separated from the logical VPU. A replaceable output module or simple maker-friendly interface is preferred when practical.

## M3 freeze boundary

M3 freezes the logical concepts and programming model direction. Exact register encodings, FPGA resource limits, memory clock and physical video output are frozen only after simulation/bandwidth qualification.
