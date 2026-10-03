#!/usr/bin/env python3
"""Check the generated Level II chain boundary, including CMOC register use."""
import subprocess
import tempfile
import unittest
from pathlib import Path

APP = Path(__file__).resolve().parent


class PhaseChainM4(unittest.TestCase):
    def test_generated_chain_contract(self):
        with tempfile.TemporaryDirectory() as temporary:
            output = Path(temporary)
            subprocess.run(['python3', str(APP / 'build_opening.py'), '--out', temporary],
                           check=True, stdout=subprocess.DEVNULL)
            chain = (output / 'phase-chain.s').read_text()
            launcher = (output / 'opening-launch.s').read_text()
            load = chain.index('os9 $22')
            release = chain.index('LBSR\t_dod_chain_release', load)
            replace = chain.index('os9 $05', release)
            self.assertLess(load, release)
            self.assertLess(release, replace)
            self.assertNotIn('ORB\t#128', chain[release:replace])
            self.assertIn('os9 $1d', chain)
            self.assertIn('leax -288,U\n        os9 $22', chain[load - 100:load + 20])
            self.assertIn('lda #$11', chain[replace - 150:replace])
            self.assertIn('ldy 8,U', chain[replace - 150:replace])
            self.assertIn('leau -256,U', chain[replace - 150:replace])
            self.assertIn('LDB\t#$08', launcher)
            self.assertIn('stb -', chain[replace:replace + 100].lower())


if __name__ == '__main__':
    unittest.main()
