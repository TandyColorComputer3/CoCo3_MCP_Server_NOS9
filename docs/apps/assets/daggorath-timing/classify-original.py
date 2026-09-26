import ctypes,subprocess,json
from pathlib import Path
p=Path('/private/tmp/daggorath-timing');subprocess.run(['cc','-shared','-fPIC','-Iapps/daggorath/src','apps/daggorath/src/original/logical.c','-o',str(p/'logical.dylib')],check=True);c=ctypes.CDLL(str(p/'logical.dylib'))
frames={}
for fade in list(range(0,33,2))+[255]:
 for msg in (0,1):
  a=(ctypes.c_ubyte*6144)();c.wizard_frame(a,fade,msg);frames[bytes(a)]=(fade,msg)
data=(p/'original/frames.bin').read_bytes();rows=(p/'original/timing.csv').read_text().splitlines();groups=[]
for i,row in enumerate(rows):
 label=frames.get(data[i*6144:(i+1)*6144]);fields=row.split(',')
 if label is not None:
  if groups and groups[-1]['label']==label and groups[-1]['last']==i:groups[-1]['last']=i+1
  else:groups.append({'label':label,'first':i+1,'last':i+1,'seconds':float(fields[1])})
print(json.dumps(groups,indent=2));(p/'original/groups.json').write_text(json.dumps(groups,indent=2))
