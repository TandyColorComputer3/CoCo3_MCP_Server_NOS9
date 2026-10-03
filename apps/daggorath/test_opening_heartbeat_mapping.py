#!/usr/bin/env python3
"""Guard the target-side nonmapping heartbeat-pack ownership contract.

The M5 live DAT proof found F$Load consumed the only gameplay overlay slot.
FNMLoad/ReturnModLinkInfo and FUnLoad in nitros9-reference f470fa52 supply
one system reference without a caller mapping, released after native close.
"""
from pathlib import Path
import re
import subprocess
import tempfile
import unittest

APP = Path(__file__).resolve().parent


class OpeningHeartbeatMapping(unittest.TestCase):
    def test_generated_pack_acquisition_is_nonmapping(self):
        with tempfile.TemporaryDirectory(prefix='dod-pack-mapping-') as temp:
            out = Path(temp)
            subprocess.run([
                'cmoc', '--os9', '-O2', '--compile', '--intermediate',
                '--intdir=' + temp, '-I' + str(APP / 'src'),
                '-o', str(out / 'pack.o'),
                str(APP / 'src/gameplay/opening-heartbeat.c')],
                cwd=out, check=True, stdout=subprocess.DEVNULL)
            assembly = (out / 'opening-heartbeat.s').read_text()
            calls = re.findall(r'\bos9\s+\$([0-9a-f]+)', assembly, re.I)
            self.assertEqual(calls, ['1d', '22'])
            acquisition = assembly[assembly.index('os9 $22'):]
            self.assertLess(acquisition.index('puls u,y'), acquisition.index('sta '))
            self.assertIn('bcs @failed', acquisition)
            self.assertIn('lda #$e1', assembly)
            self.assertIn('/d1/dhbpack', (APP / 'src/gameplay/opening-heartbeat.c').read_text())

    def test_device_path_closes_before_owned_pack_reference(self):
        source = (APP / 'src/gameplay/demo.c').read_text()
        cleanup = source[source.index('opening_done:'):source.index('\n#else', source.index('opening_done:'))]
        self.assertLess(cleanup.index('native_heartbeat_close'),
                        cleanup.index('opening_heartbeat_modules_close'))
        self.assertIn('if(!heartbeat.opened)', cleanup)


if __name__ == '__main__':
    unittest.main()
