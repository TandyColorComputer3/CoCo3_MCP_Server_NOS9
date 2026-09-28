#!/usr/bin/env python3
"""Bounded SSC diagnostic command-byte and lifecycle checks."""
from pathlib import Path
import subprocess
import tempfile

app = Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix="dod-listen-") as tmp:
    subprocess.run(["cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
                    "-I" + str(app.parent / "window-manager/test/compat"),
                    "-I" + str(app / "src"),
                    str(app / "test/audio_listen.c"), "-o", tmp + "/test"], check=True)
    subprocess.run([tmp + "/test"], check=True)
