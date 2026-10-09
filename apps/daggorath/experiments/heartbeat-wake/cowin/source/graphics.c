#include <cmoc.h>
#include "graphics.h"

#ifdef RETURN_DIAG
extern Word _stbot;
extern Word _mtop;
#endif

/* Disposable memory-only scene-boundary record.  It is read externally and
 * never affects control flow or performs I/O. */
#ifdef RETURN_DIAG
Byte cowinDiag[80]={0xc6,0xc0,0x57,0x49,0x4e,0x02};
#else
Byte cowinDiag[64]={0xc6,0xc0,0x57,0x49,0x4e,0x02};
#endif
static void result(Byte stage,Byte value){cowinDiag[6]=stage;cowinDiag[27]=(Byte)(value!=0);cowinDiag[28]=value;}
static void packet(Byte stage,Byte path,const Byte *p,Byte n){Byte i;cowinDiag[6]=stage;cowinDiag[7]=path;cowinDiag[29]=n;for(i=0;i<16;i++)cowinDiag[30+i]=i<n?p[i]:0;}

/* Disposable CoWin POC.  Packets are the production Daggorath type-5
 * presentation packets; group 196 buffers 2 and 3 are persistent 32x7
 * heart-only GP buffers.  The broker owns the path and is the only writer. */
static const Byte setup[]={27,0x20,5,0,0,80,25,1,0,0,27,0x35,0,
    27,0x31,0,0,27,0x31,1,63,5,32};
static const Byte selectWindow[]={27,0x21};
static const Byte statusDefine[]={27,0x29,196,1,3,0};
static Byte statusLoad[651];
static const Byte statusPut[]={27,0x2d,196,1,0,0,0,152};
static const Byte statusKill[]={27,0x2a,196,1};
static const Byte sceneDefine[]={27,0x29,196,4,2,0};
/* Installed EOU GetBlk packet: group/buffer, x/y, width/height.  Initialize
 * persistent GP metadata once before SS.MpGPB, as in the verified production
 * strip path.  Scene requests continue to use mapped writes plus PutBlk only. */
