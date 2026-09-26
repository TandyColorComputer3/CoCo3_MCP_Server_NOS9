import fs from 'node:fs';
const dir='MCP/work/audio-187';
async function call(name,args={}){const q={name,arguments:args};const r=await(await fetch('http://127.0.0.1:5971',{method:'POST',body:JSON.stringify(q)})).json();fs.appendFileSync(dir+'/reproduction.jsonl',JSON.stringify({q,r})+'\n');return r;}
const deadline=Date.now()+75000;
for(;;){const line=fs.readFileSync(dir+'/clock-trace.csv','utf8').trim().split('\n').at(-1);if(Number(line.split(',').at(-2))===40){console.log('checkpoint',line);fs.writeFileSync(dir+'/checkpoint-time.txt',line+'\n');break;}if(Date.now()>deadline)throw Error('phase missing');await new Promise(r=>setTimeout(r,100));}
console.log(await call('coco_save_state',{name:'audio_mt_ready'}));
// Deliberate experimental offset, not a completion detector. An auxiliary private
// save invokes MAME RTC pre-save refresh; neither guest time nor hardware is written.
await new Promise(r=>setTimeout(r,25000));
console.log(await call('coco_save_state',{name:'audio187_rtc_advanced'}));
for(const mode of ['gfxtone','gfxidle']){
 const ready=await call('os9_restore_ready');if(!ready.structuredContent?.ready)throw Error('not ready');
 const result=await call('os9_run',{command:'audmt '+mode,allow_graphics:true,timeout_ms:45000});console.log(mode,result.structuredContent);
 const snap=await call('coco_snapshot');fs.writeFileSync(dir+'/repro-'+mode+'.png',Buffer.from(snap.content.find(c=>c.type==='image').data,'base64'));
}
console.log((await call('os9_run',{command:'date',timeout_ms:20000})).structuredContent);
