#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "compat/present.h"

static Byte frame[DAG_COMPAT_FRAME_BYTES],mapped[DAG_COMPAT_PRESENT_TARGET_BYTES];
static Byte patterns[28];
static unsigned directVisiblePuts,statusRequests,sceneRequests,progressCalls;
static Byte sceneY[24],sceneStatus[24];
static int visiblePhase=-1;
static Word clockValue;
static DagHeartbeatBroker broker={.command=3,.reply=4,.pid=9,.opened=1,
 .phase=1,.generation=7};

Byte window_open(Byte *path){*path=7;return 0;}
Byte window_close(Byte path){assert(path==7);return 0;}
Byte os_clock(Word *ticks,Byte validate){assert(!validate);*ticks=clockValue++;return 0;}
Byte os_map_buffer(Byte path,Word id,Byte map,Byte **buffer,Word *length)
{assert(path==7&&id==0xc401);if(map){*buffer=mapped;*length=sizeof(mapped);}return 0;}
void dag_compat_expand_strip(const Byte *input,Byte *output)
{static const Byte d[]={0,3,12,15,48,51,60,63,192,195,204,207,240,243,252,255};unsigned i;
 for(i=0;i<DAG_COMPAT_STRIP_SOURCE_BYTES;i++){output[i*2]=d[input[i]>>4];output[i*2+1]=d[input[i]&15];}}
void dag_compat_expand_slice(const Byte *input,Byte *output)
{static const Byte d[]={0,3,12,15,48,51,60,63,192,195,204,207,240,243,252,255};unsigned i;
 for(i=0;i<DAG_COMPAT_PRESENT_SOURCE_BYTES;i++){output[i*2]=d[input[i]>>4];output[i*2+1]=d[input[i]&15];}}
Byte os_write(Byte path,const void *raw,Word length){const Byte *p=raw;
 if(length==8&&p[1]==0x2d&&p[3]==1){unsigned strip=(p[7]-4)/DAG_COMPAT_PRESENT_ROWS;
  (void)strip;assert(path==7);++directVisiblePuts;
 }else if(length==8&&p[1]==0x2d&&(p[3]==2||p[3]==3)){
  ++directVisiblePuts;visiblePhase=p[3]-2;
 }
 return 0;
}
Byte dag_broker_snapshot(DagHeartbeatBroker *b){assert(b==&broker&&b->opened);return 0;}
Byte dag_broker_scene(DagHeartbeatBroker *b,Byte y,Byte status,Byte *stale){static Byte reject=1;
 assert(b==&broker&&b->opened&&y>=4);if(status){++statusRequests;
  if(reject){reject=0;b->generation++;b->phase=0;*stale=1;return 0;}
  visiblePhase=(mapped[(152-(y-4))*64+30]!=0); /* phase 0 cache starts with $01 */
 }else *stale=0;
 assert(sceneRequests<24);sceneY[sceneRequests]=y;sceneStatus[sceneRequests]=status;
 /* Exact C4 oracle pixels: status rows 152, 158, 159 and prompt row 174
  * must reside in their own eight-row transfers. */
 if(y==156){assert(mapped[0]==0xc0&&mapped[1]==0x0f);
  assert(mapped[6*64+62]==0xfc&&mapped[6*64+63]==0xff);
  assert(mapped[7*64]==0xff&&mapped[7*64+1]==0xff);}
 if(y==172)assert(mapped[6*64]==0x03&&mapped[6*64+1]==0x00);
 ++sceneRequests;*stale=0;return 0;
}
static Byte progress(void *context){assert(context==frame);++progressCalls;return 0;}

int main(void){DagCompatPresentTiming timing;unsigned row;
 assert(DAG_COMPAT_PRESENT_ROWS==8&&DAG_COMPAT_PRESENT_SOURCE_BYTES==256&&
        DAG_COMPAT_PRESENT_TARGET_BYTES==512&&DAG_COMPAT_HEIGHT/DAG_COMPAT_PRESENT_ROWS==24);
 memset(frame,0,sizeof(frame));memset(patterns,0,sizeof(patterns));
 /* Distinct protected phases make the selected preloaded buffer observable. */
 for(row=0;row<14;row++){patterns[row]=(Byte)(row+1);patterns[14+row]=(Byte)(0x80+row);}
 dag_compat_present_set_hearts(patterns);assert(!dag_compat_present_open());
 dag_compat_present_set_broker(&broker);
 frame[152*32]=0x83;frame[158*32+31]=0xef;frame[159*32]=0xff;
 frame[174*32]=0x10;
 for(row=0;row<7;row++){frame[(152+row)*32+15]=patterns[14+row];frame[(152+row)*32+16]=patterns[21+row];}
 assert(!dag_compat_present_progress(frame,&timing,progress,frame));
 /* Each strip is checked before expansion, between expansion and PutBlk,
  * and after PutBlk. The status slices already carry the current protected
  * phase, so no redundant heart transition is presented. */
 assert(statusRequests==2&&sceneRequests==24&&directVisiblePuts==0&&visiblePhase==0&&progressCalls==72);
 for(row=0;row<24;row++){assert(sceneY[row]==4+row*8);assert(sceneStatus[row]==(row==19));}
 sceneRequests=0;assert(!dag_compat_present_cell(frame,32,&timing));
 assert(sceneRequests==1&&sceneY[0]==172&&sceneStatus[0]==0);
 assert(!dag_compat_present_close());
 puts("compat C6 full-frame status overwrite restores authoritative protected heart");return 0;
}
