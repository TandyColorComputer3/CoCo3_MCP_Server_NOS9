#!/usr/bin/env python3
"""Exact DGNGEN/NEWLVL state and RANDOM-consumption regression fixtures."""
import ctypes
import hashlib
import subprocess
import tempfile
import unittest
from pathlib import Path

from import_gameplay import generate
from test_demo_init import APP, Game


# The level-two/21 maze, OCB, CCB and seed are independently captured from
# original GAME40. The other rows lock the existing source-derived port across
# different level seeds and final-spin lengths before target optimization.
FIXTURES = (
    ('game_init', 0, 2470, '610054d84aa89db1ee6aa25dd7b011540c04b7fbe993ef5c9b21240920e08218', '485d62', 0, 16, 11, 65, 24),
    ('game_init', 21, 2257, '7d66f8c2d4f11826a5396048ca1f9c5d7064d5a815eea54a7cde8812a9536276', '6ed1f6', 0, 16, 11, 65, 24),
    ('game_init_demo', 0, 2583, '0516b1243e8f39594b04c83bbf27acdad42175bccb6dede6f62f25ea35b69fb1', '41d407', 2, 12, 22, 66, 23),
    ('game_init_demo', 21, 2344, 'd8f29331f6383203e5a5cc6b2e0b0498f962a9d73ef6c34c6d0df6aca50d9b33', '756519', 2, 12, 22, 66, 23),
    ('game_init_demo', 37, 2360, '5e05bbb798137d9a6ca14ac7b159adc8d9e6e09cb8edb74ce49731689ad34f5e', 'a4a5b1', 2, 12, 22, 66, 23),
)


class MazeOracle(unittest.TestCase):
    def test_6809_seed_abi_offset(self):
        self.assertEqual(ctypes.sizeof(Game), 2608)
        self.assertEqual(Game.seed.offset, 2576)

    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory()
        out = Path(cls.temp.name)
        generate(out)
        source = (APP / 'src/gameplay/game.c').read_text()
        marker = 'Byte game_random(Game *g){'
        assert source.count(marker) == 1
        # Test-only counter; normal modules and their Game layout are untouched.
        source = source.replace(marker, 'unsigned long maze_oracle_rng_calls=0; Byte game_random(Game *g){++maze_oracle_rng_calls;')
        (out / 'game.c').write_text(source)
        subprocess.run([
            'cc', '-O2', '-shared', '-fPIC', '-I' + str(APP / 'src'),
            '-I' + str(APP / 'src/gameplay'), '-I' + str(out),
            str(out / 'game.c'), str(APP / 'src/original/logical.c'),
            '-o', str(out / 'game.so'),
        ], check=True)
        cls.lib = ctypes.CDLL(str(out / 'game.so'))
        for name in ('game_init', 'game_init_demo'):
            getattr(cls.lib, name).argtypes = [ctypes.POINTER(Game), ctypes.c_ubyte]

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()

    def test_exact_full_state_and_random_calls(self):
        for name, second, calls, digest, seed, level, row, col, objects, creatures in FIXTURES:
            with self.subTest(name=name, second=second):
                g = Game()
                ctypes.c_ulong.in_dll(self.lib, 'maze_oracle_rng_calls').value = 0
                getattr(self.lib, name)(ctypes.byref(g), second)
                self.assertEqual(ctypes.c_ulong.in_dll(self.lib, 'maze_oracle_rng_calls').value, calls)
                self.assertEqual(hashlib.sha256(bytes(g)).hexdigest(), digest)
                self.assertEqual(bytes(g.seed).hex(), seed)
                self.assertEqual((g.level, g.row, g.col, g.dir, g.count, g.creatureCount),
                                 (level, row, col, 0, objects, creatures))


if __name__ == '__main__':
    unittest.main()
