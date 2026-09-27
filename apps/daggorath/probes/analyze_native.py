"""Read-only MAME native heartbeat analysis; requires NumPy.
Use hook block timestamps rather than WAV position across save restores.
"""
import csv,json,statistics,sys,wave
from pathlib import Path
import numpy as np
w=Path(sys.argv[1]);writes=list(csv.DictReader((w/'writes.csv').open()))
# Last record may be incomplete while MAME is running; analyze after shutdown.
reads=list(csv.DictReader((w/'reads.csv').open()));reads=[r for r in reads if all(r.values())];frames=list(csv.DictReader((w/'frames.csv').open()))
blocks=list(csv.DictReader((w/'audio-blocks.csv').open()))
def sample(t,tag,rate):
 r=[r for r in blocks if r['tag']==tag and float(r['time'])-int(r['count'])/rate<=t<=float(r['time'])][-1]
 return int(r['first_sample'])+int(r['count'])-round((float(r['time'])-t)*rate)
speaker=np.memmap(w/'_speaker.f32',dtype='<f4',mode='r')
ssc=np.memmap(w/'_ext_multi_slot2_ssc_ssc_audio.f32',dtype='<f4',mode='r')
def span(x,tag,a,b):return np.asarray(x[sample(a,tag,48000):sample(b,tag,48000)])
results=[]
for name in ['slow','fast','repeat','observer','ssc','wizard','cancel','error']:
 if not (w/(name+'.json')).exists():continue
 q=json.loads((w/(name+'.json')).read_text());a,b=q['start'],q['end']
 wr=[r for r in writes if a<float(r['time'])<b]
 levels=[r for r in wr if int(r['address'])==0xff22 and int(r['data']) in (0,2)]
 edges=[];old=0
 for r in levels:
  if int(r['data'])!=old:edges.append(float(r['time']));old=int(r['data'])
 intervals=np.diff(edges)
 timing=[]
 for t in [float(r['time']) for r in levels]:
  before=[r for r in reads if 0<t-float(r['time'])<.0001 and int(r['address'])==0xff23]
  after=[r for r in reads if 0<float(r['time'])-t<.0001 and int(r['address'])==0xff23]
  if before and after:timing.append((float(after[0]['time'])-float(before[-1]['time']))*1e6)
 fr=[r for r in frames if a<float(r['time'])<b];ticks=[int(r['second'])*60+60-int(r['tick']) for r in fr];d=[(y-x)%3600 for x,y in zip(ticks,ticks[1:])]
 rec={'control':name,'response':q['response'],'edgeCount':len(edges),'edges':edges,'intervalMin':float(min(intervals)) if len(intervals) else None,'intervalMax':float(max(intervals)) if len(intervals) else None,'intervalMean':float(np.mean(intervals)) if len(intervals) else None,'readBracketMicroseconds':{'min':min(timing),'max':max(timing)} if timing else None,'maxFrameTickDelta':max(d) if d else None}
 active=[r for r in fr if edges and edges[0]<=float(r['time'])<=edges[-1]]
 ticks=[int(r['second'])*60-int(r['tick']) for r in active]
 rec['maxActiveFrameTickDelta']=max([(y-x)%3600 for x,y in zip(ticks,ticks[1:])] or [0])
 if name=='slow' and len(edges)>2:rec['steadyIntervalMean']=float(np.mean(np.diff(edges[1:])))
 if name in ['slow','fast'] and len(edges)>2:
  t=edges[2];x=span(speaker,':speaker',t-.03,t+.07)
  rec['step']={'time':t,'min':float(x.min()),'max':float(x.max()),'before':float(np.median(x[:480])),'after':float(np.median(x[-480:]))}
  out=w/(name+'-step.wav')
  with wave.open(str(out),'wb') as f:f.setnchannels(1);f.setsampwidth(2);f.setframerate(48000);f.writeframes(np.clip(x*32768,-32768,32767).astype('<i2').tobytes())
  # Audition window is raw, no normalization or gap repair.
  x=span(speaker,':speaker',edges[0]-.2,edges[min(len(edges)-1,7)]+.3)
  with wave.open(str(w/(name+'-excerpt.wav')),'wb') as f:f.setnchannels(1);f.setsampwidth(2);f.setframerate(48000);f.writeframes(np.clip(x*32768,-32768,32767).astype('<i2').tobytes())
 if name=='ssc':
  play=next(float(r['time']) for r in wr if int(r['address'])==0xff7e and int(r['data'])==0xd9)
  cf=next(float(r['time']) for r in wr if int(r['address'])==0xff7e and int(r['data'])==0xcf and float(r['time'])>play)
  rec['sscPlay']=play;rec['sscStop']=cf;rec['sscFrames']=[{k:r[k] for k in ['time','ay0','ay1','ay2','ay3','ay7','ay8','ay9']} for r in fr if play-.1<float(r['time'])<play+.9]
  x=span(ssc,':ext:multi:slot2:ssc:ssc_audio',play-.3,cf-.1)
  z=np.diff(np.r_[False,x==0,False].astype(int));beg=np.where(z==1)[0];end=np.where(z==-1)[0]
  rec['sscZeroGaps']=[{'time':play-.3+i/48000,'ms':(j-i)/48} for i,j in zip(beg,end) if j-i>=96]
  rec['sscWindowStd']={label:float(span(ssc,':ext:multi:slot2:ssc:ssc_audio',play+l,play+h).std()) for label,l,h in [('B',-.3,-.1),('AB',.05,.15),('BafterA',.85,1.1)]}
 results.append(rec)
(w/'analysis.json').write_text(json.dumps(results,indent=2)+'\n')
for r in results:print(r['control'],r['edgeCount'],r['intervalMin'],r['intervalMax'],r['readBracketMicroseconds'],r['maxFrameTickDelta'])
