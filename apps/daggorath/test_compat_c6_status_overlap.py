#!/usr/bin/env python3
"""C6 status scenes tolerate heartbeat advancement and restore the heart."""
from pathlib import Path

APP=Path(__file__).resolve().parent
service=(APP/'src/compat/heartbeat-broker-service.c').read_text()
scene=service[service.index('case DAG_BROKER_SCENE:'):service.index('case DAG_BROKER_SHUTDOWN:')]
restore=service[service.index('static Byte restore_status_heart'):service.index('static Byte reply')]

# Status validity follows the embedded generation/phase pair.  An ordinary
# heartbeat edge after the client's snapshot therefore does not make the
# semantic status update stale, while future or inconsistent pairs still do.
def valid(current_generation,current_phase,requested_generation,requested_phase):
    age=(current_generation-requested_generation)&0xffffffff
    return not age&0x80000000 and (requested_phase^current_phase)==(age&1)

assert valid(46,0,45,1)
assert valid(47,1,45,1)
assert not valid(45,1,46,0)       # future generation
assert not valid(46,0,45,0)       # phase/generation mismatch

assert 'age=state.edgeGeneration-requested' in scene
assert 'age&0x80000000UL' in scene
assert 'request[10]^state.phase' in scene
assert 'restore_status_heart()' in scene
assert scene.index('os_write(2,putScene') < scene.index('restore_status_heart()')
assert scene.index('restore_status_heart()') < service.index('reply(request[2]',service.index('case DAG_BROKER_SCENE:'))

# A restoration with no new edge issues the protected heart PutBlk but does
# not mutate generation accounting or claim a new heartbeat transition.
assert 'service_heart()' in restore and 'shownGeneration!=before' in restore
assert 'os_write(2,putHeart' in restore
assert 'shownGeneration=' not in restore
assert 'heartWrites++' not in restore

print('compat C6 status-overlap generation and broker heart restoration checks passed')
