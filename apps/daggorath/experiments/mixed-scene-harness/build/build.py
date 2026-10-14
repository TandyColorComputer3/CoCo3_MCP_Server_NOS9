#!/usr/bin/env python3
"""Build the disposable mixed-scene client without touching production outputs."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess

HARNESS = Path(__file__).resolve().parents[1]
REPO = HARNESS.parents[3]
APP = REPO / "apps/daggorath"
TOOLSHED_DEFAULT = Path(
    "/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9"
)
SOURCES = [
    HARNESS / "workload/status-client.c",
    APP / "src/compat/heartbeat-broker-client.asm",
    APP / "src/compat/expand.asm",
    APP / "src/compat/present.c",
    APP / "src/gameplay/opening-heartbeat.c",
    APP / "src/audio/ipc.c",
    APP / "src/window-path.c",
    APP / "src/os9.c",
]
HEADERS = [
    HARNESS / "workload/status-client.c",
    APP / "src/platform.h",
    APP / "src/compat/compat.h",
    APP / "src/compat/present.h",
    APP / "src/compat/heartbeat-broker.h",
    APP / "src/gameplay/opening-heartbeat.h",
    APP / "src/audio/audio.h",
    APP / "src/audio/ipc.h",
]
CMOC_INPUTS = [
    Path("/usr/local/share/cmoc/include/cmoc.h"),
    Path("/usr/local/share/cmoc/include/stdarg.h"),
    Path("/usr/local/share/cmoc/lib/libcmoc-crt-os9.a"),
    Path("/usr/local/share/cmoc/lib/libcmoc-std-os9.a"),
]


def run(args: list[str], *, cwd: Path | None = None) -> str:
    completed = subprocess.run(args, cwd=cwd, text=True, stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT)
    if completed.returncode:
        raise RuntimeError(f"command failed ({completed.returncode}): {' '.join(args)}\n{completed.stdout}")
    return completed.stdout


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def tool_identity(name: str) -> dict[str, str]:
    executable = shutil.which(name)
    if not executable:
        raise RuntimeError(f"required tool not found on PATH: {name}")
    path = Path(executable).resolve()
    return {"path": str(path), "sha256": sha256(path)}


def input_label(path: Path) -> str:
    try:
        return str(path.relative_to(REPO))
    except ValueError:
        return str(path)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--rate", type=int, default=46, choices=(46, 45, 41))
    parser.add_argument("--presentations", type=int, default=8)
    parser.add_argument("--os9", type=Path, default=TOOLSHED_DEFAULT)
    args = parser.parse_args()
    if not 1 <= args.presentations <= 250:
        parser.error("--presentations must be from 1 through 250")
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    if any(out.iterdir()):
        parser.error(f"output directory is not empty: {out}")
    inputs = list(dict.fromkeys(SOURCES + HEADERS + CMOC_INPUTS))
    missing = [str(path) for path in inputs if not path.is_file()]
    if missing:
        parser.error("missing source dependencies: " + ", ".join(missing))
    if not args.os9.is_file():
        parser.error(f"ToolShed executable not found: {args.os9}")

    cmd = [
        "cmoc", "--os9", "-O2", "--intermediate", "--verbose",
        "--add-os9-stack-space=1024", "-DDAG_COMPAT_HEARTBEAT",
        "-DDAG_COMPAT_C6", f"-DMSH_RATE={args.rate}",
        f"-DMSH_PRESENTATIONS={args.presentations}",
        f"--intdir={out}", f"-I{APP / 'src'}", f"-I{APP / 'src/compat'}",
        f"-I{APP / 'src/audio'}", "-o", "msharness",
        *(str(path) for path in SOURCES),
    ]
    compiler_log = run(cmd, cwd=out)
    (out / "cmoc.log").write_text(compiler_log, encoding="utf-8")
    module = out / "msharness"
    ident = run([str(args.os9), "ident", str(module)])
    if not re.search(r"Module CRC\s*:.*\(Good\)", ident):
        raise RuntimeError("ToolShed did not report a Good OS-9 module CRC:\n" + ident)
    if not re.search(r"Header for\s*:\s*msharness\s*$", ident, re.MULTILINE | re.IGNORECASE):
        raise RuntimeError("ToolShed output did not identify module name msharness:\n" + ident)
    if not re.search(r"Ty/La At/Rv:\s*\$11\s+\$80", ident):
        raise RuntimeError("ToolShed output did not show a reentrant executable module:\n" + ident)
    data_match = re.search(r"Data size\s*:\s*\$[0-9A-Fa-f]+\s+#(\d+)", ident)
    map_path = out / "msharness.map"
    map_text = map_path.read_text(encoding="utf-8") if map_path.is_file() else ""
    end_match = re.search(r"Section:\s+program_end.*?load at\s+([0-9A-Fa-f]+)", map_text)
    if not data_match or not end_match:
        raise RuntimeError("ToolShed data request or linker program_end is missing")
    data_request = int(data_match.group(1))
    program_end = int(end_match.group(1), 16)

    versions = {
        "cmoc": {**tool_identity("cmoc"), "version": run(["cmoc", "--version"]).strip()},
        "cpp": {**tool_identity("cpp"), "version": run(["cpp", "--version"]).splitlines()[0]},
        "lwasm": {**tool_identity("lwasm"), "version": run(["lwasm", "--version"]).strip()},
        "lwlink": {**tool_identity("lwlink"), "version": run(["lwlink", "--version"]).strip()},
        "toolshed": {"path": str(args.os9.resolve()), "sha256": sha256(args.os9.resolve()),
                     "version": "ToolShed 2.2"},
    }
    result = {
        "module": "msharness",
        "size": module.stat().st_size,
        "data_request": data_request,
        "program_image_end": program_end,
        "program_dat_blocks": (program_end + 8191) // 8192,
        "data_dat_blocks": (data_request + 8191) // 8192,
        "sha256": sha256(module),
        "rate": args.rate,
        "presentations": args.presentations,
        "stack_space": 1024,
        "toolchain": versions,
        "ident": ident.strip().splitlines(),
        "build_command": [os.path.basename(cmd[0]), *cmd[1:]],
        "source_dependencies": {
            input_label(path): sha256(path) for path in inputs
        },
    }
    (out / "build-result.json").write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
