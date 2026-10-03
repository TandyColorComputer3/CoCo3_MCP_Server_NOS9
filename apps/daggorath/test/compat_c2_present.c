#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "compat/present.h"

static Byte source[DAG_COMPAT_FRAME_BYTES],mapped[DAG_COMPAT_STRIP_TARGET_BYTES];
static unsigned stripPuts,maps,unmaps;
Byte window_open(Byte *path){*path=7;return 0;}
Byte window_close(Byte path){assert(path==7);return 0;}
Byte os_clock(Word *ticks,Byte validate){static Word t;assert(!validate);*ticks=t++;return 0;}
Byte os_map_buffer(Byte path,Word id,Byte map,Byte **buffer,Word *length)
{assert(path==7&&id==0xc401);if(map){++maps;*buffer=mapped;*length=sizeof(mapped);}else ++unmaps;return 0;}
void dag_compat_expand_strip(const Byte *input,Byte *output)
{static const Byte e[16]={0,3,12,15,48,51,60,63,192,195,204,207,240,243,252,255};unsigned i;for(i=0;i<1024;i++){output[i*2]=e[input[i]>>4];output[i*2+1]=e[input[i]&15];}}
Byte os_write(Byte path,const void *raw,Word length)
{const Byte *p=raw;if(length==8&&p[0]==27&&p[1]==0x2d){assert(path==7&&p[3]==1);assert(p[4]==0&&p[5]==64&&p[6]==0&&p[7]==164);++stripPuts;}return 0;}
int main(void)
{DagCompatPresentTiming t;unsigned y;memset(source+160*32,0xa5,32*32);assert(!dag_compat_present_open());assert(!dag_compat_present_primary(source,&t));assert(stripPuts==1&&t.conversionTicks==1&&t.transferTicks==1);for(y=0;y<8;y++)source[(168+y)*32+1]=0xf0;assert(!dag_compat_present_cell(source,33,&t));for(y=0;y<8;y++)assert(mapped[8*64+2+y*64]==0xff&&mapped[8*64+3+y*64]==0);assert(stripPuts==2&&t.mappedLength==sizeof(mapped)&&t.conversionTicks==1&&t.transferTicks==1);assert(!dag_compat_present_close());assert(maps==1&&unmaps==1);puts("compat C2 bounded cell conversion in primary presentation passed");return 0;}
