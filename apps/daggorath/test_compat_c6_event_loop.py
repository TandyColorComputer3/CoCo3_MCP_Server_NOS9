#!/usr/bin/env python3
"""C6 production broker event-loop ordering contract."""
from pathlib import Path

APP=Path(__file__).resolve().parent
service=(APP/'src/compat/heartbeat-broker-service.c').read_text()
loop=service[service.index('while(!done)'):service.index('\ndone:',service.index('while(!done)'))]

# Normal request processing is authorized by an atomic claim, reconciles on
# both sides of its ACK, and never performs an unclaimed PipeMan read.
claim=loop.index('native_heartbeat_claim_request')
read=loop.index('ipc_read(0,request')
reply=loop.index('reply(request[2]')
assert claim < read < reply
assert loop[:claim].count('service_heart()') == 1
assert loop[read:reply].count('service_heart()') >= 1
assert loop[reply:].count('service_heart()') >= 1
assert 'ipc_ready' not in service

# Idle commitment follows the accepted authoritative ARM/recheck protocol.
arm=loop.index('native_heartbeat_arm')
generation_recheck=loop.index('service_heart()',arm)
request_recheck=loop.index('native_heartbeat_claim_request',generation_recheck)
sleep=loop.index('os_sleep(2)',request_recheck)
assert reply < arm < generation_recheck < request_recheck < sleep
assert 'os_sleep(0)' not in loop and 'os_sleep(1)' not in loop
assert loop.count('native_heartbeat_disarm') >= 3
assert 'claimed=1' in loop and 'if(claimed){pending=1;claimed=0;}' in loop

print('compat C6 event-driven broker ordering checks passed')
