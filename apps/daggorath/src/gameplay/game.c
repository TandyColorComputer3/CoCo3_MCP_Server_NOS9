/* Original-derived GAME/DGNGEN/NEWLVX/OBIRTX/CBIRTH/PPULL/PUSE/PTURN/
 * PMOVE/HUPDAX/HSLOW/BURNER. Pinned paths/labels: import_gameplay.py and
 * DAGGORATH_GAMEPLAY_M1.md. No hardware, OS calls, physical coordinates or AI. */
#ifdef _CMOC_VERSION_
#include <cmoc.h>
#else
#include <string.h>
#endif
#include "game.h"
#include "game_data.h"
static const signed char dr[]={-1,0,1,0},dc[]={0,1,0,-1};
static Word getword(const Byte *p){return (Word)p[0]*256+p[1];}
static void putword(Byte *p,Word w){p[0]=w>>8;p[1]=w&255;}
#define OBASE 0x0b15
static Byte *ocb(Game *g,Word p){return g->objects[(p-OBASE)/14];}
Byte game_random(Game *g){Byte n,i,b,carry,next;
 for(n=0;n<8;n++){b=g->seed[2]&0xe1;carry=0;for(i=0;i<8;i++){carry^=b&1;b>>=1;}
  for(i=0;i<3;i++){next=g->seed[i]>>7;g->seed[i]=(g->seed[i]<<1)|carry;carry=next;}}
 return g->seed[0];}
static Byte cell(Game *g,int r,int c){return r<0||r>31||c<0||c>31?255:g->maze[(Word)r*32+c];}
static void random_cell(Game *g,Byte *r,Byte *c){*c=game_random(g)&31;*r=game_random(g)&31;}
static void maze(Game *g,Byte second){Byte r,c,dir,dist,n[9],x,y,kind;int nr,nc;Word left,i;
 memset(g->maze,255,1024);g->seed[0]=0x73;g->seed[1]=0xc7;g->seed[2]=0x5d;
 random_cell(g,&r,&c);left=500;
 while(left){dir=game_random(g)&3;dist=(game_random(g)&7)+1;
  while(dist){nr=r+dr[dir];nc=c+dc[dir];if(nr<0||nr>31||nc<0||nc>31)break;
   if(cell(g,nr,nc)){
    for(y=0;y<3;y++)for(x=0;x<3;x++)n[y*3+x]=cell(g,nr+y-1,nc+x-1);
    if(!(Byte)(n[3]+n[0]+n[1])||!(Byte)(n[1]+n[2]+n[5])||!(Byte)(n[5]+n[8]+n[7])||!(Byte)(n[7]+n[6]+n[3]))break;
    g->maze[nr*32+nc]=0;--left;if(!left)break;
   }r=nr;c=nc;--dist;
  }
 }
 for(r=0;r<32;r++)for(c=0;c<32;c++)if(cell(g,r,c)!=255)
  for(dir=0;dir<4;dir++)if(cell(g,r+dr[dir],c+dc[dir])==255)g->maze[(Word)r*32+c]|=3<<(dir*2);
 for(kind=1;kind<=2;kind++){left=kind==1?70:45;while(left){random_cell(g,&r,&c);if(cell(g,r,c)==255)continue;
   dir=game_random(g)&3;if(cell(g,r,c)&(3<<(dir*2)))continue;
   g->maze[(Word)r*32+c]|=kind<<(dir*2);g->maze[((int)r+dr[dir])*32+c+dc[dir]]|=kind<<(((dir+2)&3)*2);--left;}}
 i=second?second:256;while(i--)game_random(g);
}
static void fill(Byte *o,Byte type){Word i;memcpy(o+10,odb+type*4,4);
 for(i=0;i<sizeof(special);i+=4)if(special[i]==type){memcpy(o+6,special+i+1,3);break;}}
static Word birth(Game *g,Byte type,Byte level){Byte *o=g->objects[g->count];Word addr=OBASE+(Word)g->count*14;Byte rev;
 ++g->count;o[9]=type;o[4]=level;fill(o,type);rev=o[11];
 if(o[10]>=3){fill(o,o[10]==3?16:o[10]==4?17:15);o[11]=rev;}return addr;}
