#!/usr/bin/env python3
"""Static checks for the private Lua observer; does not run MAME or Lua."""
from pathlib import Path
import re

HERE = Path(__file__).resolve().parent
OBSERVER = HERE.parent / "observer/observer.lua"
text = OBSERVER.read_text(encoding="utf-8")

for required in (
    "mem:install_write_tap(0xff22, 0xff22",
    "m.video:snapshot_pixels()",
    "emu.add_machine_frame_notifier",
    "WAITING_FOR_GRAPHICS",
    "GRAPHICS_ACTIVE",
    "GRAPHICS_DEPARTED",
    "partial-rows",
    "obsolete",
    "MAX_EVENTS = 4096",
    "msh_observer_ready",
    "msh_observer_drain",
):
    if required not in text:
        raise SystemExit(f"observer source missing required contract: {required}")
if "screen:pixel" in text:
    raise SystemExit("observer must not classify through screen:pixel")
if re.search(r"(?:io\.open|writefile|save_png|png\.encode)", text, re.IGNORECASE):
    raise SystemExit("observer source contains a file/PNG write path")

patterns = re.findall(r"local m[01] = '([0-9A-F]+)'", text)
if len(patterns) != 2 or any(len(pattern) != 56 for pattern in patterns):
    raise SystemExit("heart phase masks must each contain 56 hex pixels")
m0, m1 = patterns
if m0 == m1:
    raise SystemExit("phase masks must differ")

def classify(mask: str, phase: int) -> str:
    current, old = (m0, m1) if phase == 0 else (m1, m0)
    if mask == current:
        return "new"
    if mask == old:
        return "obsolete"
    if mask == "0" * 56:
        return "blank"
    current_row = old_row = True
    saw_current = saw_other = False
    for start in range(0, 56, 8):
        row = mask[start:start + 8]
        if row == current[start:start + 8]:
            saw_current = True
        elif row == old[start:start + 8] or row == "0" * 8:
            saw_other = True
        else:
            current_row = False
            old_row = False
            break
    return "partial-rows" if current_row and old_row and saw_current and saw_other else "other"

partial = m0[:16] + m1[16:]
cases = {
    "new phase 0": classify(m0, 0) == "new",
    "old phase 0": classify(m1, 0) == "obsolete",
    "new phase 1": classify(m1, 1) == "new",
    "blank": classify("0" * 56, 0) == "blank",
    "partial rows": classify(partial, 0) == "partial-rows",
    "unknown": classify("A" * 56, 0) == "other",
}
failed = [name for name, passed in cases.items() if not passed]
if failed:
    raise SystemExit("classifier controls failed: " + ", ".join(failed))
print("observer static contract: PASS")
print("classifier controls: " + ", ".join(f"{name}=PASS" for name in cases))
print("Lua initialization, callback behavior, and MAME compatibility: NOT TESTED")
