#!/usr/bin/env python3
"""Feed SingleStepTests 65816 JSON vectors to the K16 differential runner.

M5.55 bootstrap stage: validates corpus shape and selects deterministic vectors.
The next gate serializes selected initial/final states to the C runner.
"""
import argparse,json,pathlib,sys,subprocess
p=argparse.ArgumentParser();p.add_argument("json");p.add_argument("--limit",type=int,default=10);p.add_argument("--runner",default="emulator/build/test_differential");p.add_argument("--emit",action="store_true");p.add_argument("--run",action="store_true");a=p.parse_args()
data=json.loads(pathlib.Path(a.json).read_text())
if not isinstance(data,list): raise SystemExit("expected top-level JSON array")
required={"pc","s","p","a","x","y","dbr","d","pbr","e","ram"}
for i,v in enumerate(data[:a.limit]):
 if not {"name","initial","final","cycles"}<=v.keys(): raise SystemExit(f"vector {i}: missing top-level fields")
 for side in ("initial","final"):
  missing=required-set(v[side])
  if missing: raise SystemExit(f"{v['name']} {side}: missing {sorted(missing)}")
 if a.emit:\n  keys=("pc","s","p","a","x","y","dbr","d","pbr","e")\n  vals=[str(v["initial"][k]) for k in keys]+[str(v["final"][k]) for k in keys]\n  ram=lambda side: ",".join(f"{addr}={val}" for addr,val in v[side]["ram"]) or "-"\n  cmd=[a.runner,v["name"],*vals,ram("initial"),ram("final")]\n  print("\\t".join(cmd))\n  if a.run:\n   q=subprocess.run(cmd)\n   if q.returncode: raise SystemExit(q.returncode)\n else:\n  print(v["name"])\n  if a.run:\n   keys=("pc","s","p","a","x","y","dbr","d","pbr","e"); vals=[str(v["initial"][k]) for k in keys]+[str(v["final"][k]) for k in keys]; ram=lambda side: ",".join(f"{addr}={val}" for addr,val in v[side]["ram"]) or "-"; q=subprocess.run([a.runner,v["name"],*vals,ram("initial"),ram("final")]);\n   if q.returncode: raise SystemExit(q.returncode)
print(f"validated {min(a.limit,len(data))} vectors",file=sys.stderr)
