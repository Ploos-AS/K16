#!/usr/bin/env python3
"""K16 FPGA qualification: missing evidence is PENDING, never PASS."""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent
candidates = json.loads((ROOT / "candidates.json").read_text())
requirements = json.loads((ROOT / "requirements.json").read_text())
assert candidates["schema_version"] == requirements["schema_version"] == 1
limits = requirements["limits"]
failed = False
for c in candidates["candidates"]:
    issues = []
    candidate_failed = False
    if not c.get("device"): issues.append("device missing")
    if not c.get("package"): issues.append("exact package pending")
    io = c.get("usable_io")
    if io is None: issues.append("usable I/O and bank legality pending")
    elif not isinstance(io, int) or io < requirements["minimum_usable_io"]:
        issues.append("usable I/O below minimum")
        failed = True
        candidate_failed = True
    measurements = c.get("measurements")
    if not isinstance(measurements, dict):
        issues.append("routed utilization, timing and runtime evidence pending")
    else:
        for key, maximum in limits.items():
            val = measurements.get(key)
            if val is None: issues.append(f"{key} pending")
            elif not isinstance(val, (int, float)) or val < 0 or val > maximum:
                issues.append(f"{key} exceeds limit or invalid")
                failed = True
        for gate in ("bank_legal", "timing_pass", "scene_max_pass", "shr_pass", "passive_thermal_pass"):
            if measurements.get(gate) is not True:
                issues.append(f"{gate} not demonstrated")
                if measurements.get(gate) is False:
                    failed = True
                    candidate_failed = True
    status = "FAIL" if candidate_failed else "PENDING" if issues else "PASS"
    print(f"{c['id']}: {status} — " + ("; ".join(issues) if issues else "all evidence present"))
raise SystemExit(1 if failed else 0)
