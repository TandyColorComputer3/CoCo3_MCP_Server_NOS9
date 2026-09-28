import json,collections,statistics,sys
from pathlib import Path
r=[json.loads(l)for l in Path(sys.argv[1]).read_text().splitlines()]
def unique(xs):
 out=[]
 for x in xs:
  if out and x.get('kind')==out[-1].get('kind')and x.get('name')==out[-1].get('name')and x['time']-out[-1]['time']<.000005:continue
  out.append(x)
 return out
starts=unique([x for x in r if x['kind']=='enter'and x['name']=='_native_heartbeat_enable']);runs=[]
for s in starts:
 remove=next(x for x in r if x['kind']=='virq_removed'and x['time']>s['time']);a=[x for x in r if s['time']<=x['time']<=remove['time']];ticks=[x for x in a if x['kind']=='heartbeat_tick'];edges=[x for x in a if x['kind']=='heartbeat_edge'];b=unique([x for x in a if x['kind']=='simulation_batch']);q=[x for x in a if x['kind']=='qten_boundary'];p=[x for x in a if x['kind']=='presented'];gaps=[1000*(y['time']-x['time'])for x,y in zip(ticks,ticks[1:])];expected=[x['tick']for x in ticks if x['enabled']and x['remaining']==1];actual=[x['tick']for x in edges];assert actual==expected,(actual,expected)
 phase=b[0]['phase'];expectedQ=sum(x['ticks']for x in b)//6;assert len(q)==expectedQ,(len(q),expectedQ)
 offset=edges[0]['edge']-1;present=[dict(x,actualGeneration=x['observedEdges']-offset,lag=x['observedEdges']-offset-x['generation'])for x in p]
 stages={}
 for name in ['_game_render','_screen_prepare','_screen_prepare_ui','_screen_flip','_native_heartbeat_close','_screen_close']:
  rows=unique([x for x in r if x['kind']in ['enter','leave']and x['name']==name and s['time']<=x['time']<remove['time']+1]);dur=[];enter=None
  for x in rows:
   if x['kind']=='enter':enter=x['time']
   elif enter is not None:dur.append(1000*(x['time']-enter));enter=None
  if dur:stages[name]={'count':len(dur),'minMs':min(dur),'maxMs':max(dur),'durationsMs':dur}
 result={'start':s['time'],'removed':remove['time'],'intervalSeconds':remove['time']-s['time'],'callbacks':len(ticks),'callbackGapMs':{'min':min(gaps),'max':max(gaps)},'faults':sum(x['fault']!=0 for x in ticks),'edges':len(edges),'edgeTickOffsets':[x-ticks[0]['tick']for x in actual],'edgesExactlyMatchCountdown':actual==expected,'fdcBusAccesses':sum(x['kind']=='active_fdc'and (x['address']==0xff40 or 0xff48<=x['address']<=0xff4b or 0xff58<=x['address']<=0xff5a or 0xff74<=x['address']<=0xff76)for x in a),'rtcBusAccesses':sum(x['kind']=='active_fdc'and x['address']in [0xff50,0xff51]for x in a),'simulationTicksConsumed':sum(x['ticks']for x in b),'simulationBatches':len(b),'maxBatchTicks':max(x['ticks']for x in b),'qTenBoundaries':len(q),'qTenWallGapMs':{'min':min(1000*(y['time']-x['time'])for x,y in zip(q,q[1:])),'max':max(1000*(y['time']-x['time'])for x,y in zip(q,q[1:]))},'sixTickAccountingMatches':len(q)==expectedQ,'maxPresentationLagObserved':max(x['lag']for x in present),'lastPresentation':present[-1],'stages':stages}
 runs.append(result)
print(json.dumps(runs,indent=2))