static int creature(Game *g,Byte r,Byte c){Byte i;for(i=0;i<g->creatureCount;i++)if(g->creatures[i][15]==r&&g->creatures[i][16]==c)return i;return -1;}
/* Exact increment-before-borrow quotient; 24-bit quantities fit unsigned long.
 * Original faint/death flags retained; visual faint transitions are not here. */
static void health(Game *g){unsigned long numerator=(unsigned long)g->power*64,den=(unsigned long)g->power+2UL*g->damage;
 g->rate=(Byte)((numerator/den+1)-19);
 if(!g->faint&&(signed char)g->rate<=3)g->faint=1;
 else if(g->faint&&(signed char)g->rate>4)g->faint=0;
 g->dead=g->power<g->damage;
}
void game_init(Game *g,Byte second){Byte type,level,n,r,c,i;int t;Word p,tail;Byte *o,*cr;
 memset(g,0,sizeof(*g));g->row=16;g->col=11;g->power=160;g->weight=35;
 for(type=0;type<sizeof(omx);type++){level=omx[type]>>4;n=omx[type]&15;while(n--){p=birth(g,type,level);ocb(g,p)[5]=255;if(++level>5)level=omx[type]>>4;}}
 maze(g,second);
 for(t=11;t>=0;t--)for(n=0;n<cmt[t];n++){
  do{random_cell(g,&r,&c);}while(cell(g,r,c)==255||creature(g,r,c)>=0);
  cr=g->creatures[g->creatureCount++];memcpy(cr,cdb+t*8,8);cr[12]=255;cr[13]=t;cr[15]=r;cr[16]=c;
 }
 i=0;for(n=0;n<g->count;n++){o=g->objects[n];if(o[4]||o[5]!=255)continue;
  cr=g->creatures[i];putword(o,getword(cr+8));putword(cr+8,OBASE+(Word)n*14);if(++i==g->creatureCount)i=0;}
 tail=0;for(i=0;i<2;i++){/* GAME10 leaves B=11; SWI preserves it through NEWLVX/GAME30. */
 p=birth(g,i?15:17,11);o=ocb(g,p);o[5]=1;fill(o,i?15:17);o[11]=0;
  if(tail)putword(ocb(g,tail),p);else g->bag=p;tail=p;}
 health(g);g->recovery=g->rate;g->burn=3600;
}
Byte game_command(Game *g,const char *s){Word p,previous;Byte *o;int r,c;Byte result=GAME_OK;
 if(g->faint||g->dead)return GAME_FAINT;
 if(!strcmp(s,"TURN LEFT"))g->dir=(g->dir-1)&3;
 else if(!strcmp(s,"TURN RIGHT"))g->dir=(g->dir+1)&3;
 else if(!strcmp(s,"TURN AROUND"))g->dir=(g->dir+2)&3;
 else if(!strcmp(s,"MOVE")){r=g->row+dr[g->dir];c=g->col+dc[g->dir];if(cell(g,r,c)==255)result=GAME_BLOCKED;else {g->row=r;g->col=c;}
  g->damage=(Word)(g->damage+(g->weight>>3)+3);health(g);
 }else if(!strcmp(s,"PULL LEFT TORCH")){if(g->hand)return GAME_INVALID;p=g->bag;previous=0;
  while(p&&ocb(g,p)[10]!=5){previous=p;p=getword(ocb(g,p));}if(!p)return GAME_INVALID;
  if(previous)putword(ocb(g,previous),getword(ocb(g,p)));else g->bag=getword(ocb(g,p));g->hand=p;if(g->torch==p)g->torch=0;
 }else if(!strcmp(s,"USE LEFT")){if(!g->hand||ocb(g,g->hand)[10]!=5)return GAME_INVALID;
  g->torch=g->hand;o=ocb(g,g->hand);putword(o,g->bag);g->bag=g->hand;g->hand=0;
 }else if(strcmp(s,"LOOK"))return GAME_INVALID;
 g->lit=g->torch!=0;return result;
}
void game_tick(Game *g,Word ticks){Byte *o;while(ticks--){
 if(!--g->recovery){g->damage=g->damage-(g->damage+63)/64;health(g);g->recovery=g->rate?g->rate:256;}
 if(!--g->burn){g->burn=3600;if(g->torch){o=ocb(g,g->torch);if(o[6]){--o[6];if(o[6]<=5){o[9]=24;o[11]=0;}if(o[6]<o[7])o[7]=o[6];if(o[6]<o[8])o[8]=o[6];}}}
}}
/* VIEWER/VCTLST: absolute endpoint scaling about (128,76), signed arithmetic
 * shift by seven; VECTOR retains original fade sampling and clipping. */
