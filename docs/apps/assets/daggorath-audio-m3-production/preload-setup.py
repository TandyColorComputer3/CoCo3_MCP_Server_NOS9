from pathlib import Path
import subprocess,json,sys,time
w=Path(__file__).resolve().parent

def call(n,a):
 r=json.loads(subprocess.check_output(['node',str(w/'client.mjs'),n,json.dumps(a)],text=True));return r.get('structuredContent',r)
def run(cmd):
 r=call('os9_run',{'command':cmd,'timeout_ms':90000,'allow_graphics':True});print(cmd,json.dumps(r),flush=True);return r
if sys.argv[1]=='init':
 print(call('coco_save_state',{'name':'hb_ready'}),flush=True);print(call('os9_restore_ready',{}),flush=True)
 for f in ['dhbpack','hbtest','dodsnd','dodwiz']:run('load /d1/'+f)
elif sys.argv[1]=='case':
 name=sys.argv[2];command=' '.join(sys.argv[3:]);begin=len((w/'ticks.csv').read_text().splitlines());r=run(command);end=len((w/'ticks.csv').read_text().splitlines());(w/(name+'.json')).write_text(json.dumps({'command':command,'response':r,'startLine':begin,'endLine':end},indent=2));call('coco_snapshot',{});(w/(name+'.png')).write_bytes((w/'latest.png').read_bytes())
else:run(' '.join(sys.argv[1:]))
