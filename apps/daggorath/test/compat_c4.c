#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "compat/c4.h"
#include "compat/present.h"
#include "gameplay/opening-map.h"
#include "game_data.h"
static unsigned presents,wait81,wait2,wait1;
static Byte five(const Byte *p,Word bit){Byte n=0,i;for(i=0;i<5;i++,bit++)n=(n<<1)|((p[bit/8]>>(7-bit%8))&1);return n;}
Byte dag_compat_text_cell(DagCompatCellContext *c){Byte y;for(y=0;y<7;y++)c->logicalFrame[(c->cell/32*8+(Word)y)*32+c->cell%32]=(five(font+(Word)c->value*5,5+(Word)y*5)<<2)^c->inverse;return 0;}
Byte dag_compat_text_apply(DagCompatTextContext *c){Byte y;if(c->operation==DAG_COMPAT_TEXT_CLEAR_PRIMARY){memset(c->logicalFrame+160*32,0,32*32);c->cursor=0;}
 else if(c->operation==DAG_COMPAT_TEXT_CHARACTER){if(c->value==31)c->cursor=(c->cursor+32)&~31;else{for(y=0;y<7;y++)c->logicalFrame[(160+c->cursor/32*8+(Word)y)*32+c->cursor%32]=five(font+(Word)c->value*5,5+(Word)y*5)<<2;c->cursor++;}}return 0;}
Byte dag_compat_present(const Byte *f,DagCompatPresentTiming *t){(void)f;(void)t;presents++;return 0;}
Byte dag_compat_present_cell(const Byte *f,Word c,DagCompatPresentTiming *t){(void)f;(void)c;(void)t;presents++;return 0;}
Byte os_sleep(Word n){if(n==81)wait81++;else if(n==2)wait2++;else if(n==1)wait1++;else assert(0);return 0;}
int main(int argc,char **argv){Game expected,actual;Byte astate[DAG_COMPAT_STATE_BYTES],map1[6144],map2[6144],initial[6144];Word i;
 game_init_demo(&expected,21);dag_compat_c4_init(&actual,astate,21);assert(!memcmp(&expected,&actual,sizeof(Game)));
 assert(dag_compat_c4_random_calls()==2344);
 assert(!memcmp(astate+DAG_COMPAT_CCB_BASE,actual.creatures,32*17));assert(!memcmp(astate+0x3f4,actual.maze,1024));assert(!memcmp(astate+0x915,actual.objects,72*14));
 assert(actual.seed[0]==0x75&&actual.seed[1]==0x65&&actual.seed[2]==0x19);assert(actual.row==12&&actual.col==22&&actual.dir==0&&actual.level==2&&actual.power==6048);assert(actual.count==66&&actual.creatureCount==23);
 opening_render_map(&expected,map1);dag_compat_c4_map(&actual,map2);assert(!memcmp(map1,map2,6144));
 dag_compat_c4_initial(&actual,initial);for(i=0;i<152*32;i++)assert(initial[i]==0);for(i=160*32;i<192*32;i++)assert(initial[i]==0);
 dag_compat_c4_prompt(initial);
 {Byte seen=0,y;for(y=0;y<7;y++){seen|=initial[(168+(Word)y)*32];for(i=1;i<32;i++)assert(initial[(168+(Word)y)*32+i]==0);}assert(seen);}
 for(i=176*32;i<192*32;i++)assert(initial[i]==0);
 presents=wait81=wait2=wait1=0;assert(!dag_compat_c4_run(&actual,astate,map2));assert(presents==4&&wait81==2&&wait2==1&&wait1==1);
 if(argc==2){char path[512];FILE *f;
  snprintf(path,sizeof(path),"%s/game.bin",argv[1]);f=fopen(path,"wb");assert(f);assert(fwrite(&expected,1,sizeof(expected),f)==sizeof(expected));fclose(f);
  snprintf(path,sizeof(path),"%s/state.bin",argv[1]);f=fopen(path,"wb");assert(f);assert(fwrite(astate,1,sizeof(astate),f)==sizeof(astate));fclose(f);
  dag_compat_c4_map(&expected,map1);snprintf(path,sizeof(path),"%s/map.frame",argv[1]);f=fopen(path,"wb");assert(f);assert(fwrite(map1,1,sizeof(map1),f)==sizeof(map1));fclose(f);
  dag_compat_c4_initial(&expected,initial);snprintf(path,sizeof(path),"%s/initial.frame",argv[1]);f=fopen(path,"wb");assert(f);assert(fwrite(initial,1,sizeof(initial),f)==sizeof(initial));fclose(f);
  dag_compat_c4_prompt(initial);snprintf(path,sizeof(path),"%s/prompt.frame",argv[1]);f=fopen(path,"wb");assert(f);assert(fwrite(initial,1,sizeof(initial),f)==sizeof(initial));fclose(f);
 }
 puts("compat C4 initialization, map, transition and lone-dot contracts passed");return 0;}
