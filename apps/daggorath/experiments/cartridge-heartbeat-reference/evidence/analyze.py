#!/usr/bin/env python3
import csv, wave, struct, statistics, math, json, sys
from pathlib import Path
ROOT=Path('/private/tmp/c6-stage3e0')
FRAME_PERIOD=0.016688153335
SMALL='FFFFFF5FFE0FFE0FFF1FFFBFFFFF'
LARGE='FFFFFE4FFC07FC07FE0FFF1FFFBF'

def percentile(a,p):
 a=sorted(a)
 if not a:return None
 x=(len(a)-1)*p;lo=int(x);hi=min(lo+1,len(a)-1);return a[lo]+(a[hi]-a[lo])*(x-lo)
def stats(a):
 if not a:return {}
 return {'n':len(a),'min_ms':min(a)*1000,'median_ms':statistics.median(a)*1000,'p95_ms':percentile(a,.95)*1000,'max_ms':max(a)*1000,'stddev_ms':statistics.pstdev(a)*1000}
def row_classes(raw,old,new):
 out=[]
 for y in range(7):
  s=raw[y*4:y*4+4];o=old[y*4:y*4+4];n=new[y*4:y*4+4]
  if o==n and s==n: out.append('IDENTICAL')
  elif s==o: out.append('PREVIOUS')
  elif s==n: out.append('NEW')
  else: out.append('MIXED')
 return out

def wav_onsets(path,times):
 with wave.open(str(path),'rb') as w:
  fs=w.getframerate(); n=w.getnframes(); data=list(struct.unpack('<%dh'%n,w.readframes(n)))
 out={}
 for eid,t in times.items():
  i=math.ceil(t*fs)
  pre=data[max(0,i-40):max(0,i-8)]
  if len(pre)<16:out[eid]=(None,'short-pre');continue
  base=statistics.median(pre); spread=max(pre)-min(pre)
  if spread>512:out[eid]=(None,f'ambiguous-pre-spread-{spread}');continue
  found=None
  for j in range(i,min(i+32,n)):
   if abs(data[j]-base)>=1024:found=j;break
  if found is None:out[eid]=(None,'no-threshold-crossing')
  else:out[eid]=(found/fs,f'ok-spread-{spread}')
 return out

def snapshot_phases(root):
 import hashlib
 W,H=640,239;sz=W*H*3
 names=sorted(root.glob('raster-*.png'))
 raw=(root/'rasters.rgb').read_bytes()
 assert len(raw)==len(names)*sz
 # Event 2 is the verified large phase; event 3 is the verified small phase.
 hmap={}
 crops={}
 for i,n in enumerate(names):
  fr=raw[i*sz:(i+1)*sz]
  crop=b''.join(fr[(y*W+304)*3:(y*W+336)*3] for y in range(177,184))
  h=hashlib.sha256(crop).hexdigest()
  parts=n.stem.split('-');eid=int(parts[1]);age=int(parts[2])
  hmap[(eid,age)]=h;crops[h]=crop
 large=hmap[(2,1)];small=hmap[(3,1)]
 return hmap,large,small

def analyze(name):
 root=ROOT/name
 snap_hash,large_vis,small_vis=snapshot_phases(root)
 evrows=list(csv.DictReader(open(root/'events.csv')))
 fr=list(csv.DictReader(open(root/'frames.csv')))
 by={}
 for r in evrows:
  eid=int(r['event_id']); by.setdefault(eid,{})[r['kind']]=r
 valid=[]
 for eid,d in sorted(by.items()):
  if 'T0_LOGICAL' in d and 'T2_PB1' in d and 'V2_FINAL_WRITE' in d and int(d['T0_LOGICAL']['heartr'])>0:valid.append(eid)
 t2times={eid:float(by[eid]['T2_PB1']['emu_s']) for eid in valid}
 audio=wav_onsets(root/f'original-{name}.wav',t2times)
 ledger=[]
 for ix,eid in enumerate(valid):
  d=by[eid];t0=float(d['T0_LOGICAL']['emu_s']);t2=float(d['T2_PB1']['emu_s']);v1=float(d['V1_FIRST_WRITE']['emu_s']);v2=float(d['V2_FINAL_WRITE']['emu_s'])
  phase=int(d['V2_FINAL_WRITE']['hearts']);target=LARGE if phase==255 else SMALL;old=SMALL if phase==255 else LARGE
  eframes=[r for r in fr if int(r['event_id'])==eid and float(r['emu_s'])>=t0]
  # screen:pixel() is rejected here: on legacy VDG it was one render update stale.
  # The MAME snapshot product is the authoritative completed output bitmap.
  expected_vis=large_vis if phase==255 else small_vis
  age1=snap_hash.get((eid,1));age2=snap_hash.get((eid,2))
  complete=eframes[0] if age1==expected_vis else (eframes[1] if len(eframes)>1 and age2==expected_vis else None)
  first_change=complete
  partial=[]
  for age,h in ((1,age1),(2,age2)):
   if h is not None and h not in (large_vis,small_vis): partial.append((age,h))
  # writes in event frame +/- one completed frame
  sf0=int(d['T2_PB1']['screen_frame']); busy=sum(int(r['nonheart_writes']) for r in fr if abs(int(r['screen_frame'])-sf0)<=1)
  at,ast=audio[eid]
  ledger.append({'run':name,'event_id':eid,'index':ix+1,'phase':phase,'heartr':int(d['T0_LOGICAL']['heartr']),'t0_s':t0,'t2_pb1_s':t2,'t3_wave_s':at,'audio_status':ast,'v1_s':v1,'v2_s':v2,'v3_s':float(first_change['emu_s']) if first_change else None,'v4_s':float(complete['emu_s']) if complete else None,'v3_frame':int(first_change['screen_frame']) if first_change else None,'v4_frame':int(complete['screen_frame']) if complete else None,'logical_to_pb1_us':(t2-t0)*1e6,'pb1_to_v1_us':(v1-t2)*1e6,'v1_to_v2_us':(v2-v1)*1e6,'pb1_to_wave_us':(at-t2)*1e6 if at else None,'logical_to_v2_ms':(v2-t0)*1000,'pb1_to_v4_ms':(float(complete['emu_s'])-t2)*1000 if complete else None,'logical_to_v4_ms':(float(complete['emu_s'])-t0)*1000 if complete else None,'wave_to_v4_ms':(float(complete['emu_s'])-at)*1000 if complete and at else None,'partial_frames':len(partial),'partial_rows':'|'.join('unclassified-snapshot-age-%d'%a for a,_ in partial),'busy_writes_pm1':busy,'target':target})
 out=root/'heartbeat-correlation.csv'
 with open(out,'w',newline='') as f:
  w=csv.DictWriter(f,fieldnames=ledger[0].keys());w.writeheader();w.writerows(ledger)
 return ledger

