#!/usr/bin/env python3
"""Refuse speculative protected-presenter builds until original inputs return.

The original CMOC/lwasm commands and source bytes were not preserved. This
guard is intentional: another experiment's flags are not evidence of this one.
"""

import argparse
from pathlib import Path
import sys


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    required = [root / "src/presenter.c", root / "src/client.c",
                root / "build/original-build.json"]
    missing = [str(p.relative_to(root)) for p in required if not p.is_file()]
    if missing:
        print("NOT REPRODUCIBLE: original build inputs missing: " +
              ", ".join(missing), file=sys.stderr)
        return 2
    print("NOT REPRODUCIBLE: original commands have not been audited; "
          "do not execute a reconstructed build", file=sys.stderr)
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
