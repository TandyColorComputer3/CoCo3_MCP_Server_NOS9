/* NitrOS-9 f470fa52: defs/os9.d; kernel ffork/fwait/fsend; PipeMan Read.
 * F$Fork inherits only paths 0..2. I$Dup chooses lowest free process path.
 * F$Wait returns child status in B; carry distinguishes syscall error. */
#include <cmoc.h>
#include "ipc.h"
Byte ipc_open(const char *name,Byte *path){Byte e,n;asm{
 ldx :name
 lda #3
 os9 $84
 bcs @bad
 sta :n
 clrb
@bad
 stb :e
 }if(!e)*path=n;return e;}
Byte ipc_close(Byte path){Byte e;asm{
 lda :path
 os9 $8f
 bcs @bad
 clrb
@bad
 stb :e
 }return e;}
Byte ipc_dup(Byte path,Byte *copy){Byte e,n;asm{
 lda :path
 os9 $82
 bcs @bad
 sta :n
 clrb
@bad
 stb :e
 }if(!e)*copy=n;return e;}
Byte ipc_read(Byte path,Byte *data,Word count){Byte e;Word n;
 while(count){asm{
 pshs y
 lda :path
 ldx :data
 ldy :count
 os9 $89
 sty :n
 bcs @bad
 clrb
@bad
 puls y
 stb :e
 }if(e)return e;if(!n||n>count)return AUDIO_IO;data+=n;count-=n;}
 return 0;}
Byte ipc_fork(const char *name,const char *args,Byte *pid){Word length=strlen(args);Byte e,n;
 asm{
 pshs y,u
 ldx :name
 ldy :length
 ldu :args
 lda #$11
 clrb
 os9 $03
 bcs @bad
 clrb
@bad
 puls u,y
 sta :n
 stb :e
 }if(!e)*pid=n;return e;}
Byte ipc_wait(Byte *pid,Byte *status){Byte e,n,s;asm{
 os9 $04
 sta :n
 stb :s
 bcs @bad
 clrb
@bad
 stb :e
 }if(!e){*pid=n;*status=s;}return e;}
Byte ipc_signal(Byte pid,Byte signal){Byte e;asm{
 lda :pid
 ldb :signal
 os9 $08
 bcs @bad
 clrb
@bad
 stb :e
 }return e;}
/* NitrOS-9 fid.asm: F$ID ($0c) returns this process ID in A. */
Byte ipc_self_pid(Byte *pid){Byte e,n;asm{
 pshs y
 os9 $0c
 bcs @bad
 sta :n
 clrb
@bad
 puls y
 stb :e
 }if(!e)*pid=n;return e;}
