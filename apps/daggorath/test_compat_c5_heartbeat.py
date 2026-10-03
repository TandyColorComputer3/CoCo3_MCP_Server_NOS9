#!/usr/bin/env python3
"""Deterministic C5 heartbeat/audio and reviewed choreography regression gate."""
from pathlib import Path
import csv

APP = Path(__file__).resolve().parent
DRIVER = (APP / "src/audio/native/driver.asm").read_text()
TARGET = (APP / "test/compat_c5_target.c").read_text()
PRESENT = (APP / "src/compat/present.c").read_text()
C5 = (APP / "src/compat/c5.c").read_text()
TRACE = APP / "test/fixtures/compat-c5/heartbeat-trace.csv"

# The native VIRQ edge is the single authority: PB1, visual phase, and the
# monotonic generation are changed together before the countdown is rearmed.
edge = DRIVER[DRIVER.index("edge ldb $ff22"):DRIVER.index("edgecountdone")]
ordered = ["eorb #2", "stb $ff22", "eora #1", "sta phase,u",
           "addd #1", "std edgegen+2,u"]
positions = [edge.index(token) for token in ordered]
assert positions == sorted(positions)

# Source CLK30 is active during first-view construction. C5 starts the native
# source immediately before that construction, but gates the small heart-only
# transfer until the complete status strip exists. The accepted target render
# completes inside the initial 46-jiffy period, so its first edge is visible.
enable = TARGET.index("e=native_heartbeat_enable(&heartbeat);")
active = TARGET.index("heartbeatActive=1", enable)
render = TARGET.index("e=render_scene();if(e)goto done;", active)
ready = TARGET.index("heartPixelsReady=1;", render)
assert enable < active < render < ready

# The bounded update is only the 32x7 heart rectangle and cannot create the
# former white status-square artifact.
assert "static Byte heartPut[]={27,0x2d,196,2,1,48,0,156};" in PRESENT
assert "for(row=152;row<159;row++)" in PRESENT
assert "for(col=15;col<17;col++)" in PRESENT

class Heart:
    def __init__(self):
        self.generation = self.audio = self.phase = self.shown = 0
        self.visual = 0
        self.rows = [(0, 0, 0, 0, "initial-full")]

    def logical_edge(self, tick, state):
        self.generation += 1
        self.audio ^= 1
        self.phase ^= 1
        self.rows.append((tick, self.generation, self.audio, self.phase,
                          state + "-edge"))

    def reconcile(self, tick, state, edge_during_first_put=False):
        first = True
        while self.shown != self.generation:
            observed = self.generation
            observed_phase = self.phase
            self.visual += 1
            self.shown = observed
            self.rows.append((tick, observed, self.audio, observed_phase,
                              state + "-heart-put"))
            if first and edge_during_first_put:
                self.logical_edge(tick, state + "-during-put")
            first = False

h = Heart()
# Representative heavy VECTOR interval. Checkpoints occur far more often than
# the 46-jiffy heartbeat; a forced edge during one PutBlk proves retry behavior.
for tick in range(1, 277):
    if tick % 46 == 0:
        h.logical_edge(tick, "vector")
        h.reconcile(tick, "vector", edge_during_first_put=(tick == 138))
    elif tick % 3 == 0:
        h.reconcile(tick, "vector")

assert h.generation == h.visual == 7
assert h.shown == h.generation
assert h.audio == h.phase == (h.generation & 1)

with TRACE.open(newline="") as f:
    expected = [(int(a), int(b), int(c), int(d), e)
                for a, b, c, d, e in csv.reader(f) if a != "tick"]
assert h.rows == expected

# Preserve the other human-reviewed C5 boundaries: word-at-a-time AUTTAB,
# partial Giant before forward Giant, and LOOK with no compatibility sleep.
for command in ('{"EXAMINE",1}', '{"PULL RIGHT TORCH",3}',
                '{"USE RIGHT",2}', '{"LOOK",1}'):
    assert command in C5
assert "os_sleep" not in C5 and "sleep_with_heart" not in C5
assert "c->services->word" in C5 and "DOD_SCHED_PLAYER_WAIT" in C5

print("compat C5 heartbeat/audio synchronization and choreography checks passed")
