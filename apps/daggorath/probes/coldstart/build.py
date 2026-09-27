#!/usr/bin/env python3
"""Disposable copies of exact Wizard operations, with RAM-only phase markers.
No production file is rewritten. See generated sources and provenance hashes.
"""
from pathlib import Path
import subprocess,json,hashlib,sys
root=Path(__file__).resolve().parents[4];app=root/'apps/daggorath';here=Path(__file__).parent
out=Path(sys.argv[1]).resolve();out.mkdir(parents=True,exist_ok=True)
sys.path.insert(0,str(app));from prepare_frames import generate
generate(app,out)
s=(app/'src/presentation.c').read_text();s='extern unsigned char limit; void phase(unsigned char);\n'+s
s=s.replace('e=window_open(&fd);if(e)return e;','phase(10);e=window_open(&fd);phase(20);if(e)return e;if(limit==1)return 0;')
s=s.replace('e=os_write(fd,setup,sizeof(setup));if(e)return e;','phase(11);e=os_write(fd,setup,sizeof(setup));phase(21);if(e)return e;if(limit==2)return 0;')
s=s.replace('e=os_write(fd,define,sizeof(define));if(e)return e;owned=1;','phase(12);e=os_write(fd,define,sizeof(define));phase(22);if(e)return e;owned=1;if(limit==3)return 0;')
s=s.replace('e=os_write(fd,capture,sizeof(capture));if(e)return e;','phase(13);e=os_write(fd,capture,sizeof(capture));phase(23);if(e)return e;if(limit==4)return 0;')
s=s.replace('e=os_map_buffer(fd,0xc401,1,&pixels,&mappedLength);','phase(14);e=os_map_buffer(fd,0xc401,1,&pixels,&mappedLength);phase(24);')
s=s.replace('return os_write(fd,selectWindow,sizeof(selectWindow));','phase(15);e=os_write(fd,selectWindow,sizeof(selectWindow));phase(25);return e;')
(out/'presentation.c').write_text(s)
s=(app/'src/main.c').read_text().replace('int main(', 'int wizard_main(');s='void phase(unsigned char);\n'+s
s=s.replace('playback_frame(frame,fade,messages);','phase(28);playback_frame(frame,fade,messages);phase(29);')
s=s.replace('error=screen_prepare(frame);','phase(30);error=screen_prepare(frame);phase(31);')
s=s.replace('error=screen_flip();','phase(32);error=screen_flip();phase(33);')
s=s.replace('sound_event(0);cleanup=screen_close();','phase(34);sound_event(0);cleanup=screen_close();phase(35);')
(out/'wizard-main.c').write_text(s)
commands=[];ids={}
for name,files in [('startprobe',[here/'probe.c',out/'wizard-main.c',out/'presentation.c',app/'src/playback.c',app/'src/original/logical.c',app/'src/window-path.c']),('vgap',[here/'controller.c',app/'src/audio/ipc.c']),('vready',[here/'enable.c',out/'presentation.c',app/'src/playback.c',app/'src/original/logical.c',app/'src/window-path.c',app/'src/audio/ipc.c'])]:
 module=out/(name+'-module.asm');module.write_text(' section __os9\n fcc /'+name+'/\n fcb 0\nedition equ 1\nrev equ 1\n endsection\n')
 cmd=['cmoc','--os9','-O0','--intermediate','--intdir='+str(out),'--add-os9-stack-space=2048','-I'+str(app/'src'),'-I'+str(app/'src/audio'),'-I'+str(out),'-o',str(out/name)]+list(map(str,files+[here/'phase.c',app/'src/os9.c',module]))
 commands.append(cmd);subprocess.run(cmd,check=True)
 ids[name]=subprocess.check_output(['/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9','ident',str(out/name)],text=True)
(out/'build.json').write_text(json.dumps({'commands':commands,'ident':ids,'inputs':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in list(here.glob('*.*'))+list(app.glob('src/*.c'))},'outputs':{n:hashlib.sha256((out/n).read_bytes()).hexdigest() for n in ids}},indent=2))
print(ids)