static const Byte sceneCapture[]={27,0x2c,196,4,0,0,0,0,2,0,0,8};
static Byte scenePut[]={27,0x2d,196,4,0,64,0,4};
static const Byte sceneKill[]={27,0x2a,196,4};
static Byte *scenePixels;
static Word sceneLength;
enum { SCENE_ACKNOWLEDGED,SCENE_PREPARE,SCENE_READY,SCENE_SUBMITTED };
static Byte sceneOwner=SCENE_ACKNOWLEDGED;
static Byte heartDefine[]={27,0x29,196,2,0,32};
static Byte heartLoad[]={27,0x2b,196,2,5,0,32,0,7,0,28,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static Byte heartPut[]={27,0x2d,196,2,1,48,0,156};
static const Byte heartKill[]={27,0x2a,196,2};
static const Byte doubled[]={0,3,12,15,48,51,60,63,192,195,204,207,240,243,252,255};
/* Original COMDAT:SPCTAB phases, inverted exactly as game_heart_patterns(). */
static const Byte hearts[]={
 255,255,254,254,255,255,255,255,95,15,15,31,191,255,
 255,254,252,252,254,255,255,255,79,7,7,15,31,191
};

static Byte open_window(Byte *fd){const char *name="/w";Byte e,n;asm{
 ldx :name
 lda #3
 os9 $84
 bcs @bad
 sta :n
 clrb
@bad
 stb :e
 }if(!e)*fd=n;return e;}
static Byte close_window(Byte fd){Byte e;asm{
 lda :fd
 os9 $8f
 bcs @bad
 clrb
@bad
 stb :e
 }return e;}
static Byte preload(Byte path,Byte slot,const Byte *heart){Byte row,*target,e;
 heartLoad[3]=(Byte)(2+slot);
 for(row=0;row<7;row++){
  target=heartLoad+11+(Word)row*4;
  *target++=doubled[heart[row]>>4];*target++=doubled[heart[row]&15];
  *target++=doubled[heart[7+row]>>4];*target=doubled[heart[7+row]&15];
 }
 e=os_write(path,heartLoad,sizeof(heartLoad));return e;
}
static void expanded_row(Byte *target,const Byte *heart,Byte row){
 *target++=doubled[heart[row]>>4];*target++=doubled[heart[row]&15];
 *target++=doubled[heart[7+row]>>4];*target=doubled[heart[7+row]&15];
}
Byte graphics_open(Byte *path){Byte e,p=255;
 e=open_window(&p);if(e)return e;
 e=os_write(p,setup,sizeof(setup));if(e)goto fail;
 memset(statusLoad,255,sizeof(statusLoad));
 statusLoad[0]=27;statusLoad[1]=0x2b;statusLoad[2]=196;statusLoad[3]=1;
 statusLoad[4]=5;statusLoad[5]=2;statusLoad[6]=128;statusLoad[7]=0;
 statusLoad[8]=8;statusLoad[9]=2;statusLoad[10]=128;
 e=os_write(p,statusDefine,sizeof(statusDefine));if(e)goto fail;
 e=os_write(p,statusLoad,sizeof(statusLoad));if(e)goto fail;
 e=os_write(p,statusPut,sizeof(statusPut));if(e)goto fail;
 packet(1,p,sceneDefine,sizeof(sceneDefine));e=os_write(p,sceneDefine,sizeof(sceneDefine));cowinDiag[22]=e;result(2,e);if(e)goto fail;
 packet(3,p,sceneCapture,sizeof(sceneCapture));e=os_write(p,sceneCapture,sizeof(sceneCapture));cowinDiag[23]=e;result(4,e);if(e)goto fail;
 e=os_map_buffer(p,0xc404,1,&scenePixels,&sceneLength);cowinDiag[23]=e;
 cowinDiag[48]=(Byte)((Word)scenePixels>>8);cowinDiag[49]=(Byte)scenePixels;
 cowinDiag[50]=(Byte)(sceneLength>>8);cowinDiag[51]=(Byte)sceneLength;
 result(5,e);if(e)goto fail;
 if(sceneLength!=512){e=187;result(6,e);goto fail;}
 heartDefine[3]=2;e=os_write(p,heartDefine,sizeof(heartDefine));if(e)goto fail;
 e=preload(p,0,hearts);if(e)goto fail;
 heartDefine[3]=3;e=os_write(p,heartDefine,sizeof(heartDefine));if(e)goto fail;
 e=preload(p,1,hearts+14);if(e)goto fail;
 e=os_write(p,selectWindow,sizeof(selectWindow));if(e)goto fail;
 *path=p;return 0;
fail: close_window(p);return e;
}
Byte graphics_heart(Byte path,Byte phase){Byte e;heartPut[3]=(Byte)(2+(phase&1));e=os_write(path,heartPut,sizeof(heartPut));if(e){cowinDiag[53]=e;result(15,e);}return e;}
Byte graphics_scene(Byte path,Byte destinationY,Byte status,Byte phase){Byte e,row,*data=scenePixels;
 cowinDiag[7]=path;cowinDiag[9]=status;cowinDiag[10]=destinationY;cowinDiag[11]=phase;
 cowinDiag[12]=(Byte)((Word)scenePixels>>8);cowinDiag[13]=(Byte)scenePixels;
 cowinDiag[14]=196;cowinDiag[15]=4;cowinDiag[16]=2;cowinDiag[17]=0;
 cowinDiag[18]=0;cowinDiag[19]=8;cowinDiag[20]=2;cowinDiag[21]=0;
 if(sceneOwner!=SCENE_ACKNOWLEDGED)return 188;
 sceneOwner=SCENE_PREPARE;cowinDiag[52]=sceneOwner;
 if(status){memset(data,255,512);for(row=0;row<7;row++)expanded_row(data+(Word)row*64+30,hearts+(phase?14:0),row);}
 else {for(row=0;row<8;row++)memset(data+(Word)row*64,(row&1)?0x11:0x44,64);}
 sceneOwner=SCENE_READY;cowinDiag[52]=sceneOwner;result(10,0);
 sceneOwner=SCENE_SUBMITTED;cowinDiag[52]=sceneOwner;
 scenePut[7]=(Byte)(4+destinationY);packet(13,path,scenePut,sizeof(scenePut));e=os_write(path,scenePut,sizeof(scenePut));cowinDiag[24]=e;
#ifdef RETURN_DIAG
 /* Disposable mapped-scene return capture.  U is this CMOC frame: saved U
  * is at 0,U and the caller return PC is at 2,U.  These are memory stores
  * only and execute after PutBlk returns, before any further C call. */
 asm {
  ldb #$b1
  stb _cowinDiag+79,y
  sty _cowinDiag+64,y
  stu _cowinDiag+66,y
  sts _cowinDiag+68,y
  ldd 2,u
  std _cowinDiag+70,y
  ldd 0,u
  std _cowinDiag+72,y
  ldd __stbot,y
  std _cowinDiag+74,y
  ldd __mtop,y
  std _cowinDiag+76,y
  pshs cc
  puls a
  sta _cowinDiag+78,y
 }
#endif
 result(14,e);
#ifdef RETURN_DIAG
 cowinDiag[79]=0xb2;
#endif
 sceneOwner=SCENE_ACKNOWLEDGED;cowinDiag[52]=sceneOwner;return e;
}
Byte graphics_close(Byte path){Byte e,first=0;
 heartKill[3]=2;e=os_write(path,heartKill,sizeof(heartKill));if(e&&!first)first=e;
 heartKill[3]=3;e=os_write(path,heartKill,sizeof(heartKill));if(e&&!first)first=e;
 e=os_write(path,statusKill,sizeof(statusKill));if(e&&!first)first=e;
 e=os_map_buffer(path,0xc404,0,&scenePixels,&sceneLength);if(e&&!first)first=e;
 e=os_write(path,sceneKill,sizeof(sceneKill));if(e&&!first)first=e;
 e=close_window(path);return first?first:e;
}
