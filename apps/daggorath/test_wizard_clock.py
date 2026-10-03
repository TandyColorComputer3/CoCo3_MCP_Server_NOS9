#!/usr/bin/env python3
"""Wizard resynchronizes a restored RTC jump without advancing animation time."""
import subprocess
import tempfile
from pathlib import Path

app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-wizard-clock-') as tmp:
    exe=Path(tmp)/'wizard-clock'
    subprocess.run(['cc','-I'+str(app.parent/'window-manager/test/compat'),
                    '-I'+str(app/'src'),str(app/'test/wizard_clock.c'),'-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
print('Wizard clock: normal tick, minute wrap, two RTC jumps, and recovery pass')
