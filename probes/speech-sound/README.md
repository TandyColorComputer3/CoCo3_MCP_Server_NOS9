# Disposable Speech/Sound architecture probe

Research only; **not a production audio driver or Daggorath backend**.
See [the experiment report](../../docs/apps/DAGGORATH_SPEECH_SOUND_PROBE.md).

Build from the repository root:

```sh
python3 probes/speech-sound/build.py
```

This uses CMOC 0.1.90 / lwtools 4.22 and the unchanged OS-9 adapters in
`apps/daggorath/src/os9.c`. `-O0` is intentional: CMOC does not implement
`volatile`. The live-tested generated assembly explicitly loads/stores each
hardware register; re-audit this if changing compiler/optimization.

The build writes a module, intermediate assembly and provenance under ignored
`MCP/work/audio-ssc/build/`. It never mounts or writes an image. Use the documented
disposable staging workflow; do not install this into canonical EOU media.

## Commands

Run only with exclusive SSC and audio-mux ownership. Do not run another sound,
speech or joystick client concurrently. All timings are research measurements
for this verified EOU clock layout, not a portable monotonic-clock API.

| Command | Experiment |
|---|---|
| `ssprobe baseline` | Sleeping helper + five-second observer |
| `ssprobe load` | Two-second CPU load + observer |
| `ssprobe sstone` | Existing blocking `SS.Tone` control + observer |
| `ssprobe ssc` | Two-second SSC tone + observer |
| `ssprobe canceltest` | Helper sends itself signal 3, cleans up, returns 003 |
| `ssprobe errortest` | Helper injects 187 after one second, cleans up |
| `ssprobe graphics` | SSC + observer + separately loaded unchanged `dodwiz` |

`hold`, `cancel`, `error`, `idle`, `busy`, `tone`, and `obs` are internal helper
modes. The coordinator forks/reaps actual guest processes. Signal/cancellation
tests target the hardware-owning helper, not an unverified whole-process-group
cancellation scheme. An uncatchable kill cannot execute cleanup.

`analyze_audio.py capture.wav output-directory` uses NumPy to analyze the
verified four-channel MAME WAV layout. It extracts SSC channel 1, finds sustained
AC activity, reports FFT peaks/cleanup silence, and produces a native-rate mono
excerpt. It is not a generic channel-discovery or audio-fidelity tool.