#include "logical.h"
static int scale(Byte x,int centre,Byte factor){int d=(int)x-centre;long v=(long)d*factor;return centre+(int)(v>=0?v/128:-((-v+127)/128));}
static void draw(Byte *frame,Byte list,Byte factor,Byte light,Byte range){Word i,end;int diff=(int)light-7-range;Byte fade;
 if(diff<=-7)return;fade=diff>=0?0:(1<<(-diff-1));
 i=game_lists[list][0];end=i+game_lists[list][1];for(;i<end;i++){
  const Byte *v=game_vectors[i];wizard_line(frame,scale(v[1],128,factor),scale(v[0],76,factor),scale(v[3],128,factor),scale(v[2],76,factor),fade);
 }}
static Byte five(const Byte *p,Word bit){Byte n=0,i;for(i=0;i<5;i++,bit++)n=(n<<1)|((p[bit/8]>>(7-bit%8))&1);return n;}
static void text(Byte *frame,const char *s,Byte row){Byte col=0,c,y;while(*s&&col<32){c=*s++;c=c>='A'&&c<='Z'?c-'A'+1:0;for(y=0;y<7;y++)frame[((Word)row+y)*32+col]=five(font+c*5,5+y*5)<<2;col++;}}
void game_render_input(Byte *frame,const char *input,const Byte *underlay){
 memcpy(frame+GAME_INPUT_OFFSET,underlay,GAME_INPUT_BYTES);text(frame,input,184);
}
void game_render(Game *g,Byte *frame,const char *input,const char *message){
 static const Byte scales[]={200,128,80,50,31,20,12,8,4,2};
 Byte r=g->row,c=g->col,range,side,feature,relative,light=0,magic=0,i,which;int cr;
 memset(frame,0,FRAME_BYTES);
 if(g->torch){light=ocb(g,g->torch)[7];magic=ocb(g,g->torch)[8];}
 if(!g->faint&&!g->dead)for(range=0;range<10;range++){
  for(side=0;side<3;side++){relative=draw_order[side];feature=(cell(g,r,c)>>(((g->dir+relative)&3)*2))&3;
   if(feature==2){draw(frame,side*4+2,scales[range],magic,range);feature=3;}
   draw(frame,side*4+feature,scales[range],light,range);
  }
  cr=creature(g,r,c);if(cr>=0)draw(frame,19+g->creatures[cr][13],scales[range],g->creatures[cr][2]?magic:light,range);
  for(side=0;side<2;side++){relative=side?1:3;which=(g->dir+relative)&3;
   if(!((cell(g,r,c)>>(which*2))&3)){cr=creature(g,r+dr[which],c+dc[which]);if(cr>=0)draw(frame,13+side,scales[range],g->creatures[cr][2]?magic:light,range);}}
  which=12;for(i=0;i<sizeof(vertical);i+=3)if(vertical[i+1]==r&&vertical[i+2]==c){which=15+vertical[i];break;}
  draw(frame,which,scales[range],light,range);
  /* Initial objects are player/creature-owned. No DROP/combat can create
   * floor objects in this bounded slice, so OFIND has no unowned matches. */
  if((cell(g,r,c)>>(g->dir*2))&3)break;r+=dr[g->dir];c+=dc[g->dir];
 }
 /* Isolated port UI: original font, not a claimed port of STATUS/TXTSER. */
 text(frame,g->faint?"FAINT":g->lit?"PINE TORCH LIT":"DARK  PULL LEFT TORCH",152);
 text(frame,message,168);text(frame,input,184);
}
