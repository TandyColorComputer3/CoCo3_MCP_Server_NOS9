#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "audio/audio.h"
#include "audio/ssc_catalog.h"
static const char *const expected[]={"squeak","rattle","growl","beoop",
 "klank","grawl","pssst","kklank","pssht","snarl","wizard-1",
 "wizard-2","gluglg","phaser","whoop","clang","whoosh",
 "chuck","klink","clank","thud","bang","kaboom"};
static unsigned duration(const Byte *data,Byte length){
 Byte i=0;unsigned ticks=0;while(i<length-1){
  Byte kind=data[i]&0xe0;
  if(kind==0x80||kind==0xa0){assert(i+3<=length-1);ticks+=data[i+2]+1;i+=3;}
  else {assert(i+4<=length-1);ticks+=data[i+3]+1;i+=4;}
 }assert(i==length-1&&data[i]==0xff);return ticks;
}
int main(void){Byte id,data[SSC_CATALOG_CAPACITY],len,slow[SSC_CATALOG_CAPACITY],sl;
 Byte frame[AUDIO_FRAME];unsigned checked=0;
 for(id=0;id<=AUDIO_LAST_SOURCE_ID;id++){
  assert(!strcmp(ssc_catalog_name(id),expected[id]));
  audio_frame(frame,AUDIO_CATALOG_PLAY,id,255,id);
  assert(!audio_validate(frame));
  if(id==AUDIO_SQUEAK||id==AUDIO_PHASER||id==AUDIO_WHOOP){
   assert(ssc_catalog_build(id,1,data,&len)==AUDIO_UNSUPPORTED);
  }else{
   memset(data,0xee,sizeof(data));
   assert(!ssc_catalog_build(id,1,data,&len));
   assert(len>4&&len<=SSC_CATALOG_CAPACITY&&data[len-1]==0xff);
   assert(!ssc_catalog_build(id,0,slow,&sl)&&sl==len);
   assert(slow[sl-1]==0xff&&duration(data,len)>0);
   ++checked;
  }
 }
 assert(checked==20&&ssc_catalog_name(23)==0);
 audio_frame(frame,AUDIO_CATALOG_PLAY,23,255,1);
 assert(audio_validate(frame)==AUDIO_UNSUPPORTED);
 audio_frame(frame,AUDIO_CATALOG_PLAY,AUDIO_RATTLE,254,1);
 assert(audio_validate(frame)==AUDIO_UNSUPPORTED);
 audio_frame(frame,AUDIO_PLAY,AUDIO_RATTLE,255,1);
 assert(audio_validate(frame)==AUDIO_UNSUPPORTED); /* legacy M1 contract */
 assert(ssc_catalog_build(AUDIO_RATTLE,1,data,&len)==0&&len==61);
 assert(ssc_catalog_build(AUDIO_PSSST,1,data,&len)==0&&len==13);
 assert(ssc_catalog_build(AUDIO_PSSHT,1,data,&len)==0&&len==7);
 assert(ssc_catalog_build(AUDIO_KLANK,1,data,&len)==0&&len==57);
 assert(data[0]==12&&data[1]==1&&data[2]==94&&data[3]==10);
 assert(data[4]==0x2c&&data[5]==0&&data[6]==108);
 assert(ssc_catalog_build(AUDIO_KKLANK,1,data,&len)==0&&len==57);
 assert(data[0]==12&&data[1]==0&&data[2]==100&&data[3]==4);
 assert(data[4]==0x2c&&data[5]==0&&data[6]==36);
 assert(ssc_catalog_build(AUDIO_WIZARD_1,1,data,&len)==0);
 assert(ssc_catalog_build(AUDIO_WIZARD_2,1,slow,&sl)==0&&sl==len);
 assert(!memcmp(data,slow,len)); /* original SNDTAB shares BDLBDL */
 puts("23 source IDs, 20 bounded SSC catalog recipes and legacy wire contract passed");
 return 0;
}
