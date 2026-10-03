/* ONCE.ASM:GAME20..GAME50, NEWLVL.ASM, DGNGEN.ASM, COMCRE.ASM,
 * MAPPER.ASM and PLOOK.ASM:INIVUX adapted to caller-owned Level II state.
 * This is the compatibility core's initializer, not the modern game runner. */
#ifdef _CMOC_VERSION_
#include <cmoc.h>
#else
#include <string.h>
#endif
#include "c4.h"
#include "present.h"
#include "game_data.h"

#define OBASE 0x0b15
static const signed char dr[]={-1,0,1,0},dc[]={0,1,0,-1};
static Word getword(const Byte *p){return (Word)p[0]*256+p[1];}
static void putword(Byte *p,Word w){p[0]=w>>8;p[1]=w;}
static Byte *ocb(Game *g,Word p){return g->objects[(p-OBASE)/14];}
#ifdef _CMOC_VERSION_
extern Byte game_random(Game *g);
#define c4_random game_random
#else
static unsigned long randomCalls;
static Byte c4_random(Game *g){Byte n,i,b,carry,next;
 ++randomCalls;
 for(n=0;n<8;n++){b=g->seed[2]&0xe1;carry=0;for(i=0;i<8;i++){carry^=b&1;b>>=1;}
  for(i=0;i<3;i++){next=g->seed[i]>>7;g->seed[i]=(g->seed[i]<<1)|carry;carry=next;}}
 return g->seed[0];}
#endif
#ifdef DAG_COMPAT_TEST
unsigned long dag_compat_c4_random_calls(void){return randomCalls;}
#endif
static Byte cell(const Game *g,int r,int c){return r<0||r>31||c<0||c>31?255:g->maze[(Word)r*32+c];}
static void random_cell(Game *g,Byte *r,Byte *c){*c=c4_random(g)&31;*r=c4_random(g)&31;}
static void maze(Game *g,Byte level,Byte second){Byte r,c,dir,dist,n[9],x,y,kind;int nr,nc;Word left,i;
 memset(g->maze,255,1024);g->seed[0]=level_seeds[level];g->seed[1]=level_seeds[level+1];g->seed[2]=level_seeds[level+2];
 random_cell(g,&r,&c);left=500;
 while(left){dir=c4_random(g)&3;dist=(c4_random(g)&7)+1;
  while(dist){nr=r+dr[dir];nc=c+dc[dir];if(nr<0||nr>31||nc<0||nc>31)break;
   if(cell(g,nr,nc)){for(y=0;y<3;y++)for(x=0;x<3;x++)n[y*3+x]=cell(g,nr+y-1,nc+x-1);
    if(!(Byte)(n[3]+n[0]+n[1])||!(Byte)(n[1]+n[2]+n[5])||!(Byte)(n[5]+n[8]+n[7])||!(Byte)(n[7]+n[6]+n[3]))break;
    g->maze[nr*32+nc]=0;--left;if(!left)break;}r=nr;c=nc;--dist;}}
 for(r=0;r<32;r++)for(c=0;c<32;c++)if(cell(g,r,c)!=255)
  for(dir=0;dir<4;dir++)if(cell(g,r+dr[dir],c+dc[dir])==255)g->maze[(Word)r*32+c]|=3<<(dir*2);
 for(kind=1;kind<=2;kind++){left=kind==1?70:45;while(left){random_cell(g,&r,&c);if(cell(g,r,c)==255)continue;
   dir=c4_random(g)&3;if(cell(g,r,c)&(3<<(dir*2)))continue;
   g->maze[(Word)r*32+c]|=kind<<(dir*2);g->maze[((int)r+dr[dir])*32+c+dc[dir]]|=kind<<(((dir+2)&3)*2);--left;}}
 i=second?second:256;while(i--)c4_random(g);
}
static void fill(Byte *o,Byte type){Word i;memcpy(o+10,odb+(Word)type*4,4);
 for(i=0;i<sizeof(special);i+=4)if(special[i]==type){memcpy(o+6,special+i+1,3);break;}}
static Word birth(Game *g,Byte type,Byte level){Byte *o=g->objects[g->count];Word addr=OBASE+(Word)g->count*14;Byte rev;
 ++g->count;o[9]=type;o[4]=level;fill(o,type);rev=o[11];if(o[10]>=3){fill(o,o[10]==3?16:o[10]==4?17:15);o[11]=rev;}return addr;}
static int creature(const Game *g,Byte r,Byte c){Byte i;for(i=0;i<g->creatureCount;i++)if(g->creatures[i][12]&&g->creatures[i][15]==r&&g->creatures[i][16]==c)return i;return -1;}
static void health(Game *g){unsigned long n=(unsigned long)g->power*64,d=(unsigned long)g->power+2UL*g->damage;
 g->rate=(Byte)((n/d+1)-19);g->dead=g->power<g->damage;}
