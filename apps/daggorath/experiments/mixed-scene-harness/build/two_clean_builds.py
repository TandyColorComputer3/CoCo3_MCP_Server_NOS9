#!/usr/bin/env python3
"""Run two isolated CMOC builds and compare the resulting module bytes."""
import argparse
import hashlib
from pathlib import Path
import subprocess
import sys
import tempfile

HERE = Path(__file__).resolve().parent
TOOLSHED_DEFAULT = Path(
    "/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9"
)


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--rate", type=int, choices=(46, 45, 41), default=46)
    parser.add_argument("--presentations", type=int, default=8)
    parser.add_argument("--os9", type=Path, default=TOOLSHED_DEFAULT)
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix="mixed-scene-harness-") as temp:
        root = Path(temp)
        outputs = [root / "clean-a", root / "clean-b"]
        for output in outputs:
            subprocess.run([
                sys.executable, str(HERE / "build.py"), "--out", str(output),
                "--rate", str(args.rate), "--presentations", str(args.presentations),
                "--os9", str(args.os9),
            ], check=True)
        modules = [output / "msharness" for output in outputs]
        hashes = [sha(module) for module in modules]
        if hashes[0] != hashes[1]:
            raise SystemExit(f"two clean builds differ: {hashes[0]} != {hashes[1]}")
        print(f"two clean builds: byte-identical SHA-256 {hashes[0]}")


if __name__ == "__main__":
    main()
