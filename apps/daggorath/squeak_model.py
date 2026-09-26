#!/usr/bin/env python3
"""Nominal MC6809 cycle model, not an original-cartridge timing measurement.
Provenance: daggorath-reference SOUNDS.ASM SQUEAK/SNSQK2/SNSUB2/SNWAIT/SNOUT;
CD.ASM SETDP $02 / SNVOL. Motorola M6809PM Appendix D instruction timings and
indexed-mode table: LEAX -1,X = 4+1; stack = 5+bytes; SNVOL direct-page LDB=4.
Excludes interrupts, cartridge dispatch, memory wait states and 6309 native timing.
"""
import json
# SNWAIT includes PSHS X (7), n * (LEAX 5 + BNE 3), PULS X,PC (9).
# SNOUT = LDB direct 4 + MUL 11 + ANDA 2 + STA extended 5 + RTS 5 = 27.
# SNSUB2 = BSR 7 + SNOUT 27 + BRA 3 + SNWAIT = 53 + 8*n.
# One outer iteration = BSR7 + LDA2 + BSR7 + SNSUB2 + CLRA2 + BRA3
#                    + SNSUB2 + LEAX5 + BNE3 = 135 + 16*n.
cycles=3+sum(135+16*n for n in range(32,0,-1))+5
clock=894886
centres=[28,20,12,4]
print(json.dumps({'assumed_6809_hz':clock,'cycles_excluding_dispatch_irq':cycles,
 'nominal_duration_ms':cycles/clock*1000,
 'grouped_frequencies_hz':[round(clock/(135+16*n)) for n in centres],
 'original_path':'daggorath-reference/SOUNDS.ASM:SQUEAK'},indent=2))
