#!/usr/bin/env python3
"""C6 production PipeMan/KICK transport contract checks."""
from pathlib import Path

APP=Path(__file__).resolve().parent
ipc=(APP/'src/audio/ipc.c').read_text()
service=(APP/'src/compat/heartbeat-broker-service.c').read_text()
client=(APP/'src/compat/heartbeat-broker-client.asm').read_text()
header=(APP/'src/compat/heartbeat-broker.h').read_text()
accepted=(APP/'experiments/heartbeat-wake/pipeman/ipc.c').read_text()

def function(source,name,next_name):
    start=source.index('Byte '+name+'(')
    end=source.index('Byte '+next_name+'(',start)
    return source[start:end]

# The production exact-length loops are the target-accepted PipeMan routines,
# including pointer advance, remaining-count reduction, raw error propagation,
# and zero/over-count rejection.
assert function(ipc,'ipc_read','ipc_write')==function(accepted,'ipc_read','ipc_write')
assert function(ipc,'ipc_write','ipc_fork')==function(accepted,'ipc_write','ipc_fork')
assert 'ipc_ready' not in service and 'ipc_ready' not in (APP/'src/audio/ipc.h').read_text()

# Broker authority is the driver's atomic request claim. A blocking read is
# reachable only after the claim reports pending work.
claim=service.index('native_heartbeat_claim_request')
pending=service.index('if(!pending)',claim)
read=service.index('ipc_read(0,request',pending)
assert claim<pending<read
assert service.index('service_heart()',service.index('while(!done)'))<claim
assert service.index('service_heart()',read)<service.index('reply(request[2]',read)

# Client commits the entire request, issues KICK through its own local /dhb
# path, and only then waits for the exact ACK.
exchange_start=client.index('exchange\n')
exchange=client[exchange_start:client.index('_dag_broker_rate',exchange_start)]
assert exchange.index('lbsr    write16')<exchange.index('os9     i_setstt')<exchange.index('lbsr    read16')
assert 'ldb     #$9a' in exchange and 'heartbeat fcc  "/dhb"' in client
assert 'Byte command,reply,pid,opened,sequence,phase,fault;' in header
assert 'traceEmpty,kick;' in header

# Every public C entry preserves CMOC resident Y with U on its single return.
for name in ('rate','snapshot','stats','trace','scene','start','close','cancel'):
    body=client[client.index('\n_dag_broker_'+name+'\n')+1:]
    body=body[:body.find('\n_dag_broker_',1) if body.find('\n_dag_broker_',1)>=0 else len(body)]
    assert 'pshs    y,u' in body
    assert 'puls    y,u,pc' in body or 'bra     cout' in body

print('compat C6 production exact PipeMan/KICK transport checks passed')
