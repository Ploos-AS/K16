# K16 Audio Synchronization — M4

## Demo synchronization

K16 treats audiovisual synchronization as hardware functionality.

The audio subsystem exposes deterministic counters/events that can be used by CPU code, Copper lists and the debugger.

## Event sources

The architecture reserves events for:

- voice start
- voice loop
- voice end
- programmable sample position
- beat/tick timer
- frame/raster event

## Copper integration

Copper may trigger approved audio operations at exact raster positions, including:

- voice start
- parameter update
- synchronization event

This enables effects intentionally locked to scanline/frame timing without CPU interrupt jitter.

## Demo sequencer hook

K16 reserves a lightweight event/sequencer facility for a later milestone. It may schedule register writes against frame, raster, audio tick or sample-position time.

The sequencer must remain transparent and inspectable rather than becoming a hidden high-level runtime.
