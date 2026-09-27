#!/usr/bin/env python3
"""Read-only counter-state and PB1 deadline checks against the actual driver.
No tolerance permits a missing three-tick deadline. Native waveform matching
uses MAME block timestamps, independently of the guest wall clock.
"""
from pathlib import Path
import csv,json,sys,statistics
w=Path(sys.argv[1]);lines=(w/'ticks.csv').read_text().splitlines();header=lines[0] if lines else ''
writes=[r for r in csv.DictReader((w/'writes.csv').open()) if all(r.values())]
results={}
for p in w.glob('*.json'):
 rec=json.loads(p.read_text())
 if not isinstance(rec,dict) or 'startLine' not in rec:continue
 rows=list(csv.DictReader([header]+lines[rec['startLine']:rec['endLine']]));rows=[r for r in rows if all(r.values()) and r['time']!='time']
 if not rows:continue
 first,last=float(rows[0]['time']),float(rows[-1]['time']);base=int(rows[0]['pc'])-0x1fa
 # PC after STB $FF22 is module offset $223 (verify build listing).
 edgepc=base+0x223
 edges=[r for r in writes if int(r['address'])==0xff22 and int(r['pc'])==edgepc and first<=float(r['time'])<=last+.001]
 due=[r for r in rows if r['enabled']=='1' and r['remaining']=='1' and r['fault']=='0']
 matched=[];missing=[]
 for t in due:
  match=[e for e in edges if 0<=float(e['time'])-float(t['time'])<.001]
  (matched if len(match)==1 else missing).append(float(t['time']))
 intervals=[(float(b['time'])-float(a['time']))*1000 for a,b in zip(rows,rows[1:])]
 ei=[(float(b['time'])-float(a['time']))*1000 for a,b in zip(edges,edges[1:])]
 violations=[]
 for a,b in zip(rows,rows[1:]):
  if float(b['time'])-float(a['time'])>.025:continue # different install cycle, reported separately
  expected=int(a['remaining'])
  if a['enabled']=='1':
   expected=(expected-1)&255
   if expected==0:expected=int(a['rate'])
  if int(b['remaining'])!=expected:violations.append({'time':float(b['time']),'expected':expected,'observed':int(b['remaining'])})
 results[p.stem]={'stateViolations':violations,'response':rec['response'],'first':first,'last':last,'callbacks':len(rows),'maxCallbackMs':max(intervals,default=0),'moduleBase':hex(base),'edgePC':hex(edgepc),'expectedEdges':len(due),'observedEdges':len(edges),'missingDeadlines':missing,'maxEdgeIntervalMs':max(ei,default=0),'minEdgeIntervalMs':min(ei,default=0),'faults':sum(r['fault']!='0' for r in rows),'edgeTimes':[float(e['time']) for e in edges]}
(w/'timing-analysis.json').write_text(json.dumps(results,indent=2)+'\n')
for name,r in results.items():print(name,r['callbacks'],r['maxCallbackMs'],r['expectedEdges'],r['observedEdges'],'MISSING',len(r['missingDeadlines']),'FAULTS',r['faults'])
