"""Raw MAME speaker PCM analysis; no normalization, interpolation or gap repair."""
from pathlib import Path
import csv,json,sys,wave
import numpy as np
w=Path(sys.argv[1]);timing=json.loads((w/'timing-analysis.json').read_text());blocks=list(csv.DictReader((w/'audio-blocks.csv').open()));speaker=np.memmap(w/'_speaker.f32',dtype='<f4',mode='r')
def sample(t,tag):
 choices=[r for r in blocks if r['tag']==tag and float(r['time'])-int(r['count'])/48000<=t<=float(r['time'])]
 if not choices:raise ValueError(('no block',tag,t))
 r=choices[-1];return int(r['first_sample'])+int(r['count'])-round((float(r['time'])-t)*48000)
def span(a,b):return np.asarray(speaker[sample(a,':speaker'):sample(b,':speaker')])
result={}
for name in ['slow','fast']:
 if name not in timing:continue
 edges=timing[name]['edgeTimes'];deltas=[]
 for t in edges[1:-1]:
  before=float(np.median(span(t-.015,t-.005)));after=float(np.median(span(t+.005,t+.015)));deltas.append(abs(after-before))
 a=edges[0]-.2;b=edges[min(7,len(edges)-1)]+.15;x=span(a,b)
 result[name]={'absoluteStepMin':min(deltas),'absoluteStepMax':max(deltas),'edgeSamplesMeasured':len(deltas),'sampleRate':48000,'rawExcerptStart':a,'rawExcerptEnd':b,'peakMin':float(x.min()),'peakMax':float(x.max())}
 with wave.open(str(w/(name+'-native.wav')),'wb') as f:
  f.setnchannels(1);f.setsampwidth(2);f.setframerate(48000);f.writeframes(np.clip(x*32768,-32768,32767).astype('<i2').tobytes())
(w/'pcm-analysis.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
