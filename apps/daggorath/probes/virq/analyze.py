#!/usr/bin/env python3
"""Analyze callback records in emulated time; never treat sleeps as timing proof."""
import pathlib,json,sys,statistics
w=pathlib.Path(sys.argv[1]);lines=(w/'callbacks.csv').read_text().splitlines();results={}
for p in w.glob('*.json'):
 try:r=json.loads(p.read_text())
 except Exception:continue
 if not isinstance(r,dict) or 'traceStartLine' not in r:continue
 rows=[s.split(',') for s in lines[r['traceStartLine']:r['traceEndLine']] if s and not s.startswith('#')]
 if rows:
  t=[float(x[0]) for x in rows];f=[int(x[1]) for x in rows];c=[int(x[4]) for x in rows]
  dt=[(b-a)*1000 for a,b in zip(t,t[1:])];df=[b-a for a,b in zip(f,f[1:])]
  results[p.stem]={'callbacks':len(rows),'first_count':c[0],'last_count':c[-1],'frame_span':f[-1]-f[0]+1,'empty_frame_bins':sum(max(0,d-1) for d in df),'multiple_callback_frame_bins':sum(d==0 for d in df),'expected_ticks_elapsed':round((t[-1]-t[0])/0.0166881595)+1,'missing_tick_intervals':sum(max(0,round(d/16.6881595)-1) for d in dt),'short_tick_intervals':sum(d<8.34407975 for d in dt),'counter_discontinuities':sum((b-a)%65536!=1 for a,b in zip(c,c[1:])),'interval_ms':{'min':min(dt),'median':statistics.median(dt),'max':max(dt)},'response':r['response']}
print(json.dumps(results,indent=2))
