# Daggorath SSC foreground sound catalog

**Scope:** all 23 foreground dispatch IDs in the read-only original
`/Volumes/SEDONA/Projects/daggorath-reference/SOUNDS.ASM:SNDTAB`. The native
heartbeat and wizard fade buzz retain their existing timing paths. This
catalog is an **SSC/AY interpretation** of source events, not a reconstruction
of the original six-bit DAC waveform. Original cartridge captures and their
source-built ROM provenance are in
[Gameplay M8](DAGGORATH_GAMEPLAY_M8.md#original-source-cartridge-sound-catalog-for-cue-identification).

## Interface and implementation

`apps/daggorath/src/audio/audio.h` assigns the source's 0–22 IDs. The existing
version-1 `AUDIO_PLAY` request keeps its verified M1/M2 three-event allowlist;
new `AUDIO_CATALOG_PLAY` (operation 5) explicitly accepts the full catalog at
gain 255. It uses the same eight-byte IPC frame, client, ownership, priority,
stop, drain, and shutdown behavior. It does not change gameplay event wiring.

`ssc_catalog.c` translates the 20 newly supported IDs into bounded SSC buffer-6
phrases. Each phrase has at most 64 bytes, uses the Tandy manual's tone/noise
postbytes, and ends in silence plus `$FF`. The established preloaded buffers
remain for IDs 0, 13, and 14. The fast-clock timer base is the verified
approximately 8 ms SSC timebase. Buffer loading waits for ready only when
needed and yields while busy. The catalog uses the existing 18-tick yielding
startup fence before testing sound-active; a six-tick trial stopped three
longer noise phrases before MAME's SSC became active. The service does not
synthesize audio with a foreground waveform loop. Source evidence is
`SOUNDS.ASM` generators and the
[Tandy Speech/Sound Cartridge manual, pp. 15–19, 25–28](https://colorcomputerarchive.com/repo/Documents/Manuals/Hardware/Speech-Sound%20Cartridge%20(Tandy).pdf).

| IDs | Original generator | SSC interpretation |
| --- | --- | --- |
| 0 | SQUEAK | Existing four-step tone |
| 1, 6, 8 | RATTLE, PSSST, PSSHT | Source-counted noise pulses |
| 2, 5, 9 | GROWL, GRAWL, SNARL | Distinct noise colors with attack/decay |
| 3 | BEOOP | Descending tone sweep |
| 4, 7 | KLANK, KKLANK | Two-channel metallic decay; separate lengths/pitches |
| 10, 11 | BDLBDL | Shared source generator, chirps then noise |
| 12 | GLUGLG | Four small rising sweeps |
| 13, 14 | PHASER, WHOOP | Existing preloaded M2 effects |
| 15, 19 | CLANG, CLANK | Two-channel metallic decay |
| 16, 17 | WHOOSH, CHUCK | Noise envelopes |
| 18 | KLINK | Tone plus noise decay |
| 20, 21, 22 | THUD, BANG, KABOOM | Short impacts and two-stage explosion |

In particular, ID 7 is the **knight-2 KKLANK** identified by the user from the
original cartridge captures. Its longer catalog phrase replaces the earlier
short *diagnostic* SSC knight-2 sketch. The catalog rendition has not been
confirmed by the user as a faithful match, and it is not connected to combat.
AY cannot reproduce the coupled DAC counter phase exactly.

## Reproducible build and disposable test

The host used CMOC 0.1.90, lwtools 4.22, and ToolShed 2.2. Exact source hashes
and CMOC arguments are retained in the private build records produced by:

```sh
python3 apps/daggorath/build_audio.py /private/tmp/dod-audio-catalog-final
python3 apps/daggorath/build_catalog.py --out /private/tmp/dod-catalog-production-final
```

ToolShed `os9 ident` returned Good CRCs:

| Module | Size | CRC | SHA-256 |
| --- | ---: | --- | --- |
| dodaudio | 9,426 | FFD70A | `28addc0bd752d23a24ae9fbce8c882608cdbab89fb0a5a52a5b7038748cec321` |
| dodsnd | 10,109 | 284953 | `59b052881ffdb1a85698e75aea485f8e5ab67a8971bb1168201e0e71fe35a5bc` |
| dodcatalog | 9,993 | 792A82 | `0ff9eb6bec1d0dcd3d3bad64fd3cefd9cbc59c561a50557d7e8c799c01fad959` |

Fresh second builds had identical SHA-256 values. `dodcatalog` is a separate
diagnostic using the same production SSC backend; `dodcatalog list`,
`dodcatalog all`, and `dodcatalog <name>` require no installation into `/dd`.
The live runs cold-booted fresh copies of the EOU VHD and boot floppy with a
read-only disposable `/d1` artifact floppy. Session-specific ready states were
made only after that floppy was attached.

Live verification completed:

- Before the fence correction, 23 separate `dodcatalog <name>` commands all
  returned `000` and had measurable SSC-channel AC in their own recorded WAV
  byte ranges. GROWL, GRAWL, and SNARL were only about 0.10–0.11 s of weak
  activity, although their phrases were longer. Their peak 10-ms AC RMS values
  were about 211, 127, and 105, respectively. A controlled 18-tick build
  yielded sustained approximately 0.42–0.44-s output clusters for these
  effects. This supports premature stop on delayed sound-active startup as the
  cause; it does not prove cartridge waveform fidelity. The approved new test
  expectation now requires this fence while preserving packet and dispatch
  checks.
- The final direct backend diagnostic, with the corrected fence, ran
  `dodcatalog growl`, `dodcatalog grawl`, and `dodcatalog snarl` from a fresh
  paired EOU state; each returned `000`. A prior full direct-backend catalog
  pass also returned `000` for IDs 0–22.
- The **final production IPC path** ran `load /d1/dodsnd`, `dodsnd catalog`,
  and `date`; all returned status `000`. The console showed `CAT 4` through
  `CAT 22` and `FINISH status=0`; `dodsnd catalog` completed in 36,065 ms.
  The harness submits and drains all IDs 0–22 through operation 5.
- The MAME four-channel WAV capture's SSC channel was non-silent over the
  final production IPC run (peak 100-ms RMS 4,472.3 PCM units, active region
  approximately 134.5–158.5 s in the session recording). Listen to the
  [25-second production-path preview](assets/daggorath-gameplay-m8/ssc-catalog-service-preview.mp3).
  This verifies MAME output, not audibility on the user's configured speakers
  or fidelity on a real cartridge. MAME's raw WAV header did not finalize on
  process termination; `ffmpeg` decoded its samples before the preview was
  created. No silent intervals were interpolated.

The stock VHD, development VHD, and EOU boot floppy retained SHA-256 hashes
`db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c`,
`4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e`,
and `9a51adb8656b9f003c7f822352c291487362f1b4c43aac1a16672d5d2bcb4c40`.
Their disposable session copies also matched after playback.

## Validation and limits

All 20 Daggorath test scripts passed, including new catalog bounds, dispatch,
and legacy-wire checks. The full MCP suite passed 115/115; `npm run build`
and `git diff --check` passed. The prior audio test runners were changed only
to link the separate `ssc_catalog.c` translation unit, with explicit user
approval; their existing assertions and mocks were not changed.

The catalog makes every foreground source ID callable and testable. It does
not establish original-cartridge pitch, timbre, envelope, or overlap fidelity,
nor does it wire effects into gameplay. The native heartbeat and wizard fade
buzz remain independent. A future game integration should use these source
IDs at verified original event boundaries and repeat coexistence/scheduling
tests; it should not assume that user recognition of original KKLANK validates
the SSC approximation.
