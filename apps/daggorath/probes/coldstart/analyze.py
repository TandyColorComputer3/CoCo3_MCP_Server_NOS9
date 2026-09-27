#!/usr/bin/env python3
"""Every >25 ms interval retained; PC/CC/HALT correlation is sampled, not inferred."""
import csv,json,sys,collections
from pathlib import Path
w=Path(sys.argv[1]);period=.0166881595
out={}
for file in sorted(w.glob('*.json')):
 rec=json.loads(file.read_text())
 if 'startLine' not in rec:continue
 lines=(w/rec.get('trace','trace-first.csv')).read_text().splitlines();names=lines[0].split(',')
 rows=list(csv.DictReader(lines[rec['startLine']:rec['endLine']],fieldnames=names));c=[r for r in rows if r['kind']=='callback'];ph=[r for r in rows if r['kind']=='phase'];gaps=[];intervals=[]
 if not c:continue
 first=float(c[0]['time']);lastphase=None
 for a,b in zip(c,c[1:]):
  start=float(a['time']);end=float(b['time']);dt=end-start;intervals.append(dt)
  if dt<=.025:continue
  inner=[r for r in rows if start<float(r['time'])<end];f=[r for r in inner if r['kind']=='frame'];preceding=[r for r in ph if float(r['time'])<=start]
  gaps.append({'start':start,'relativeStart':start-first,'end':end,'ms':dt*1000,'phase':int(preceding[-1]['value']) if preceding else None,'frames':len(f),'irqMaskedFrames':sum(bool(int(r['cc'])&16) for r in f),'irqPendingFrames':sum(int(r['irq'])!=0 for r in f),'haltFrames':sum(bool(int(r['suspend'])&1) for r in f),'fdcCommands':[{'time':float(r['time']),'command':int(r['value']),'pc':hex(int(r['pc']))} for r in inner if r['kind']=='fdc-command'],'sampledPCs':dict(collections.Counter(hex(int(r['pc'])) for r in f))})
 out[file.stem]={'command':rec['command'],'response':rec['response'],'callbacks':len(c),'expectedTicks':round((float(c[-1]['time'])-first)/period)+1,'minMs':min(intervals)*1000,'maxMs':max(intervals)*1000,'counterDiscontinuities':sum(int(b['value'])!=((int(a['value'])+1)&65535) for a,b in zip(c,c[1:])),'gaps':gaps,'phases':[{'phase':int(r['value']),'time':float(r['time']),'relative':float(r['time'])-first,'sec':int(r['syssec']),'tick':int(r['systick'])} for r in ph]}
(w/'analysis.json').write_text(json.dumps(out,indent=2)+'\n')
for k,v in out.items():print(k,v['callbacks'],v['expectedTicks'],round(v['maxMs'],3),'gaps',len(v['gaps']),'phases',collections.Counter(g['phase'] for g in v['gaps']))
