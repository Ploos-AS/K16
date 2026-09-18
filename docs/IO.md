# K16 Revision 1 Baseline I/O

K16 Rev.1 includes the following user-facing I/O as baseline hardware. These are part of the machine architecture, not optional expansion targets.

## Video

- VPU-driven display output.
- Digital video is the primary target; DVI/HDMI-compatible signaling is preferred if it can remain open, deterministic and maker-friendly.
- VGA/RGB is a desirable secondary output when practical.
- The physical encoder/output stage must not constrain raster effects, Copper timing, overscan, palette tricks or other demo capabilities.

## Keyboard

- Native PS/2 keyboard interface.
- Raw make/break scan codes exposed through memory-mapped registers.
- FIFO, status and interrupt support.
- Emulator maps the host keyboard onto the same K16 controller.
- Modern USB keyboards may be supported through an external/simple adapter rather than making USB host complexity mandatory in the core machine.

## Controller ports and mouse

- Two native DE-9 multifunction controller ports.
- Each port supports a digital joystick or quadrature mouse; both ports may be used as joysticks simultaneously.
- Direct memory-mapped direction/button state provides low-latency access for demos and games.
- FPGA logic decodes mouse quadrature signals and maintains hardware X/Y counters plus button state, so software does not need to service every transition.
- Either controller port may host the mouse; software and the emulator use the same register model.
- Atari/Commodore/Amiga-style digital joystick compatibility is the baseline, with Amiga-style mouse compatibility where electrically practical.
- The electrical design must protect FPGA/system logic from unsafe peripheral pin states and power/signalling conflicts.
- Input changes may optionally generate interrupts, but polling must remain simple and deterministic.

## Serial

- Full UART exposed through a real RS-232 level shifter and DE-9 connector.
- Separate TTL UART/debug header on the PCB.
- RX/TX FIFO, status and interrupts.
- Serial is a primary bring-up, monitor and debugging path.

## Ethernet

- Baseline Ethernet interface.
- Prefer a well-documented, readily available controller with a simple host interface over opaque/proprietary integration.
- Packet RX/TX, interrupts and DMA-capable transfer are architectural goals so the W65C816S does not have to copy every byte.
- Intended uses include file transfer, remote debugging, network demos and multiplayer.

## Architectural rules

1. Ultimate demo-machine capability remains the first design priority.
2. I/O is memory mapped and observable by the debugger.
3. Hardware behavior must be reproducible in the emulator.
4. Direct/raw access is always available; an SDK may add higher-level APIs without hiding the hardware.
5. Interfaces should remain understandable, repairable and maker-friendly.
6. Exact controller ICs/connectors are frozen only after prototype and availability qualification.
