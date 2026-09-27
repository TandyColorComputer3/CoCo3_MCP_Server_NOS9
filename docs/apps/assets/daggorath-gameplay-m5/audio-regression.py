from pathlib import Path
import subprocess,json,time
w=Path('MCP/work/gameplay-m5').resolve()
def call(n,a):
 r=json.loads(subprocess.check_output(['node',str(w/'client.mjs'),n,json.dumps(a)],text=True));return r.get('structuredContent',r)
records=[]
for name,command in [('squeak','hbtest squeak'),('whoop','hbtest whoop'),('phaser','hbtest phaser'),('wizard','dodwiz')]:
 # Begin after observed clock rollover, with ample room for the bounded trial.
 deadline=time.monotonic()+70
 while True:
  with (w/'frames.csv').open('rb') as f:
   f.seek(-min(4096,f.seek(0,2)),2);last=f.read().decode().splitlines()[-1].split(',')
  if 10<=int(last[6])<=15:break
  assert time.monotonic()<deadline,'clock window not observed'
  time.sleep(.1)
 begin=len((w/'ticks.csv').read_text().splitlines());r=call('os9_run',{'command':command,'timeout_ms':90000,'allow_graphics':True});end=len((w/'ticks.csv').read_text().splitlines());print(name,json.dumps(r),flush=True)
 x={'case':name,'response':r,'startLine':begin,'endLine':end};records.append(x);(w/'audio-regression.json').write_text(json.dumps(records,indent=2)+'\n');assert r.get('status')==0 and r.get('completed') and r.get('shellReady'),r
 call('coco_snapshot',{});(w/(name+'-regression.png')).write_bytes((w/'latest.png').read_bytes())
 for cmd in ['date','pwd']:
  r=call('os9_run',{'command':cmd,'timeout_ms':30000});records.append(r);assert r.get('status')==0,r
 assert len((w/'ticks.csv').read_text().splitlines())==end,'callbacks after teardown'
(w/'audio-regression.json').write_text(json.dumps(records,indent=2)+'\n')
