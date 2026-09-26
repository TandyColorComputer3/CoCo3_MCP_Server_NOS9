#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "audio.h"
#include "backend.h"
#include "ssc_io.h"
static Byte ports[65536],bytes[512],flag;static unsigned count,reset,sleeps;
Byte ssc_read_io(Word a){return ports[a];}
void ssc_write_io(Word a,Byte b){ports[a]=b;if(a==0xff7e){bytes[count++]=b;ports[a]=0xe0;}if(a==0xff7d)++reset;}
Byte os_sleep(Word n){sleeps+=n;return 0;}
Byte os_signal_value(Byte *p){return *p;}
static void setup(void){memset(ports,0,sizeof(ports));ports[0xff7e]=0xe0;ports[0xff01]=0x35;ports[0xff03]=0x30;ports[0xff23]=0x34;count=reset=sleeps=flag=0;}
int main(void){Byte f[8];unsigned before;int n=0;
 audio_frame(f,AUDIO_PLAY,AUDIO_SQUEAK,255,17);assert(!audio_validate(f));++n;
 f[1]=2;assert(audio_validate(f)==AUDIO_BAD);++n;
 f[1]=1;f[3]=1;assert(audio_validate(f)==AUDIO_UNSUPPORTED);++n;
 f[3]=0;f[4]=128;assert(audio_validate(f)==AUDIO_UNSUPPORTED);++n;
 audio_frame(f,AUDIO_STOP,0,0,18);assert(!audio_validate(f));++n;
 f[7]=1;assert(audio_validate(f)==AUDIO_BAD);++n;
 setup();assert(backend_open("unknown",&flag)==AUDIO_UNSUPPORTED&&reset==0);++n;
 setup();ports[0xff7e]=0;assert(backend_open("ssc-mame-fast",&flag)==AUDIO_BUSY&&reset==0);++n;
 setup();assert(!backend_open("ssc-mame-fast",&flag));assert(reset==2);++n;
 assert(bytes[0]==0x8f&&bytes[2]==0x98&&bytes[count-1]==0xff&&count==24);++n;
 /* Manufacturer format: four tone groups, fixed amplitude, descending periods. */
 const unsigned periods[4]={145,113,81,49};
 for(unsigned i=0;i<4;i++){assert(bytes[3+i*4]==12);assert(bytes[4+i*4]*256+bytes[5+i*4]==periods[i]);assert(bytes[6+i*4]==0);if(i)assert(bytes[4+i*4]*256+bytes[5+i*4]<bytes[i*4]*256+bytes[1+i*4]);}++n;
 assert(bytes[19]==0&&bytes[20]==0&&bytes[21]==1&&bytes[22]==0);++n;
 before=count;assert(!backend_play(AUDIO_SQUEAK,255));assert(bytes[before]==0xd8&&count==before+1&&reset==2);++n;
 assert(!backend_play(AUDIO_SQUEAK,255)&&reset==2);++n;
 assert(!backend_stop()&&bytes[count-1]==0xcf&&reset==2);++n;
 flag=3;assert(backend_play(0,255)==3);assert(!backend_close());assert(reset==2&&bytes[count-1]==0xcf);++n;
 assert(ports[0xff01]==0x35&&ports[0xff03]==0x30&&ports[0xff23]==0x34);++n;
 setup();assert(!backend_open("ssc-mame-fast",&flag));ports[0xff7e]=0;before=sleeps;
 assert(backend_stop()==246&&sleeps-before==12);++n;
 assert(backend_close()==246&&reset==4);++n;
 printf("%d audio protocol/backend checks passed\n",n);return 0;
}
