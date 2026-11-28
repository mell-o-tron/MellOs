#!/usr/bin/env python3
import json, os, subprocess, shlex

with open("compile_commands.json") as f:
    compdb = json.load(f)

for entry in compdb:
    directory = entry["directory"]
    file = entry["file"]
    command = entry["command"]
    flags = [x for x in shlex.split(command) if x.startswith("-I") or x.startswith("-D")]
    cpp_flags = sum([["--cpp-flag=" + f] for f in flags], [])
    flags.append("-D__ILP32__")
    flags.append("-D'__attribute__(x)='")
    flags.append("-D'__section__(x)='")
    flags.append("--main=main")
    print(flags)
    subprocess.run(
        ["cflow", "--format=dot", "--cpp", *flags, file],
        cwd=directory,
        stdout=open("calls.dot", "a")
    )
