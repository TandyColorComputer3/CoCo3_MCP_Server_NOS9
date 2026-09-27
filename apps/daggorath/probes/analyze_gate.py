# Diagnostic analysis; requires NumPy. Uses MAME hook-block times across restores.
import csv,json,wave
import numpy as np
from pathlib import Path
import sys
w=Path(sys.argv[1])
prefix=sys.argv[2] if len(sys.argv)>2 else ''
rows=list(csv.DictReader((w/(prefix+'frames.csv')).open()));writes=list(csv.DictReader((w/(prefix+'writes.csv')).open()))
f=wave.open(str(w/(prefix+'session.wav')));sr=f.getframerate();ch=f.getnchannels();pcm=np.frombuffer(f.readframes(f.getnframes()),'<i2').reshape(-1,ch)[:,1].astype(float)/32768
blocks=[r for r in csv.DictReader((w/(prefix+'audio-blocks.csv')).open()) if 'ssc_audio' in r['tag']]
# MAME time can rewind on restore; use the recorded sound-hook blocks.
# All controls are later than the checkpoint, so choose the last matching block.
def sample(t):
 candidates=[r for r in blocks if float(r['time'])-int(r['count'])/sr<=t<=float(r['time'])]
 assert candidates,t
 r=candidates[-1];return int(r['first_sample'])+int(r['count'])-round((float(r['time'])-t)*sr)
pcm=np.fromfile(w/(prefix.rstrip('-')+'_ext_multi_slot2_ssc_ssc_audio.f32'),dtype='<f4').astype(float)
results=[]
for mode in (['natural-sync','register-sync','isolation-observer','cancel','error'] if prefix else ['silence','stop-b']):
 q=json.loads((w/(mode+'.json')).read_text());ws=[r for r in writes if q['start']<float(r['time'])<q['end'] and int(r['address'])==0xff7e]
 start=next(float(r['time']) for r in ws if int(r['data'])==0xd9)
 cf=next(float(r['time']) for r in ws if int(r['data'])==0xcf and float(r['time'])>start)
 tr=[];last=None
 for r in rows:
  t=float(r['time'])-start
  if not -.1<t<2.8:continue
  state={k:int(r[k]) for k in ['ay0','ay1','ay2','ay3','ay7','ay8','ay9']}
  if state!=last:tr.append({'afterPlaySeconds':t,**state});last=state
 windows={}
 end=cf-.05
 b_off=[float(r['time']) for r in rows if start<float(r['time'])<cf and int(r['ay9'])==0]
 if b_off:end=min(end,b_off[0]-.02)
 a=start-.5;x=pcm[sample(a):sample(end)];zero=x==0
 changes=np.diff(np.r_[False,zero,False].astype(np.int8));beg=np.where(changes==1)[0];fin=np.where(changes==-1)[0]
 gaps=[{'start':a+i/sr,'durationMs':1000*(j-i)/sr} for i,j in zip(beg,fin) if j-i>=sr*.002]
 for name,a,b in [('B_only',start-.6,start-.2),('A_and_B',start+.05,start+.16),('after_A',start+.85,start+1.3),('after_all_silent',cf+.3,cf+.45)]:
  x=pcm[sample(a):sample(b)]
  bins=np.fft.rfftfreq(len(x),1/sr);spec=abs(np.fft.rfft((x-x.mean())*np.hanning(len(x))))
  def peak(lo,hi):
   take=np.where((bins>=lo)&(bins<=hi))[0];i=take[np.argmax(spec[take])];return {'hz':float(bins[i]),'magnitude':float(spec[i])}
  windows[name]={'start':a,'end':b,'rms':float(np.sqrt(np.mean(x*x))),'acStd':float(x.std()),'A225Hz':peak(210,240),'B490Hz':peak(475,510)}
 results.append({'mode':mode,'playTime':start,'cfTime':cf,'transitions':tr,'pcmWindows':windows,'speakerZeroRunsWhileBActive':gaps,'response':q['response']})
# Diagnostic excerpts: retain sample order and every zero block, no normalization.
if prefix:
 import subprocess
 for r in results:
  if r['mode'] not in ('natural-sync','register-sync'):continue
  lo=r['playTime']-.25;hi=r['cfTime']+.4
  data=pcm[sample(lo):sample(hi)]
  path=w/(r['mode']+'.wav')
  with wave.open(str(path),'wb') as f:
   f.setnchannels(1);f.setsampwidth(2);f.setframerate(sr)
   f.writeframes(np.clip(data*32768,-32768,32767).astype('<i2').tobytes())
  subprocess.run(['/opt/local/bin/ffmpeg','-y','-v','error','-i',str(path),str(path.with_suffix('.flac'))],check=True)
  r['excerpt']={'start':lo,'end':hi,'sampleCount':len(data),'sampleRate':sr,'normalization':False}
(w/(prefix+'isolation-analysis.json')).write_text(json.dumps(results,indent=2)+'\n')
for r in results:print(r['mode'],[(k,round(v['acStd'],6),v['B490Hz']['hz']) for k,v in r['pcmWindows'].items()])