static void state_word(Byte *s,Word at,Word value){s[at]=value>>8;s[at+1]=value;}
void dag_compat_c4_sync(const Game *g,Byte *s){
 memset(s,0,DAG_COMPAT_STATE_BYTES);s[0x13]=g->row;s[0x14]=g->col;s[0x23]=g->dir;s[0x81]=g->level;
 state_word(s,0x0f,OBASE+(Word)g->count*14);state_word(s,0x15,g->weight);state_word(s,0x17,g->power);
 state_word(s,0x1d,g->hand);state_word(s,0x1f,g->rightHand);state_word(s,0x21,g->damage);
 state_word(s,0x24,g->torch);s[0x26]=g->lit;state_word(s,0x29,g->bag);
 s[0x6b]=g->seed[0];s[0x6c]=g->seed[1];s[0x6d]=g->seed[2];s[0x77]=255;s[0x94]=255;
 memcpy(s+DAG_COMPAT_CCB_BASE,g->creatures,32*17);memcpy(s+0x03f4,g->maze,1024);memcpy(s+0x0915,g->objects,72*14);
}
void dag_compat_c4_init(Game *g,Byte *state,Byte second){static const Byte demdat[]={13,15,16,255};Byte type,level=2,n,r,c,i,off,births;int t;Word p,tail;Byte *o,*cr;
#ifdef DAG_COMPAT_TEST
 randomCalls=0;
#endif
 memset(g,0,sizeof(*g));g->level=2;g->row=12;g->col=22;g->power=6048;g->weight=35;
 for(type=0;type<sizeof(omx);type++){Byte objectLevel=omx[type]>>4;n=omx[type]&15;while(n--){p=birth(g,type,objectLevel);ocb(g,p)[5]=255;if(++objectLevel>5)objectLevel=omx[type]>>4;}}
 maze(g,level,second);off=(Byte)((level<<3)+(level<<2));
 for(t=11;t>=0;t--){births=level_cmt[off+t];for(n=0;n<births;n++){do{random_cell(g,&r,&c);}while(cell(g,r,c)==255||creature(g,r,c)>=0);
  cr=g->creatures[g->creatureCount++];memcpy(cr,cdb+t*8,8);cr[12]=255;cr[13]=t;cr[15]=r;cr[16]=c;}}
 i=0;for(n=0;n<g->count;n++){o=g->objects[n];if(o[4]!=level||o[5]!=255)continue;cr=g->creatures[i];putword(o,getword(cr+8));putword(cr+8,OBASE+(Word)n*14);if(++i==g->creatureCount)i=0;}
 tail=0;for(i=0;demdat[i]!=255;i++){p=birth(g,demdat[i],0x3c);o=ocb(g,p);o[5]=1;fill(o,demdat[i]);o[11]=0;if(tail)putword(ocb(g,tail),p);else g->bag=p;tail=p;}
 health(g);g->recovery=g->rate;g->burn=3600;dag_compat_c4_sync(g,state);
}
static void mark(Byte *f,Byte row,Byte col,Byte a,Byte b){Word at=(Word)row*192+col;f[at+32]=a;f[at+64]=b;f[at+96]=b;f[at+128]=a;}
void dag_compat_c4_map(const Game *g,Byte *f){Byte row,col,y,i;const Byte *o,*c;memset(f,0,DAG_COMPAT_FRAME_BYTES);
 for(row=0;row<32;row++)for(col=0;col<32;col++){Byte v=g->maze[(Word)row*32+col]==255?255:0;for(y=0;y<6;y++)f[(Word)row*192+(Word)y*32+col]=v;}
 for(i=0;i<g->count;i++){o=g->objects[i];if(o[4]==g->level&&!o[5])mark(f,o[2],o[3],0,8);}
 for(i=0;i<32;i++){c=g->creatures[i];if(c[12])mark(f,c[15],c[16],0x10,0x54);}mark(f,g->row,g->col,0x24,0x18);
}
static void status_name(Byte *f,const Byte *name,Byte n,Byte col){Byte i;DagCompatCellContext c;c.logicalFrame=f;c.inverse=255;
 for(i=0;i<n;i++){c.cell=19*32+col+i;c.value=name[i];(void)dag_compat_text_cell(&c);}}
void dag_compat_c4_initial(const Game *g,Byte *f){static const Byte empty[]={5,13,16,20,25};Byte y;
 (void)g;memset(f,0,DAG_COMPAT_FRAME_BYTES);memset(f+152*32,255,8*32);status_name(f,empty,5,0);status_name(f,empty,5,27);
 for(y=0;y<7;y++){f[(152+(Word)y)*32+15]=255^status_hearts[y];f[(152+(Word)y)*32+16]=255^status_hearts[7+y];}
}
void dag_compat_c4_prompt(Byte *f){DagCompatTextContext t;
 t.logicalFrame=f;t.cursor=0;t.inverse=0;t.operation=DAG_COMPAT_TEXT_CLEAR_PRIMARY;t.value=0;(void)dag_compat_text_apply(&t);
 t.operation=DAG_COMPAT_TEXT_CHARACTER;t.value=31;(void)dag_compat_text_apply(&t);t.value=30;(void)dag_compat_text_apply(&t);
}
Byte dag_compat_c4_run(Game *g,Byte *state,Byte *frame){DagCompatPresentTiming timing;Byte e;
 dag_compat_c4_init(g,state,21);dag_compat_c4_map(g,frame);e=dag_compat_present(frame,&timing);if(e)return e;
 e=os_sleep(81);if(e)return e;e=os_sleep(81);if(e)return e;
 memset(frame,0,DAG_COMPAT_FRAME_BYTES);e=dag_compat_present(frame,&timing);if(e)return e;e=os_sleep(2);if(e)return e;
 dag_compat_c4_initial(g,frame);e=dag_compat_present(frame,&timing);if(e)return e;
 /* PLOOK:PUPDAT completes an UPDATE/SYNC before GAME50 invokes PROMPX. */
 e=os_sleep(1);if(e)return e;
 dag_compat_c4_prompt(frame);return dag_compat_present_cell(frame,32,&timing);
}
