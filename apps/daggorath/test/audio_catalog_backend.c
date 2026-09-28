#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "audio/backend.h"
#include "audio/ssc_catalog.h"
#include "audio/ssc_io.h"
static Byte ports[65536],wire[4096],cancelled;
static unsigned count,sleeps,reset;
Byte ssc_read_io(Word address){return ports[address];}
void ssc_write_io(Word address,Byte value){
 ports[address]=value;
 if(address==0xff7d)++reset;
 if(address==0xff7e){assert(count<sizeof(wire));wire[count++]=value;ports[address]=0xe0;}
}
Byte os_signal_value(Byte *flag){return *flag;}
Byte os_sleep(Word ticks){sleeps+=ticks;return 0;}
int main(void){Byte id,expected[SSC_CATALOG_CAPACITY],length;
 unsigned start,before;
 memset(ports,0,sizeof(ports));ports[0xff7e]=0xe0;
 assert(!backend_open("ssc-mame-fast",&cancelled)&&reset==2);
 for(id=0;id<=AUDIO_LAST_SOURCE_ID;id++){
  assert(!backend_stop());start=count;before=sleeps;
  assert(!backend_play(id,255));
  if(id==AUDIO_SQUEAK||id==AUDIO_PHASER||id==AUDIO_WHOOP){
   assert(count==start+1&&wire[start]==
    (id==AUDIO_SQUEAK?0xd8:id==AUDIO_PHASER?0xca:0xd9));
  }else{
   assert(!ssc_catalog_build(id,1,expected,&length));
   assert(count==start+length+2&&wire[start]==0x9e&&
    !memcmp(wire+start+1,expected,length)&&wire[count-1]==0xde);
   /* Ready-gated upload does not add a full scheduler tick per byte. */
   assert(sleeps-before==18); /* live MAME buffer-6 sound-active fence */
  }
 }
 assert(!backend_stop()&&!backend_close());
 assert(backend_play(AUDIO_LAST_SOURCE_ID+1,255)==AUDIO_BAD); /* closed */
 puts("23 SSC backend dispatches and bounded catalog uploads passed");
 return 0;
}
