#!/usr/bin/env python3
"""Check archived file hashes and artifact-manifest claims."""

import hashlib
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "artifact-manifest.json"
SUMS = ROOT / "SHA256SUMS"
STATUSES = {"ORIGINAL_VERIFIED", "EXACTLY_REPRODUCED",
            "REPRODUCIBLE_RECONSTRUCTION", "SOURCE_ONLY", "MISSING", "UNVERIFIED"}


def digest(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


def main() -> None:
    expected = {}
    for line in SUMS.read_text().splitlines():
        match = re.fullmatch(r"([0-9a-f]{64})  (.+)", line)
        assert match, f"malformed SHA256SUMS line: {line!r}"
        sha, name = match.groups()
        assert name not in expected, f"duplicate SHA256SUMS entry: {name}"
        expected[name] = sha
    actual = {str(p.relative_to(ROOT)) for p in ROOT.rglob("*")
              if p.is_file() and p.name != "SHA256SUMS" and
              "__pycache__" not in p.parts}
    assert set(expected) == actual, f"SHA256SUMS inventory mismatch: {set(expected) ^ actual}"
    for name, sha in expected.items():
        assert digest(ROOT / name) == sha, f"SHA-256 mismatch: {name}"
    manifest = json.loads(MANIFEST.read_text())
    assert manifest["archive_status"] == "NOT_REPRODUCIBLE"
    for item in manifest["entries"]:
        assert item["verification_status"] in STATUSES
        rel = item["relative_archive_path"]
        if item["verification_status"] == "MISSING":
            assert rel is None, f"missing item has a path: {item['name']}"
            continue
        assert rel and rel in expected, f"unlisted artifact: {item['name']}"
        assert item["sha256"] == expected[rel], f"manifest hash mismatch: {rel}"
        assert item["size"] == (ROOT / rel).stat().st_size, f"manifest size mismatch: {rel}"
    print(f"PASS: {len(expected)} archived file hashes and manifest entries verified")


if __name__ == "__main__":
    main()
