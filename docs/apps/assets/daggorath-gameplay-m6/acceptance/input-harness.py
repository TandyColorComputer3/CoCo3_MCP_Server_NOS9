import json,time,threading,urllib.request,sys
from pathlib import Path
r=Path('/private/tmp/m6-boundary');mode=sys.argv[1];response={};fail=[]
def call(n,a):
 with urllib.request.urlopen(urllib.request.Request('http://127.0.0.1:5996',data=json.dumps({'name':n,'arguments':a}).encode()),timeout=130) as h:return json.load(h)
def run():
 try:response.update(call('os9_run',{'command':'dodgame seed0','allow_graphics':True,'timeout_ms':120000}))
 except Exception as e:fail.append(str(e))
try:(r/'ready.json').unlink()
except FileNotFoundError:pass
(r/'arm').touch();thread=threading.Thread(target=run);thread.start();start=time.monotonic()
def wait(line,after=0):
 while time.monotonic()-start<110:
  try:
   p=r/'ready.json';s=json.loads(p.read_text())
   if p.stat().st_mtime_ns>after and s['input']==line:return s,p.stat().st_mtime_ns
  except (FileNotFoundError,json.JSONDecodeError):pass
  if response or fail:raise RuntimeError({'response':response,'fail':fail})
  time.sleep(.02)
 raise RuntimeError('no verified input boundary by unchanged deadline')
try:
 s,stamp=wait('');print('READY',s,flush=True)
 if mode=='lit':
  for command in ['PULL LEFT TORCH','USE LEFT']:
   (r/'keyboard.txt').write_text(command);s,stamp=wait(command,stamp);print('COMMAND_BUFFERED',s,flush=True)
   (r/'keyboard.txt').write_text('{ENTER}');s,stamp=wait('',stamp);print('COMMAND_COMPLETE_READY',s,flush=True)
  snap=call('coco_snapshot',{});(r/'lit-snapshot-result.json').write_text(json.dumps(snap,indent=2))
 if mode in ['resident','lit']:
  target=None
  while time.monotonic()-start<100:
   ticks=[json.loads(l) for l in (r/'trace.jsonl').read_text().splitlines() if '"kind":"heartbeat_tick"' in l]
   if ticks:
    if target is None:target=ticks[-1]['tick']+600
    if ticks[-1]['tick']>=target:break
   time.sleep(.02)
  assert ticks[-1]['tick']>=target,'resident ticks did not progress'
  s,stamp=wait('',stamp)
 (r/'keyboard.txt').write_text('@SHIFT_BREAK@' if mode=='cancel' else 'EXIT')
 if mode!='cancel':s,stamp=wait('EXIT',stamp);print('EXIT_BUFFERED',s,flush=True);(r/'keyboard.txt').write_text('{ENTER}')
 thread.join(max(1,125-(time.monotonic()-start)))
 assert not thread.is_alive(), 'operation did not return'
 print(json.dumps(response),flush=True)
 out=response['structuredContent'];assert out['completed'] and out['shellReady'] and out['status']==(3 if mode=='cancel' else 0),out
 (r/(mode+'-response.json')).write_text(json.dumps(response,indent=2))
 for cmd in ['date','pwd']:
  h=call('os9_run',{'command':cmd,'timeout_ms':30000});print(cmd,h['structuredContent'],flush=True);assert h['structuredContent']['status']==0
 rows=[json.loads(l) for l in (r/'trace.jsonl').read_text().splitlines()]
 removals=[x for x in rows if x['kind']=='virq_removed']
 assert removals,'no observed VIRQ removal'
 after=[x for x in rows if x['kind']=='heartbeat_tick' and x['time']>removals[-1]['time']]
 assert not after,'callbacks observed after removal'
 print('ZERO_CALLBACKS_AFTER_REMOVAL',removals[-1],flush=True)
finally:
 (r/(mode+'-attempt.json')).write_text(json.dumps({'response':response,'failure':fail,'wall':time.monotonic()-start},indent=2))