def summarize(label,rows):
 av=[r['pb1_to_v4_ms']/1000 for r in rows if r['pb1_to_v4_ms'] is not None]
 lv=[r['logical_to_v4_ms']/1000 for r in rows if r['logical_to_v4_ms'] is not None]
 fb=[r['logical_to_v2_ms']/1000 for r in rows]
 wave=[r['pb1_to_wave_us']/1e6 for r in rows if r['pb1_to_wave_us'] is not None]
 wavevis=[r['wave_to_v4_ms']/1000 for r in rows if r['wave_to_v4_ms'] is not None]
 t0pb1=[r['logical_to_pb1_us']/1e6 for r in rows]
 pb1v1=[r['pb1_to_v1_us']/1e6 for r in rows]
 v1v2=[r['v1_to_v2_us']/1e6 for r in rows]
 ints=[rows[i]['t2_pb1_s']-rows[i-1]['t2_pb1_s'] for i in range(1,len(rows))]
 return {'label':label,'events':len(rows),'pb1_to_complete_visible':stats(av),'logical_to_complete_visible':stats(lv),'logical_to_framebuffer_complete':stats(fb),'pb1_to_waveform':stats(wave),'waveform_to_complete_visible':stats(wavevis),'logical_to_pb1':stats(t0pb1),'pb1_to_first_framebuffer_write':stats(pb1v1),'framebuffer_write_span':stats(v1v2),'heartbeat_intervals':stats(ints),'partial_frame_events':sum(r['partial_frames']>0 for r in rows),'partial_frames':sum(r['partial_frames'] for r in rows),'missing_complete':sum(r['v4_s'] is None for r in rows),'audio_measured':len(wave),'audio_ambiguous':len(rows)-len(wave),'busy_events':sum(r['busy_writes_pm1']>0 for r in rows),'max_busy_writes_pm1':max((r['busy_writes_pm1'] for r in rows),default=0)}

idle=analyze('idle');stress=analyze('stress')
# Exclude first two transitions after presentation initialization; select 100 consecutive natural no-input events.
idle100=idle[2:102]
stress100=stress[:100]
pre=[r for r in stress if 68<=r['t2_pb1_s']<75.6641]
combat=[r for r in stress if 73.95<=r['t2_pb1_s']<77.733]
accel=[r for r in stress if r['heartr']==41 and 77.733<=r['t2_pb1_s']<112]
rate46=[r for r in idle100 if r['heartr']==46]
summary={'visual_phase_hashes':{'large':__import__('hashlib').sha256(snapshot_phases(ROOT/'idle')[3-3] if False else b'').hexdigest() if False else 'see per-run PNG crop hashes e5750a0a9ca6/6218df7b8da8'},'frame_period_ms':FRAME_PERIOD*1000,'video_hz':1/FRAME_PERIOD,'idle100':summarize('100 consecutive natural idle',idle100),'stress100':summarize('first 100 autoplay events',stress100),'precombat':summarize('pre-combat',pre),'combat':summarize('combat overlap',combat),'accelerated':summarize('post-attack HEARTR=41',accel),'all_idle_valid':summarize('all idle valid',idle),'all_stress_valid':summarize('all stress valid',stress)}
(ROOT/'summary.json').write_text(json.dumps(summary,indent=2))
# tearing examples and a compact verification
tears=[r for r in stress if r['partial_frames']]
(ROOT/'tearing-examples.json').write_text(json.dumps(tears[:20],indent=2))
print(json.dumps(summary,indent=2))
print('stress tears',[(r['event_id'],r['t2_pb1_s'],r['partial_frames'],r['partial_rows']) for r in tears[:20]])
