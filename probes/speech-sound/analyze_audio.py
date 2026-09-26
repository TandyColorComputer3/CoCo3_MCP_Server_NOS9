#!/usr/bin/env python3
"""Analyze MAME's captured SSC channel, not the host speaker or audio fidelity.
Requires NumPy. Usage: analyze_audio.py capture.wav output-directory
The verified session has four PCM16 channels; SSC is zero-based channel 1.
"""
from pathlib import Path
import json, sys, wave
import numpy as np
source, out = Path(sys.argv[1]), Path(sys.argv[2])
out.mkdir(parents=True, exist_ok=True)
with wave.open(str(source)) as wav:
    rate, channels = wav.getframerate(), wav.getnchannels()
    if wav.getsampwidth() != 2 or channels != 4:
        raise ValueError('Expected verified four-channel PCM16 MAME capture')
    pcm = np.frombuffer(wav.readframes(wav.getnframes()), dtype='<i2').reshape(-1, channels)
x = pcm[:, 1].astype(float)
step = rate // 100
blocks = len(x) // step
rms = np.std(x[:blocks * step].reshape(blocks, step), axis=1)
edges = np.diff(np.r_[False, rms > 100, False].astype(int))
segments = list(zip(np.where(edges == 1)[0] * step / rate,
                    np.where(edges == -1)[0] * step / rate))
results, transients = [], []
for start, end in segments:
    if end - start < .1:
        transients.append({'start_s': float(start), 'end_s': float(end)})
        continue
    z = x[int((start + .1) * rate):int((end - .1) * rate)].copy()
    z -= z.mean()
    spectrum = abs(np.fft.rfft(z * np.hanning(len(z))))
    frequency = np.fft.rfftfreq(len(z), 1 / rate)[int(np.argmax(spectrum))]
    tail = x[int((end + .1) * rate):int((end + .5) * rate)]
    results.append({'start_s': float(start), 'end_s': float(end),
                    'duration_s': float(end - start), 'peak_frequency_hz': float(frequency),
                    'ac_rms': float(np.sqrt(np.mean(z * z))),
                    'post_cleanup_peak': int(abs(tail).max()) if len(tail) else None})
if not results:
    raise ValueError('No sustained non-silent SSC waveform found')
report = {'sample_rate': rate, 'channels': channels, 'samples': len(pcm),
          'channel_analyzed': 1, 'threshold_ac_rms': 100, 'block_ms': 10,
          'segments': results, 'short_transients': transients}
(out / 'audio-metrics.json').write_text(json.dumps(report, indent=2) + '\n')
first = results[0]
with wave.open(str(out / 'ssc-normal.wav'), 'wb') as wav:
    wav.setparams((1, 2, rate, 0, 'NONE', 'not compressed'))
    wav.writeframes(pcm[int((first['start_s'] - .4) * rate):
                         int((first['end_s'] + .3) * rate), 1].tobytes())
print(json.dumps(report, indent=2))
