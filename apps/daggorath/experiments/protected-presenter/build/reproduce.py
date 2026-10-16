#!/usr/bin/env python3
"""Refuse builds until the exact protected-presenter inputs are recovered.

The original client source and build script were recovered from session history,
but the final presenter source, historical linked-source identities, and exact
compiler/assembler versions are missing. This guard intentionally performs no
build and never substitutes current production sources.
"""

import argparse
from pathlib import Path
import sys


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    presenter = root / "source/protpr/presenter.c"
    if not presenter.is_file():
        print("NOT REPRODUCIBLE: exact original source/protpr/presenter.c is missing; "
              "the recovered Stage 3E.10 predecessor and truncated patch are not substitutes",
              file=sys.stderr)
        return 2
    print("NOT REPRODUCIBLE: historical dependency hashes and tool versions must be verified "
          "before building", file=sys.stderr)
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
