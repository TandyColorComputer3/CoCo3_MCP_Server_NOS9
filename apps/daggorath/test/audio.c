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
 /* Approved M2 compatibility update: SSC manual pp15-18,26-28 defines
  * timer reload, four-byte channel-A tones and silence/FF termination.
  * M2 preloads SQUEAK in buffer 0, WHOOP in 1, PHASER in 2..5.
  * Verify each complete recipe, not a magic aggregate length. */
 {
  unsigned pos=0;
  const unsigned sq[]={145,113,81,49};
  const unsigned whoop[]={994,867,738,609,482,353,225,97};
  const unsigned phaser[]={269,225,181,141,97,53};
  const unsigned whoopTicksMinusOne[]={17,14,12,10,8,5,3,1};
  const unsigned commands[]={0x98,0x99,0x8a};
  const unsigned tones[]={4,8,60};
  assert(bytes[pos++]==0x8f&&bytes[pos++]==55); /* verified fast timer profile */
  for(unsigned recipe=0;recipe<3;recipe++){
   assert(bytes[pos++]==commands[recipe]);
   for(unsigned tone=0;tone<tones[recipe];tone++){
    unsigned period=recipe==0?sq[tone]:recipe==1?whoop[tone]:phaser[tone%6];
    assert(bytes[pos++]==12); /* channel A, fixed amplitude; no envelope */
    assert(bytes[pos++]==period/256);assert(bytes[pos++]==period%256);
    assert(bytes[pos++]==(recipe==1?whoopTicksMinusOne[tone]:0));
   }
   assert(bytes[pos++]==0&&bytes[pos++]==0&&bytes[pos++]==1);
   assert(bytes[pos++]==0&&bytes[pos++]==0xff); /* explicit silence + terminator */
  }
  assert(pos==count); /* no extra commands, playback or overlapping recipe data */
 }++n;
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
