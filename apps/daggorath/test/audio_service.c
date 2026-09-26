#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ipc.h"
#include "backend.h"
int audio_service_main(int,char **);
static Byte input[64],output[64],pos,size,written,opened,closed,played,stopped,failopen,failplay,readerror,*signalptr;
static void reset(void){pos=size=written=opened=closed=played=stopped=failopen=failplay=readerror=0;}
static void queue(Byte op){audio_frame(input+size,op,op==AUDIO_PLAY?AUDIO_SQUEAK:0,op==AUDIO_PLAY?255:0,(size/8)+1);size+=8;}
Byte os_intercept(Byte *s){signalptr=s;*s=0;return 0;}
Byte os_signal_value(Byte *s){return *s;}
Byte backend_open(const char *p,Byte *s){assert(!strcmp(p,"ssc-mame-fast")&&s==signalptr);++opened;return failopen;}
Byte backend_close(void){++closed;return 0;}
Byte backend_play(Byte s,Byte g){assert(s==0&&g==255);++played;return failplay;}
Byte backend_stop(void){++stopped;return 0;}
Byte backend_drain(void){return 0;}
Byte ipc_read(Byte p,Byte *d,Word n){assert(p==0&&n==8);if(readerror){*signalptr=readerror;return readerror;}if(pos==size)return AUDIO_EOF;memcpy(d,input+pos,8);pos+=8;return 0;}
Byte os_write(Byte p,const void *d,Word n){assert(p==1&&n==8);memcpy(output+written,d,8);written+=8;return 0;}
int main(void){char *argv[]={"dodaudio","ssc-mame-fast"};int n=0;
 reset();queue(AUDIO_PLAY);queue(AUDIO_STOP);queue(AUDIO_SHUTDOWN);assert(!audio_service_main(2,argv)&&opened==1&&closed==1&&played==1&&stopped==2&&written==32);++n;
 reset();queue(AUDIO_PLAY);queue(AUDIO_PLAY);queue(AUDIO_SHUTDOWN);assert(!audio_service_main(2,argv)&&played==2&&opened==1&&closed==1);++n;
 reset();queue(AUDIO_PLAY);input[1]=2;assert(audio_service_main(2,argv)==AUDIO_BAD&&played==0&&closed==1&&output[12]==AUDIO_BAD);++n;
 reset();failopen=AUDIO_BUSY;assert(audio_service_main(2,argv)==AUDIO_BUSY&&closed==1&&output[4]==AUDIO_BUSY);++n;
 reset();queue(AUDIO_PLAY);failplay=246;assert(audio_service_main(2,argv)==246&&closed==1&&output[12]==246);++n;
 reset();readerror=3;assert(audio_service_main(2,argv)==3&&closed==1);++n;
 reset();assert(audio_service_main(2,argv)==AUDIO_EOF&&closed==1);++n;
 printf("%d audio service lifecycle checks passed\n",n);return 0;}
