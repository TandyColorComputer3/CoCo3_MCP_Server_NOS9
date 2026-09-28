#!/usr/bin/env python3
"""Source-ID, SSC phrase-bound and explicit protocol-operation checks."""
from pathlib import Path
import subprocess
import tempfile
app = Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix="dod-audio-catalog-") as out:
    subprocess.run(["cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
                    "-I" + str(app.parent / "window-manager/test/compat"),
                    "-I" + str(app / "src"),
                    str(app / "src/audio/event.c"),
                    str(app / "src/audio/ssc_catalog.c"),
                    str(app / "test/audio_catalog.c"),
                    "-o", out + "/catalog"], check=True)
    subprocess.run([out + "/catalog"], check=True)
    subprocess.run(["cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
                    "-I" + str(app.parent / "window-manager/test/compat"),
                    "-I" + str(app / "src"),
                    str(app / "src/audio/ssc.c"),
                    str(app / "src/audio/ssc_catalog.c"),
                    str(app / "test/audio_catalog_backend.c"),
                    "-o", out + "/backend"], check=True)
    subprocess.run([out + "/backend"], check=True)
