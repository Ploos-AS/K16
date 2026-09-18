# K16 Showcase Demos

K16 should ship with polished showcase demos as well as small teaching examples. They are executable specifications, regression workloads and a visible demonstration of why the machine exists.

## Demo set

### 1. Raster Storm
Copper-driven palette gradients, split screens, per-line scrolling and overscan-style border effects. CPU load should remain low enough to make the coprocessor advantage obvious.

### 2. Sprite City
Large numbers of moving objects with priorities, clipping, multiplexing and independent scrolling layers. Includes a debugger view of sprite and DMA pressure.

### 3. Blitter Lab
Fast fills, masked graphics, boolean/minterm effects, line drawing, transitions and chained operations while the CPU performs unrelated work.

### 4. Tracker Eight
Eight-voice PCM/tracker music with stereo panning, live meters and synchronized graphics. Demonstrates deterministic audio DMA.

### 5. Copper Orchestra
A deliberately CPU-light audiovisual demo where Copper events drive palette, scrolling, Blitter triggers and audio synchronization at exact raster positions.

### 6. K16 Megademo
A polished multi-part demo combining chunky and planar graphics, sprites, Copper, Blitter, audio, DMA and transitions. This is the primary Rev.1 showcase and a system-level qualification workload.

## Rules

Each showcase should:
- run unchanged in emulator, FPGA implementation and physical K16;
- include source code and build instructions;
- document which hardware features it demonstrates;
- expose useful debugger/profiler traces;
- have deterministic reference outputs where practical;
- double as a regression/qualification workload;
- avoid fake host-side effects that the physical machine cannot reproduce.

Teaching examples remain intentionally small. Showcase demos should instead aim to be visually and technically impressive.
