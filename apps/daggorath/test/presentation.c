#include <assert.h>
#include <stdio.h>
#include "presentation.h"
static int writes,closes,kills,allocationFails,maps,unmaps;static unsigned char frame[6144],pixels[12288];
Byte window_open(Byte *p){*p=3;return 0;}
Byte window_close(Byte p){assert(p==3);++closes;return 0;}
Byte os_getstat(Byte p,Byte f,Registers *r){assert(p==3);if(f==SS_SCTYP)r->a=5;else{assert(f==SS_SCSIZ);r->x=80;r->y=25;}return 0;}
Byte os_map_buffer(Byte p,Word gb,Byte map,Byte **ptr,Word *size){
 assert(p==3&&gb==0xc401);
 if(map){++maps;*ptr=pixels;*size=sizeof(pixels);}else{assert(*ptr==pixels&&*size==12288);++unmaps;}
 return 0;
}
Byte os_write(Byte p,const void *raw,Word n){
 const Byte *b=raw;++writes;
 if(n==6&&b[1]==0x29){assert(p==3&&b[2]==196&&b[3]==1&&b[4]==48&&b[5]==0);if(allocationFails)return 194;}
 if(n==12&&b[1]==0x2c){assert(p==3&&b[2]==196&&b[3]==1&&b[4]==0&&b[5]==0&&b[6]==0&&b[7]==0&&b[8]==2&&b[9]==0&&b[10]==0&&b[11]==192);}
 if(n==8&&b[1]==0x2d){unsigned i,bit;assert(p==3&&b[4]==0&&b[5]==64&&b[6]==0&&b[7]==4);
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
 unsigned i;for(i=0;i<sizeof(frame);i++)frame[i]=(unsigned char)i;
 assert(screen_open()==0);assert(screen_present(frame)==0);assert(screen_close()==0);assert(closes==1&&kills==1&&maps==1&&unmaps==1);
 allocationFails=1;kills=closes=maps=unmaps=0;assert(screen_open()==194);assert(screen_close()==0);assert(closes==1&&kills==0&&maps==0&&unmaps==0);
 puts("2 presentation ownership/packet cases passed");return 0;
}
