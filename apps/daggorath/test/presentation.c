#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "presentation.h"
static int writes,closes,kills,allocationFails,maps,unmaps,opens,wrongWindow,nonWindow;
static unsigned char frame[6144],pixels[12288];
Byte window_open(Byte *p){++opens;*p=3;return 0;}
Byte window_close(Byte p){assert(p==3);++closes;return 0;}
Byte os_getstat(Byte p,Byte f,Registers *r){
 if(p==0||p==1){assert(f==SS_SCTYP);if(nonWindow&&p==1)return ERR_ARGUMENT;r->a=2;return 0;}
 assert(p==3);if(f==SS_SCTYP)r->a=5;else{assert(f==SS_SCSIZ);r->x=80;r->y=25;}return 0;
}
Byte os_devname(Byte p,char *buffer){
 assert(p==0||p==1);
 if(wrongWindow&&p==1)memcpy(buffer,"/w\262",3);
 else memcpy(buffer,"Term\355",5);
 return 0;
}
Byte os_map_buffer(Byte p,Word gb,Byte map,Byte **ptr,Word *size){
 assert(p==3&&gb==0xc401);
 if(map){++maps;*ptr=pixels;*size=sizeof(pixels);}else{assert(*ptr==pixels&&*size==12288);++unmaps;}
 return 0;
}
Byte os_write(Byte p,const void *raw,Word n){
 const Byte *b=raw;++writes;
 if(n==6&&b[1]==0x29){assert(p==3&&b[2]==196&&b[3]==1&&b[4]==48&&b[5]==0);if(allocationFails)return 194;}
 if(n==12&&b[1]==0x2c){assert(p==3&&b[2]==196&&b[3]==1&&b[4]==0&&b[5]==0&&b[6]==0&&b[7]==0&&b[8]==2&&b[9]==0&&b[10]==0&&b[11]==192);}
 if(n==8&&b[1]==0x2d&&writes==5){unsigned i,bit;assert(p==3&&b[4]==0&&b[5]==64&&b[6]==0&&b[7]==4);
  for(i=0;i<6144;i++)for(bit=0;bit<8;bit++){
   unsigned output=(pixels[i*2+bit/4]>>(6-2*(bit%4)))&3;
   assert(output==((frame[i]&(128>>bit))?3:0));
  }
 }
 if(n==4&&b[1]==0x2a){assert(p==1&&b[2]==196&&b[3]==1);assert(unmaps==1);++kills;}
 /* A frame must never go through the slow SCF byte upload path. */
 assert(n!=12288);return 0;
}
int main(void){
 static const Byte doubled[]={0,3,12,15,48,51,60,63,192,195,204,207,240,243,252,255};
 unsigned i,row;int before;static Byte baseline[12288];for(i=0;i<sizeof(frame);i++)frame[i]=(unsigned char)i;
 assert(screen_open()==0);assert(screen_present(frame)==0);memcpy(baseline,pixels,sizeof(baseline));
 /* Region refresh must update only the authoritative status/input rows while
  * leaving every dungeon pixel in the mapped GP buffer byte-for-byte intact. */
 for(i=0;i<sizeof(frame);i++)frame[i]=(unsigned char)(255-frame[i]);
 assert(screen_present_heart(frame)==0);
 for(row=0;row<192;row++)for(i=0;i<64;i++){
  unsigned at=row*64+i;Byte expected=baseline[at];
  if(row>=152&&row<159&&i>=30&&i<34){
   unsigned logical=frame[row*32+i/2];expected=doubled[(i&1)?(logical&15):(logical>>4)];
  }
  assert(pixels[at]==expected);
 }
 assert(screen_prepare_ui(frame)==0);
 for(row=0;row<192;row++)for(i=0;i<64;i++){
  unsigned at=row*64+i;Byte expected;
  if((row>=152&&row<160)||(row>=184&&row<191)){
   unsigned logical=frame[row*32+i/2];expected=doubled[(i&1)?(logical&15):(logical>>4)];
  }else expected=baseline[at];
  assert(pixels[at]==expected);
 }
 assert(screen_present_ui(frame)==0);assert(screen_close()==0);assert(closes==1&&kills==1&&maps==1&&unmaps==1);
 allocationFails=1;kills=closes=maps=unmaps=0;assert(screen_open()==194);assert(screen_close()==0);assert(closes==1&&kills==0&&maps==0&&unmaps==0);
 allocationFails=0;wrongWindow=1;before=opens;assert(screen_open()==ERR_ARGUMENT);assert(opens==before&&screen_close()==0);
 wrongWindow=0;nonWindow=1;assert(screen_open()==ERR_ARGUMENT);assert(opens==before&&screen_close()==0);
 nonWindow=0;assert(screen_open()==0);assert(screen_close()==0);assert(opens==before+1);
 puts("6 presentation ownership/packet and invoking-window cases passed");return 0;
}
