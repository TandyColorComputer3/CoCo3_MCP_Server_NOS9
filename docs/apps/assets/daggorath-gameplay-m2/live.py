from pathlib import Path
import urllib.request,json,time,threading,base64,sys,io,ctypes
from PIL import Image
w=Path(__file__).resolve().parent;name=sys.argv[1];records=[];response={};failure=[]
lib=ctypes.CDLL(str(w/'game.so'));game=ctypes.create_string_buffer(8192);frame=ctypes.create_string_buffer(6144);lib.game_init(game,0)
message=b'TURN LEFT RIGHT AROUND  MOVE'
def call(tool,args):
 with urllib.request.urlopen(urllib.request.Request('http://127.0.0.1:5992',data=json.dumps({'name':tool,'arguments':args}).encode()),timeout=160) as r:return json.load(r)
def snap():
 r=call('coco_snapshot',{});return base64.b64decode(next(c['data'] for c in r['content'] if c['type']=='image'))
def state():
 try:return json.loads((w/'ready.json').read_text()),(w/'ready.json').stat().st_mtime_ns
 except (FileNotFoundError,json.JSONDecodeError):return {},0
start=time.monotonic();begin=len((w/'ticks.csv').read_text().splitlines());tracebegin=len((w/'input-trace.jsonl').read_text().splitlines())
def wait(line,fields={},after=0):
 while time.monotonic()-start<115:
  s,m=state()
  if m>after and s.get('phase')=='ready' and s.get('dirty')==0 and s.get('error')==0 and s.get('key')==0 and s.get('empty') and not s.get('posting') and s.get('input')==line and s.get('n')==len(line) and all(s.get(k)==v for k,v in fields.items()):return s
  if failure:raise RuntimeError(failure)
  if response:raise RuntimeError('application returned before boundary '+json.dumps(response))
  time.sleep(.01)
 raise RuntimeError('unchanged operation deadline approaching; boundary '+json.dumps({'line':line,'fields':fields,'last':state()}))
def check_frame(line,label):
 lib.game_render(game,frame,line.encode(),message);target=bytearray()
 for b in frame.raw:
  for i in range(7,-1,-1):target.extend([255 if b&(1<<i) else 0]*2)
 png=snap();im=Image.open(io.BytesIO(png)).convert('L');view=im.crop((64,26,576,218)).point(lambda p:255 if p>128 else 0).tobytes()
 matched=[]
 for phase in [0,1]:
  lib.game_render_status(game,frame,phase);expected=bytearray()
  for b in frame.raw:
   for i in range(7,-1,-1):expected.extend([255 if b&(1<<i) else 0]*2)
  if view==bytes(expected):matched.append(phase)
 assert len(matched)==1,{'label':label,'pixelDiff':sum(a!=b for a,b in zip(view,expected))}
 records.append({'frame':label,'phase':matched[0],'exactPixels':True})
 if label:(w/(name+'-'+label+'.png')).write_bytes(png)
def post(text):
 s,m=state();assert s.get('empty') and not s.get('posting'),s
 (w/'keyboard.txt').write_text(text);return m
def input_line(cmd):
 t=time.monotonic();m=post(cmd);s=wait(cmd,after=m);records.append({'input':cmd,'state':s,'wallSeconds':time.monotonic()-t});return s
def command(cmd,fields,label=None):
 global message
 input_line(cmd);check_frame(cmd,None)
 result=lib.game_command(game,cmd.encode());message=b'BLOCKED' if result==1 else b'UNKNOWN COMMAND' if result==2 else b'OK'
 m=post('{ENTER}');s=wait('',fields,after=m);check_frame('',label);records.append({'command':cmd,'result':result,'state':s});print(cmd,json.dumps(s),flush=True)
def clear():
 s,_=state()
 while s.get('n'):
  n=s['n'];m=post('{BS}');s=wait(s['input'][:-1],after=m);assert s['n']==n-1
try:(w/'ready.json').unlink()
except FileNotFoundError:pass
def run():
 try:response.update(call('os9_run',{'command':('dognohb' if name=='E' else 'dodgame')+' seed0','timeout_ms':120000,'allow_graphics':True}))
 except Exception as e:failure.append(str(e))
t=threading.Thread(target=run);t.start()
try:
 s=wait('',{'row':16,'col':11,'dir':0,'lit':0});records.append({'initial':s});check_frame('','initial-dark')
 if name not in ['repeat','stress-dark','D']:
  command('PULL LEFT TORCH',{'hand':0xe95,'torch':0,'lit':0},'left-hand')
  if name!='A':command('USE LEFT',{'hand':0,'torch':0xe95,'lit':1},'initial-lit')
  if name=='C':command('USE LEFT',{'hand':0,'torch':0xe95,'lit':1},'repeated-use')
 if name=='normal':
  command('PULL RIGHT TORCH',{'hand':0,'rightHand':0xe95,'torch':0,'lit':0},'right-hand')
  command('USE RIGHT',{'hand':0,'rightHand':0,'torch':0xe95,'lit':1},'right-used')
 if name.startswith('stress'):
  for line in ['MOVE','MOVE','MOVE','TORCH','MOVE','MMMM','EEEE','MOVEE']:
   input_line(line);check_frame(line,None);clear()
 elif name=='cancel':post('@SHIFT_BREAK@')
 elif name in ['normal','D','cold-normal']:
  command('MOVE',{'row':15,'col':11,'dir':0},'moved')
  command('TURN RIGHT',{'row':15,'col':11,'dir':1},'turned')
  command('MOVE',{'row':15,'col':11,'dir':1},'blocked')
  command('TURN LEFT',{'row':15,'col':11,'dir':0},'north-again')
 if name!='cancel':input_line('EXIT');m=post('{ENTER}')
 t.join(timeout=max(1,130-(time.monotonic()-start)));assert not t.is_alive(),'MCP did not return';assert not failure,failure
 r=response.get('structuredContent',{});print(json.dumps(r),flush=True);assert r.get('completed') and r.get('shellReady') and r.get('status')==(3 if name=='cancel' else 0),r
 (w/(name+'-term.png')).write_bytes(snap())
 # Observe callback count through two subsequent strict commands, not a sleep.
 teardown=len((w/'ticks.csv').read_text().splitlines())
 health=[]
 for cmd in ['date','pwd']:
  q=call('os9_run',{'command':cmd,'timeout_ms':30000});health.append(q);assert q.get('structuredContent',{}).get('status')==0,q
 assert len((w/'ticks.csv').read_text().splitlines())==teardown,'callbacks after teardown'
 (w/(name+'-health.json')).write_text(json.dumps(health,indent=2)+'\n')
finally:
 (w/(name+'-response.json')).write_text(json.dumps(response,indent=2)+'\n');(w/(name+'-states.json')).write_text(json.dumps(records,indent=2)+'\n');(w/(name+'-range.json')).write_text(json.dumps({'startLine':begin,'endLine':len((w/'ticks.csv').read_text().splitlines()),'wallSeconds':time.monotonic()-start,'traceBegin':tracebegin,'traceEnd':len((w/'input-trace.jsonl').read_text().splitlines())})+'\n')
