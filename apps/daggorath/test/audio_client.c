#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ipc.h"
static Byte paths[16],queue[64],qsize,qpos,last[8],forkerror,waitstatus,signalno;
static void reset(void){memset(paths,0,16);paths[0]=10;paths[1]=11;paths[2]=12;qsize=qpos=forkerror=waitstatus=signalno=0;}
static void reply(Byte op,Byte seq,Byte status){Byte f[8];audio_frame(f,op,0,status,seq);memcpy(queue+qsize,f,8);qsize+=8;}
Byte ipc_open(const char *s,Byte *p){assert(!strcmp(s,"/pipe"));for(Byte i=3;i<16;i++)if(!paths[i]){paths[i]=20+i;*p=i;return 0;}return 200;}
Byte ipc_dup(Byte p,Byte *q){assert(paths[p]);for(Byte i=0;i<16;i++)if(!paths[i]){paths[i]=paths[p];*q=i;return 0;}return 200;}
Byte ipc_close(Byte p){assert(p<16);paths[p]=0;return 0;}
Byte ipc_fork(const char *s,const char *a,Byte *pid){assert(!strcmp(s,"/d1/dodaudio"));assert(!strcmp(a,"ssc-mame-fast\r"));assert(paths[0]==23&&paths[1]==24&&paths[2]==12);if(forkerror)return forkerror;*pid=7;return 0;}
Byte ipc_read(Byte p,Byte *d,Word n){assert(p==4&&n==8);if(qpos==qsize)return AUDIO_EOF;memcpy(d,queue+qpos,8);qpos+=8;return 0;}
Byte os_write(Byte p,const void *d,Word n){assert(p==3&&n==8);memcpy(last,d,8);return 0;}
Byte ipc_wait(Byte *p,Byte *s){*p=7;*s=waitstatus;return 0;}
Byte ipc_signal(Byte p,Byte s){assert(p==7);signalno=s;return 0;}
int main(void){AudioClient c;int n=0;
 reset();reply(0,0,0);assert(!audio_start(&c,"/d1/dodaudio","ssc-mame-fast"));assert(paths[0]==10&&paths[1]==11);++n;
 assert(!audio_submit(&c,AUDIO_PLAY,0,255));assert(last[2]==AUDIO_PLAY&&last[5]==1);++n;
 assert(audio_submit(&c,AUDIO_STOP,0,0)==AUDIO_BUSY);++n;
 reply(AUDIO_PLAY,1,0);assert(!audio_receive(&c));++n;
 reply(AUDIO_SHUTDOWN,2,0);assert(!audio_finish(&c));assert(!paths[3]&&!paths[4]&&!c.opened);++n;
 reset();forkerror=AUDIO_BUSY;assert(audio_start(&c,"/d1/dodaudio","ssc-mame-fast")==AUDIO_BUSY);assert(paths[0]==10&&paths[1]==11&&!paths[3]&&!paths[4]);++n;
 reset();reply(0,0,0);assert(!audio_start(&c,"/d1/dodaudio","ssc-mame-fast"));waitstatus=3;assert(audio_cancel(&c)==3&&signalno==3&&!c.opened);++n;
 reset();reply(0,0,0);assert(!audio_start(&c,"/d1/dodaudio","ssc-mame-fast"));assert(audio_submit(&c,AUDIO_PLAY,1,255)==AUDIO_UNSUPPORTED&&!c.pending);++n;
 assert(!audio_submit(&c,AUDIO_PLAY,0,255));reply(AUDIO_PLAY,99,0);assert(audio_receive(&c)==AUDIO_BAD);++n;
 waitstatus=AUDIO_BAD;assert(audio_finish(&c)==AUDIO_EOF&&!c.opened&&signalno==3);++n;
 printf("%d audio client/IPC lifecycle checks passed\n",n);return 0;}
