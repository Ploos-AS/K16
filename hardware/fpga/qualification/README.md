# K16 FPGA qualification (C01.5)

This directory records **candidates, not winners**. `null` means unverified; it must never be interpreted as PASS.

Run `python3 hardware/fpga/qualification/validate.py` from the repository root. The validator checks schema, candidate identity, package presence, I/O headroom, utilization, and explicit timing/runtime evidence. It intentionally returns PENDING until measurements exist. Resource percentages are *post-place-and-route* figures, not spreadsheet predictions.

The 220-pin ledger and 407 MB/s workload are preliminary estimates. Bank legality, differential-pair mapping, clock-capable pins, actual pin count, bandwidth and timing still require FPGA-specific qualification. ECP5 remains an open reference target and is not required to pass the full production SCENE_MAX gate.
