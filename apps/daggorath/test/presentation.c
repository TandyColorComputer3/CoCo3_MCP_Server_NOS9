#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "presentation.h"
static int writes,closes,kills,allocationFails,failHeartDefine3,failHeartLoadSlot=-1,maps,unmaps,opens,wrongWindow,nonWindow,stripPuts,heartPuts,heartLoads,progressCalls;
static unsigned char frame[6144],pixels[2048],stripHistory[32],killIds[4];
static unsigned char heartPatterns[28];
static const Byte doubled[]={0,3,12,15,48,51,60,63,192,195,204,207,240,243,252,255};
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
 if(map){++maps;*ptr=pixels;*size=sizeof(pixels);}
 else{assert(*ptr==pixels);++unmaps;}
 return 0;
}
Byte os_write(Byte p,const void *raw,Word n){
 const Byte *b=raw;++writes;
 if(n==6&&b[1]==0x29){assert(p==3&&b[2]==196);
  if(b[3]==1){assert(b[4]==8&&b[5]==0);if(allocationFails)return 194;}
  else {assert((b[3]==2||b[3]==3)&&b[4]==0&&b[5]==32);if(b[3]==3&&failHeartDefine3)return 194;}
 }
 if(n==12&&b[1]==0x2c)assert(p==3&&b[2]==196&&b[3]==1&&b[4]==0&&b[5]==0&&b[6]==0&&b[7]==0&&b[8]==2&&b[9]==0&&b[10]==0&&b[11]==32);
 if(n==39&&b[1]==0x2b){unsigned slot=b[3]-2;assert(p==3&&b[2]==196&&slot<2&&b[4]==5&&b[5]==0&&b[6]==32&&b[7]==0&&b[8]==7&&b[9]==0&&b[10]==28);++heartLoads;if((int)slot==failHeartLoadSlot)return 194;
  for(unsigned row=0;row<7;row++)for(unsigned col=0;col<4;col++){unsigned logical=heartPatterns[slot*14+(col/2)*7+row];Byte expected=doubled[(col&1)?(logical&15):(logical>>4)];assert(b[11+row*4+col]==expected);}
 }
 if(n==8&&b[1]==0x2d&&b[3]==1){unsigned row,col,logical,strip=(b[7]-4)/32;++stripPuts;assert(p==3&&b[4]==0&&b[5]==64&&b[6]==0&&b[7]==4+strip*32&&strip<6);stripHistory[stripPuts-1]=strip;
  for(row=0;row<32;row++)for(col=0;col<64;col++){
   logical=frame[(strip*32+row)*32+col/2];assert(pixels[row*64+col]==doubled[(col&1)?(logical&15):(logical>>4)]);
  }
 }
 if(n==8&&b[1]==0x2d&&(b[3]==2||b[3]==3)){assert(p==3&&b[2]==196&&b[4]==1&&b[5]==48&&b[6]==0&&b[7]==156);++heartPuts;
  assert(heartLoads==2);
 }
 if(n==4&&b[1]==0x2a){assert(p==1&&b[2]==196&&unmaps==1);killIds[kills++]=b[3];}
 /* A frame must never go through the slow SCF byte upload path. */
 assert(n!=12288);return 0;
}
static Byte progress(void *context){assert(context==frame);++progressCalls;return 0;}
int main(void){
 unsigned i,row;int before;for(i=0;i<sizeof(frame);i++)frame[i]=(unsigned char)i;for(i=0;i<28;i++)heartPatterns[i]=(unsigned char)(i*7+3);
 screen_set_heart_patterns(heartPatterns);assert(screen_open()==0);assert(heartLoads==2&&heartPuts==0);assert(screen_present(frame)==0);assert(stripPuts==6);
 for(i=0;i<6;i++)assert(stripHistory[i]==i);
 /* Heart refresh is independent of the reusable strip buffer. */
 for(row=0;row<7;row++){frame[(152+row)*32+15]=heartPatterns[row];frame[(152+row)*32+16]=heartPatterns[7+row];}
 assert(screen_present_heart(frame)==0);assert(heartLoads==2&&heartPuts==1&&stripPuts==6);
 for(row=0;row<7;row++){frame[(152+row)*32+15]=heartPatterns[14+row];frame[(152+row)*32+16]=heartPatterns[21+row];}
 assert(screen_present_heart(frame)==0);assert(heartLoads==2&&heartPuts==2&&stripPuts==6);
 assert(screen_present_ui(frame)==0);assert(stripPuts==8&&stripHistory[6]==4&&stripHistory[7]==5);
 /* Full presentation offers a service point before each strip, after every
  * eight expanded rows, and after each bounded PutBlk. */
 assert(screen_prepare_progress(frame,progress,frame)==0);assert(screen_flip()==0);
 assert(stripPuts==14&&progressCalls==36);for(i=0;i<6;i++)assert(stripHistory[8+i]==i);
 /* UI preparation after a queued full frame must retain all six dirty strips,
  * matching the gameplay full-render/final-status sequence. */
 assert(screen_prepare_progress(frame,progress,frame)==0);
 assert(screen_prepare_ui_progress(frame,progress,frame)==0);assert(screen_flip()==0);
 assert(stripPuts==20&&progressCalls==72);for(i=0;i<6;i++)assert(stripHistory[14+i]==i);
 assert(screen_close()==0);assert(closes==1&&kills==3&&killIds[0]==3&&killIds[1]==2&&killIds[2]==1&&maps==1&&unmaps==1);
 allocationFails=1;kills=closes=maps=unmaps=0;assert(screen_open()==194);assert(screen_close()==0);assert(closes==1&&kills==0&&maps==0&&unmaps==0);
 allocationFails=0;wrongWindow=1;before=opens;assert(screen_open()==ERR_ARGUMENT);assert(opens==before&&screen_close()==0);
 wrongWindow=0;nonWindow=1;assert(screen_open()==ERR_ARGUMENT);assert(opens==before&&screen_close()==0);
 nonWindow=0;assert(screen_open()==0);assert(screen_close()==0);assert(opens==before+1);
 failHeartLoadSlot=0;kills=closes=maps=unmaps=0;assert(screen_open()==194);assert(screen_close()==0);assert(kills==2&&killIds[0]==2&&killIds[1]==1&&closes==1&&maps==1&&unmaps==1);
 failHeartLoadSlot=-1;failHeartDefine3=1;kills=closes=maps=unmaps=0;assert(screen_open()==194);assert(screen_close()==0);assert(kills==2&&killIds[0]==2&&killIds[1]==1&&closes==1&&maps==1&&unmaps==1);
 puts("7 strip presentation, ownership, packet and invoking-window cases passed");return 0;
}
