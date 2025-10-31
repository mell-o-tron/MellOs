#!/usr/bin/env python3
import re
import sys

inp, outp = sys.argv[1], sys.argv[2]

def cmake_escape(s: str) -> str:
    return s.replace("\\", "\\\\").replace("\"", "\\\"")

lines = []
with open(inp, "r") as f:
    for raw in f:
        s = raw.strip()
        if not s or s.startswith("#"):
            continue
        m = re.match(r'([A-Z0-9_]+)=(.*)$', s)
        if not m:
            continue
        key, val = m.group(1), m.group(2)

        if val == "y":
            lines.append(f'set({key} ON)')
        elif val == "n":
            lines.append(f'set({key} OFF)')
        elif val.isdigit() or (val.startswith("-") and val[1:].isdigit()):
            lines.append(f'set({key} {val})')
        else:
            if len(val) >= 2 and ((val[0] == '"' and val[-1] == '"') or (val[0] == "'" and val[-1] == "'")):
                val = val[1:-1]
            lines.append(f'set({key} "{cmake_escape(val)}")')

with open(outp, "w") as g:
    g.write("# Auto-generated from auto.conf — do not edit.\n")
    for l in lines:
        g.write(l + "\n")
