#!/usr/bin/env python3
"""Feed SingleStepTests 65816 JSON vectors to the K16 differential runner."""
import argparse
import json
import pathlib
import subprocess
import sys

p = argparse.ArgumentParser()
p.add_argument("json")
p.add_argument("--limit", type=int, default=10)
p.add_argument("--runner", default="emulator/build/test_differential")
p.add_argument("--emit", action="store_true")
p.add_argument("--run", action="store_true")
a = p.parse_args()

data = json.loads(pathlib.Path(a.json).read_text())
if not isinstance(data, list):
    raise SystemExit("expected top-level JSON array")

required = {"pc", "s", "p", "a", "x", "y", "dbr", "d", "pbr", "e"}
keys = ("pc", "s", "p", "a", "x", "y", "dbr", "d", "pbr", "e")

def ram_arg(state):
    return ",".join(f"{addr}={val}" for addr, val in state.get("ram", [])) or "-"

for i, vector in enumerate(data[:a.limit]):
    if not {"name", "initial", "final", "cycles"} <= vector.keys():
        raise SystemExit(f"vector {i}: missing top-level fields")
    for side in ("initial", "final"):
        missing = required - set(vector[side])
        if missing:
            raise SystemExit(f"{vector['name']} {side}: missing {sorted(missing)}")

    values = [str(vector["initial"][k]) for k in keys]
    values += [str(vector["final"][k]) for k in keys]
    cmd = [a.runner, vector["name"], *values,
           ram_arg(vector["initial"]), ram_arg(vector["final"])]

    if a.run and vector["name"].startswith("6d"):
        pc = vector["initial"]["pc"]
        pbr = vector["initial"]["pbr"]
        dbr = vector["initial"]["dbr"]
        ram = dict(vector["initial"].get("ram", []))
        lo = ram.get((pbr << 16) | ((pc + 1) & 0xffff))
        hi = ram.get((pbr << 16) | ((pc + 2) & 0xffff))
        if lo is not None and hi is not None:
            ea = (dbr << 16) | lo | (hi << 8)
            print(f"{vector['name']}: ADC abs operand_addr={ea:06x} operand={ram.get(ea, -1):02x}", file=sys.stderr)

    if a.emit:
        print("\t".join(cmd))
    else:
        print(vector["name"])

    if a.run:
        result = subprocess.run(cmd, check=False)
        if result.returncode:
            raise SystemExit(result.returncode)

print(f"validated {min(a.limit, len(data))} vectors", file=sys.stderr)
