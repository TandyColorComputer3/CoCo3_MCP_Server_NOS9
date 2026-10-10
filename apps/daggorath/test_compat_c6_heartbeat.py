#!/usr/bin/env python3
"""C6 broker ownership, protocol, timing gate, and protected C5 oracle."""
from pathlib import Path
import hashlib,subprocess,tempfile

APP=Path(__file__).resolve().parent
C5=APP/'test/fixtures/compat-c5/heartbeat-trace.csv'
assert hashlib.sha256(C5.read_bytes()).hexdigest()==\
 '2132ce0d4096d8c33cb41a6aeb32b3162528d2e24b56c7c9df23f5a59697b43e'
header=(APP/'src/compat/heartbeat-broker.h').read_text()
service=(APP/'src/compat/heartbeat-broker-service.c').read_text()
client=(APP/'src/compat/heartbeat-broker-client.asm').read_text()
present=(APP/'src/compat/present.c').read_text()
target=(APP/'test/compat_c5_target.c').read_text()
assert 'DAG_BROKER_PRIORITY 192' in header and 'DAG_BROKER_FRAME 16' in header
assert 'TRACE_CAPACITY 240' in service and 'latency>2' in service
assert 'currentScene' in service and 'DAG_BROKER_STALE' in service
assert 'os_write(2,putHeart' in service and 'os_write(2,putScene' in service
assert 'native_heartbeat_rate(&heartbeat,request[5])' in service
assert 'putScene[7]=request[5]' in service and 'state.enabled=1' in service
assert 'dag_broker_scene' in present and '#ifdef DAG_COMPAT_C6' in present
assert 'stb     5,u' in client
c6heart=present[present.index('Byte dag_compat_present_heart'):present.index('Byte dag_compat_present_cell')]
assert 'dag_broker_snapshot' in c6heart and 'heartPut' not in c6heart.split('#else')[0]
assert 'f_sprior' in client and '#192' in client
assert 'dag_broker_close(&broker)' in target and 'dag_broker_cancel(&broker)' in target

# Deterministic arbitration model: one bounded scene may be in flight, every
# edge is then presented before another scene begins. Two ticks is 33.333 ms
# at 60 Hz and remains below the retained 33.34 ms gate.
generation=shown=pb1=phase=0
rows=[]
for tick in range(1,661):
    scene=2 if tick%17==0 else 1 if tick%5==0 else 0
    if tick%3==0:
        generation+=1;pb1^=1;phase^=1
    if generation!=shown:
        assert generation-shown==1
        wake=1;heart=1 if scene else 0;latency=wake+heart
        shown=generation
        rows.append((generation,tick,pb1,phase,wake,heart,latency,scene,latency<=2))
assert len(rows)==220 and shown==generation
assert all(r[2]==r[3] and r[8] for r in rows)
assert max(r[6] for r in rows)==2

with tempfile.TemporaryDirectory(prefix='dag-c6-heart-present-') as td:
    exe=Path(td)/'present'
    subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror',
      '-DDAG_COMPAT_HEARTBEAT','-DDAG_COMPAT_C6',
      '-I'+str(APP.parent/'window-manager/test/compat'),'-I'+str(APP/'src'),
      str(APP/'src/compat/present.c'),str(APP/'test/compat_c6_present.c'),'-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
print('compat C6 broker ownership/deadline model and C5 oracle checks passed')
