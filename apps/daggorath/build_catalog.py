#!/usr/bin/env python3
"""Reproducible guest diagnostic using the same SSC backend as dodaudio."""
from pathlib import Path
import argparse, hashlib, json, subprocess

parser = argparse.ArgumentParser()
parser.add_argument("--out", required=True)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
src = root / "apps/daggorath/src"
out = Path(args.out).resolve()
out.mkdir(parents=True, exist_ok=True)
sources = [src / "audio" / p for p in (
    "catalog-main.c", "ssc.c", "ssc_catalog.c", "ssc_io.c",
    "catalog-module.asm"
)] + [src / "os9.c"]
command = ["cmoc", "--os9", "-O0", "--intermediate", "--intdir=" + str(out),
           "--add-os9-stack-space=2048", "-I" + str(src),
           "-I" + str(src / "audio"), "-o", str(out / "dodcatalog")]
command += [str(p) for p in sources]
result = subprocess.run(command, cwd=root, capture_output=True, text=True, check=True)
(out / "dodcatalog.log").write_text(result.stdout + result.stderr)
os9 = "/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9"
identity = subprocess.check_output([os9, "ident", str(out / "dodcatalog")], text=True)
assert "(Good)" in identity
record = {
    "command": command,
    "sources": {str(p.relative_to(root)): hashlib.sha256(p.read_bytes()).hexdigest()
                for p in sources + [src / "audio/audio.h", src / "audio/backend.h",
                                            src / "audio/policy.h", src / "audio/ssc_catalog.h",
                                            src / "audio/ssc_io.h", src / "platform.h"]},
    "sha256": hashlib.sha256((out / "dodcatalog").read_bytes()).hexdigest(),
    "ident": identity,
}
(out / "build.json").write_text(json.dumps(record, indent=2) + "\n")
print(identity)
