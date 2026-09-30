/* Original-derived GAME/DGNGEN/NEWLVX/OBIRTX/CBIRTH/PPULL/PUSE/PTURN/
 * PMOVE/HUPDAX/HSLOW/BURNER/PGET/PDROP/OFIND/VIEW52. Pinned paths/labels: import_gameplay.py and
 * DAGGORATH_GAMEPLAY_M1.md. No hardware, OS calls, physical coordinates or AI. */
#ifdef _CMOC_VERSION_
#include <cmoc.h>
#else
#include <string.h>
#endif
#include "game.h"
#include "game_data.h"
#include "audio/audio.h"
static const signed char dr[]={-1,0,1,0},dc[]={0,1,0,-1};
static Word getword(const Byte *p){return (Word)p[0]*256+p[1];}
static void putword(Byte *p,Word w){p[0]=w>>8;p[1]=w&255;}
#define OBASE 0x0b15
static Byte *ocb(Game *g,Word p){return g->objects[(p-OBASE)/14];}
#ifndef DOD_COMMAND_OVERLAY
static Byte valid_ocb(const Game *g,Word p){return p>=OBASE&&p<OBASE+(Word)g->count*14&&((p-OBASE)%14)==0;}
#endif
Byte game_random(Game *g){Byte n,i,b,carry,next;
 for(n=0;n<8;n++){b=g->seed[2]&0xe1;carry=0;for(i=0;i<8;i++){carry^=b&1;b>>=1;}
  for(i=0;i<3;i++){next=g->seed[i]>>7;g->seed[i]=(g->seed[i]<<1)|carry;carry=next;}}
 return g->seed[0];}
static Byte cell(Game *g,int r,int c){return r<0||r>31||c<0||c>31?255:g->maze[(Word)r*32+c];}
static void random_cell(Game *g,Byte *r,Byte *c){*c=game_random(g)&31;*r=game_random(g)&31;}
static void maze(Game *g,Byte level,Byte second){Byte r,c,dir,dist,n[9],x,y,kind;int nr,nc;Word left,i;
 memset(g->maze,255,1024);g->seed[0]=level_seeds[level];g->seed[1]=level_seeds[level+1];g->seed[2]=level_seeds[level+2];
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
static int creature(Game *g,Byte r,Byte c){Byte i;for(i=0;i<g->creatureCount;i++)if(g->creatures[i][12]&&g->creatures[i][15]==r&&g->creatures[i][16]==c)return i;return -1;}
/* Exact increment-before-borrow quotient; 24-bit quantities fit unsigned long.
 * Original faint/death flags retained; visual faint transitions are not here. */
void game_health(Game *g){unsigned long numerator=(unsigned long)g->power*64,den=(unsigned long)g->power+2UL*g->damage;
 g->rate=(Byte)((numerator/den+1)-19);
 if(!g->faint&&(signed char)g->rate<=3)g->faint=1;
 else if(g->faint&&(signed char)g->rate>4)g->faint=0;
 g->dead=g->power<g->damage;
}
#ifndef DOD_COMMAND_OVERLAY
Byte game_population(const Game *g,Byte type){Byte i,n=0;
 for(i=0;i<32;i++)if(g->creatures[i][12]&&g->creatures[i][13]==type)++n;
 return n;
}
#endif
/* ONCE:GAME20/GAME30/GAME40 then NEWLVL.  The normal and demo paths share
 * packed OCB/CCB construction; only source-selected LEVEL/player data differ. */
static void init_level(Game *g,Byte level,Byte second,const Byte *initial,
                       Byte initialLevel,Byte row,Byte col,Word power){Byte type,objectLevel,n,r,c,i;int t;Word p,tail;Byte *o,*cr;
 memset(g,0,sizeof(*g));g->level=level;g->row=row;g->col=col;g->power=power;g->weight=35;
 for(type=0;type<sizeof(omx);type++){objectLevel=omx[type]>>4;n=omx[type]&15;while(n--){p=birth(g,type,objectLevel);ocb(g,p)[5]=255;if(++objectLevel>5)objectLevel=omx[type]>>4;}}
 maze(g,level,second);
 for(t=11;t>=0;t--)for(n=0;n<level_cmt[(Word)level*12+t];n++){
  do{random_cell(g,&r,&c);}while(cell(g,r,c)==255||creature(g,r,c)>=0);
  cr=g->creatures[g->creatureCount++];memcpy(cr,cdb+t*8,8);cr[12]=255;cr[13]=t;cr[15]=r;cr[16]=c;
 }
 i=0;for(n=0;n<g->count;n++){o=g->objects[n];if(o[4]!=level||o[5]!=255)continue;
  cr=g->creatures[i];putword(o,getword(cr+8));putword(cr+8,OBASE+(Word)n*14);if(++i==g->creatureCount)i=0;}
 tail=0;for(i=0;initial[i]!=255;i++){/* GAME10 leaves B=11; SWI preserves it through NEWLVX/GAME30. */
 p=birth(g,initial[i],initialLevel);o=ocb(g,p);o[5]=1;fill(o,initial[i]);o[11]=0;
  if(tail)putword(ocb(g,tail),p);else g->bag=p;tail=p;}
 game_health(g);g->recovery=g->rate;g->burn=3600;
}
void game_init(Game *g,Byte second){static const Byte gamdat[]={17,15,255};init_level(g,0,second,gamdat,11,16,11,160);}
/* GAME30 leaves the original's B register at $3C after the level-two NEWLVL
 * path; OBIRTH stores that byte in each DEMDAT OCB's level field.  Keep it as
 * captured source state rather than normalizing it to the logical LEVEL. */
void game_init_demo(Game *g,Byte second){static const Byte demdat[]={13,15,16,255};init_level(g,2,second,demdat,0x3c,12,22,6048);}
/* COMCRE:OFIND/FNDOBJ: this helper remains resident because the ordinary
 * dungeon renderer uses it on every visible object pass.  The command overlay
 * receives it through its v1 host-services table rather than duplicating a
 * second interpretation of packed OCB ownership. */
static Byte on_floor(const Byte *o,Byte r,Byte c){return !o[4]&&!o[5]&&o[2]==r&&o[3]==c;}
/* The command parser/Combat/bag implementation is compiled into the production
 * Sbrtn+Objct overlay. Host fidelity tests keep the direct implementation here;
 * the resident build uses overlay-host.c through the documented ABI. */
#ifndef DOD_COMMAND_OVERLAY
/* PARSER.ASM GETTOK/PARSE0/PAROBJ/PARHND and PGET.ASM PPULL/PSTOW.
 * Original HUMAN converts nonletters to spaces; host strings end at NUL.
 * CD.ASM TOKEN permits 32 five-bit characters. Unique prefix only: a second
 * match fails, even if another entry matched fully. No heap or shared scratch. */
static const char *token(const char *s,Byte *t){Byte n=0,c;
 while(*s&&(*s<'A'||*s>'Z'))++s;
 while(*s&&n<32){c=*s++;if(c<'A'||c>'Z')break;t[n++]=c-'A'+1;}
 t[n]=255;return s;
}
static int classify(const Byte *t,const Byte table[][16],Byte count){int found=-1;Byte i,j;
 if(t[0]==255)return -1;
 for(i=0;i<count;i++){j=0;while(t[j]!=255&&j<15&&t[j]==table[i][j])++j;
  if(t[j]==255){if(found>=0)return -1;found=i;}}
 return found;
}
static Byte bag_command(Game *g,const char *s){Byte t[33],specific=0;int cmd,dir,kind,cls;Word p,previous,*hand;Byte i,*o;
 s=token(s,t);cmd=classify(t,parser_commands,sizeof(parser_commands)/16);
 if(cmd!=PAR_PULL&&cmd!=PAR_STOW&&cmd!=PAR_GET&&cmd!=PAR_DROP)return GAME_INVALID;
 s=token(s,t);dir=classify(t,parser_directions,sizeof(parser_directions)/16);
 if(dir==PAR_LT)hand=&g->hand;else if(dir==PAR_RT)hand=&g->rightHand;else return GAME_INVALID;
 /* PGET:PDROP/WUPDAT leaves next link/fuel/reveal untouched. No floor chain. */
 if(cmd==PAR_DROP){if(!*hand)return GAME_INVALID;o=ocb(g,*hand);*hand=0;
  o[5]=0;o[2]=g->row;o[3]=g->col;o[4]=g->level;
  g->weight=(Word)(g->weight+(signed char)(Byte)(0-object_weights[o[10]]));game_health(g);return GAME_OK;}
 if(cmd==PAR_STOW){if(!*hand)return GAME_INVALID;
  putword(ocb(g,*hand),g->bag);g->bag=*hand;*hand=0;return GAME_OK;}
 if(*hand)return GAME_INVALID;
 s=token(s,t);kind=classify(t,status_generics,sizeof(status_generics)/16);
 if(kind>=0)cls=status_generics_classes[kind];
 else {specific=1;kind=classify(t,status_adjectives,sizeof(status_adjectives)/16);if(kind<0)return GAME_INVALID;
  cls=status_adjectives_classes[kind];token(s,t);dir=classify(t,status_generics,sizeof(status_generics)/16);
  if(dir<0||status_generics_classes[dir]!=cls)return GAME_INVALID;}
 /* PGET20: first class/type match in OFIND order; GET30 INC owner, add weight. */
 if(cmd==PAR_GET){for(i=0;i<g->count;i++){o=g->objects[i];
   if(on_floor(o,g->row,g->col)&&(specific?o[9]==kind:o[10]==cls)){
    *hand=OBASE+(Word)i*14;++o[5];g->weight=(Word)(g->weight+object_weights[o[10]]);game_health(g);return GAME_OK;}}
  return GAME_INVALID;}
 previous=0;p=g->bag;
 while(p&&(specific?ocb(g,p)[9]!=kind:ocb(g,p)[10]!=cls)){previous=p;p=getword(ocb(g,p));}
 if(!p)return GAME_INVALID;
 if(previous)putword(ocb(g,previous),getword(ocb(g,p)));else g->bag=getword(ocb(g,p));
 *hand=p;if(g->torch==p)g->torch=0;
 /* PULL leaves the removed OCB's next link intact. No owner/weight/RNG change.
  * HUMAN:HMAN70 flushes unconsumed tokens after the handler returns. */
 return GAME_OK;
}
/* PEXAM/PLOOK set DSPMOD without parsing operands; HUMAN discards trailing
 * tokens. Full CMDTAB classification retains unique-prefix ambiguity rules. */
Byte game_display_command(const char *s){Byte t[33];int cmd;
 token(s,t);cmd=classify(t,parser_commands,sizeof(parser_commands)/16);
 return cmd==PAR_EXAM?GAME_VIEW_EXAMINE:cmd==PAR_LOOK?GAME_VIEW_DUNGEON:GAME_VIEW_KEEP;
}
/* PATTK.ASM:SCAL16 is unsigned radix-7 multiplication. The original
 * bytewise routine truncates the discarded seven low bits. */
static Word scale16(Word value,Byte radix){return (Word)(((unsigned long)value*radix)>>7);}
static void combat_event(GameCombat *c,Byte event){if(c&&c->eventCount<3)c->events[c->eventCount++]=event;}
static Byte attack(Game *g,const char *s,GameCombat *combat){Byte t[33],*weapon=0,*creature=0,i,index=15,mgo=0,pho=5,kind=0,cls=4,random;
 Word *hand,power,remaining,value,energy,magic,physical,next;int adjustment,score;Byte ring=0;
 if(combat){memset(combat,0,sizeof(*combat));combat->target=255;}
 s=token(s,t);if(classify(t,parser_commands,sizeof(parser_commands)/16)!=PAR_ATTK)return 255;
 token(s,t);i=(Byte)classify(t,parser_directions,sizeof(parser_directions)/16);
 if(i==PAR_LT)hand=&g->hand;else if(i==PAR_RT)hand=&g->rightHand;else return GAME_INVALID;
 if(*hand){if(!valid_ocb(g,*hand))return GAME_INVALID;weapon=ocb(g,*hand);kind=weapon[9];cls=weapon[10];mgo=weapon[12];pho=weapon[13];}
 /* PATTK: empty hand is initialized as sword class, magic 0, physical 5. */
 energy=scale16(g->power,(Byte)(((Word)mgo+pho)>>3));g->damage=(Word)(g->damage+energy);
 if(combat)combat->energy=energy;combat_event(combat,(Byte)(AUDIO_GLUGLG+cls));
 /* Incantable rings spend one charge before target lookup and always hit. */
 if(weapon&&kind>=19&&kind<=21){ring=1;if(--weapon[6]==0)weapon[9]=22;}
 for(i=0;i<32;i++)if(g->creatures[i][12]&&g->creatures[i][15]==g->row&&g->creatures[i][16]==g->col){creature=g->creatures[i];break;}
 if(!creature){game_health(g);return GAME_OK;}if(combat)combat->target=i;
 if(!ring){
  remaining=(Word)(getword(creature)-getword(creature+10));value=(Word)(remaining<<2);
  do{Word old=value;value=(Word)(value-g->power);if(old<g->power)break;--index;}while(index);
  adjustment=index>=3?(index-3)*10:-(3-index)*25;
  random=game_random(g);if(combat)++combat->rngCalls;score=(int)random+adjustment-127;
  if(combat)combat->hitValue=(Word)score;
  if(score<0){game_health(g);return GAME_OK;}
  /* A live torch admits the hit. Darkness consumes a second RANDOM and
   * retains the hit only for the source's one-in-four low-bit result. */
  if(!g->torch||!valid_ocb(g,g->torch)||ocb(g,g->torch)[9]==24){random=game_random(g);if(combat)++combat->rngCalls;if(random&3){game_health(g);return GAME_OK;}}
 }
 if(combat)combat->hit=1;combat_event(combat,AUDIO_KLINK);
 power=g->power;magic=scale16(scale16(power,mgo),creature[3]);
 physical=scale16(scale16(power,pho),creature[5]);
 value=(Word)(getword(creature+10)+magic+physical);putword(creature+10,value);
 if(combat)combat->damage=(Word)(magic+physical);
 if(value<getword(creature)){game_health(g);return GAME_OK;}
 if(combat)combat->killed=1;
 /* PATTK:PATT30 follows the packed address-token object list, drops each
  * object in CCB order, and deliberately leaves the dead CCB's head token. */
 next=getword(creature+8);for(i=0;next&&i<72;i++){
  if(!valid_ocb(g,next))break;weapon=ocb(g,next);weapon[5]=0;weapon[2]=creature[15];weapon[3]=creature[16];next=getword(weapon);
 }
 creature[12]=0;combat_event(combat,AUDIO_BANG);
 next=(Word)(g->power+(getword(creature)>>3));g->power=(next&0x8000)?(Word)(0x7f00|(next&255)):next;
 game_health(g);return GAME_OK;
}
Byte game_command_combat(Game *g,const char *s,GameCombat *combat){Word *hand;Byte *o,attackResult;int r,c;Byte result=GAME_OK;
 if(combat){memset(combat,0,sizeof(*combat));combat->target=255;}
 if(g->faint||g->dead)return GAME_FAINT;
 attackResult=attack(g,s,combat);if(attackResult!=255)return attackResult;
 if(!strcmp(s,"TURN LEFT"))g->dir=(g->dir-1)&3;
 else if(!strcmp(s,"TURN RIGHT"))g->dir=(g->dir+1)&3;
 else if(!strcmp(s,"TURN AROUND"))g->dir=(g->dir+2)&3;
 else if(!strcmp(s,"MOVE")){r=g->row+dr[g->dir];c=g->col+dc[g->dir];if(cell(g,r,c)==255)result=GAME_BLOCKED;else {g->row=r;g->col=c;}
  g->damage=(Word)(g->damage+(g->weight>>3)+3);game_health(g);
 }else if(!strcmp(s,"USE LEFT")||!strcmp(s,"USE RIGHT")){hand=!strcmp(s,"USE LEFT")?&g->hand:&g->rightHand;if(!*hand||ocb(g,*hand)[10]!=5)return GAME_INVALID;
  g->torch=*hand;o=ocb(g,*hand);putword(o,g->bag);g->bag=*hand;*hand=0;
 }else if(!game_display_command(s)){if(bag_command(g,s)!=GAME_OK)return GAME_INVALID;}
 g->lit=g->torch!=0;return result;
}
Byte game_command(Game *g,const char *s){return game_command_combat(g,s,0);}
/* PGET handlers print no success text; PARSER:CMDERR prints three I.QUES.
 * Preserve the older UI adapter for commands outside this slice. */
const char *game_message(const char *s,Byte result){Byte t[33];int cmd;
 token(s,t);cmd=classify(t,parser_commands,sizeof(parser_commands)/16);
 if((cmd==PAR_GET||cmd==PAR_DROP||cmd==PAR_EXAM||cmd==PAR_LOOK)&&result!=GAME_FAINT)return result==GAME_INVALID?"???":"";
 return result==GAME_BLOCKED?"BLOCKED":result==GAME_INVALID?"UNKNOWN COMMAND":result==GAME_FAINT?"FAINT":"OK";
}
#endif
/* COMMON.ASM:CLOCK first moves due TCBs from a clock queue to SCDQUE;
 * COMPLR.ASM:HSLOW/BURNER run only when the foreground scheduler gets a turn.
 * Keeping those operations separate prevents a long PLAYER:WAITX command from
 * executing HSLOW repeatedly while it still owns the source scheduler. */
#ifndef _CMOC_VERSION_
/* Host-only compatibility helpers retain direct unit-test coverage. Production
 * CMOC builds execute this source timing state solely through dodsched. */
void game_timing_init(Game *g,GameTiming *timing){
 memset(timing,0,sizeof(*timing));
 if(!g->recovery)g->recovery=g->rate?g->rate:256;
 if(!g->burn)g->burn=3600;
}
void game_timing_advance(Game *g,GameTiming *timing,Word jiffies){
 while(jiffies--){
  if(!timing->recoveryPending&&! --g->recovery)timing->recoveryPending=1;
  if(!timing->burnPending&&! --g->burn)timing->burnPending=1;
 }
}
void game_timing_service_task(Game *g,GameTiming *timing,Byte task){Byte *o;
 if(task==GAME_TIMING_HSLOW&&timing->recoveryPending){
  /* COMPLR.ASM:HSLOW is PDAM - ceil(PDAM/64), then HUPDAT and requeue. */
  g->damage=g->damage-(g->damage+63)/64;game_health(g);
  g->recovery=g->rate?g->rate:256;timing->recoveryPending=0;
 }
 if(task==GAME_TIMING_BURNER&&timing->burnPending){
  g->burn=3600;timing->burnPending=0;
  if(g->torch){o=ocb(g,g->torch);if(o[6]){--o[6];if(o[6]<=5){o[9]=24;o[11]=0;}if(o[6]<o[7])o[7]=o[6];if(o[6]<o[8])o[8]=o[6];}}
 }
}
void game_timing_service(Game *g,GameTiming *timing){
 game_timing_service_task(g,timing,GAME_TIMING_HSLOW);
 game_timing_service_task(g,timing,GAME_TIMING_BURNER);
}
/* Compatibility helper for direct host callers: one foreground scheduling
 * boundary per jiffy.  Production uses the explicit advance/service split. */
void game_tick(Game *g,Word ticks){GameTiming timing;game_timing_init(g,&timing);
 while(ticks--){game_timing_advance(g,&timing,1);game_timing_service(g,&timing);}
}
#endif
/* VIEWER/VCTLST: absolute endpoint scaling about (128,76), signed arithmetic
 * shift by seven; VECTOR retains original fade sampling and clipping. */
#include "logical.h"
static int scale(Byte x,int centre,Byte factor){int d=(int)x-centre;long v=(long)d*factor;return centre+(int)(v>=0?v/128:-((-v+127)/128));}
typedef struct {Game *game;Byte *frame;GameRenderProgress progress;void *context;Byte error;} DrawProgress;
static unsigned char draw_checkpoint(void *context){DrawProgress *p=(DrawProgress *)context;
 p->error=p->progress(p->game,p->frame,p->context);return p->error!=0;
}
static Byte draw(Byte *frame,Byte list,Byte factor,Byte light,Byte range,DrawProgress *progress){Word i,end;int diff=(int)light-7-range;Byte fade;
 if(diff<=-7)return 0;fade=diff>=0?0:(1<<(-diff-1));
 i=game_lists[list][0];end=i+game_lists[list][1];for(;i<end;i++){
  const Byte *v=game_vectors[i];int x0,y0,x1,y1;
  /* CMOC lowers each signed scale to several 32-bit helpers.  Keep no more
   * than two transforms between heartbeat service points; live HD6309
   * tracing measured a transform at about 6-7 ms. */
  if(progress&&progress->progress&&draw_checkpoint(progress))return progress->error;
  x0=scale(v[1],128,factor);y0=scale(v[0],76,factor);
  if(progress&&progress->progress&&draw_checkpoint(progress))return progress->error;
  x1=scale(v[3],128,factor);y1=scale(v[2],76,factor);
  /* CMOC initializes both 32-bit fixed-point accumulators on entry to
   * wizard_line_progress before that routine reaches its first callback.
   * Bound the measured endpoint-scale + line-setup interval here; this only
   * services the already displayed authoritative heart and cannot expose the
   * unfinished logical vector. */
  if(progress&&progress->progress&&draw_checkpoint(progress))return progress->error;
  if(progress&&progress->progress){
   if(wizard_line_progress(frame,x0,y0,x1,y1,fade,draw_checkpoint,progress))return progress->error;
  }else wizard_line(frame,x0,y0,x1,y1,fade);
 }return 0;}
static Byte five(const Byte *p,Word bit){Byte n=0,i;for(i=0;i<5;i++,bit++)n=(n<<1)|((p[bit/8]>>(7-bit%8))&1);return n;}
static Byte text_progress(Byte *frame,const char *s,Byte row,DrawProgress *progress){Byte col=0,c,y,e;while(*s&&col<32){
 c=*s++;c=c>='A'&&c<='Z'?c-'A'+1:c=='?'?29:0;
 for(y=0;y<7;y++)frame[((Word)row+y)*32+col]=five(font+c*5,5+y*5)<<2;
 ++col;if(progress&&progress->progress&&!(col&1)){e=draw_checkpoint(progress);if(e)return progress->error;}
 }return 0;}
static void text(Byte *frame,const char *s,Byte row){(void)text_progress(frame,s,row,0);}
/* HUMAN:M$CURS writes original I.BAR ($1C, underline) then I.BS, leaving
 * the cursor position in place. The original-derived font's code 28 is used. */
static void input_line(Byte *frame,const char *input){Byte n=0,y;while(input[n]&&n<31)n++;text(frame,input,184);
 if(n<32)for(y=0;y<7;y++)frame[(184+(Word)y)*32+n]=five(font+28*5,5+y*5)<<2;
}
/* STATUS:OBJNAM/COPY$, COMTXT:TXTDPB, COMDAT:STSVDB. Level zero uses
 * VDGINV=0: inverse status glyphs on a filled 256x8 strip at y=152.
 * No physical presentation coordinates or duplicate inventory state. */
Byte game_object_name(Game *g,Word token,Byte *name){
 static const Byte empty[]={5,13,16,20,25,255};
 const Byte *s;Byte *o,n=0;
 if(!token)s=empty;
 else {o=ocb(g,token);if(!o[11]){s=status_adjectives[o[9]];while(*s!=255)name[n++]=*s++;name[n++]=0;}s=status_generics[o[10]];}
 while(*s!=255)name[n++]=*s++;return n;
}
static Byte status_name(Byte *frame,const Byte *name,Byte n,Byte col,DrawProgress *progress){Byte i,y,e;
 for(i=0;i<n;i++){
  for(y=0;y<7;y++)frame[(152+(Word)y)*32+col+i]=255^(five(font+name[i]*5,5+y*5)<<2);
  if(progress&&progress->progress){e=draw_checkpoint(progress);if(e)return progress->error;}
 }
 return 0;
}
void game_heart_patterns(Byte patterns[28]){Byte i;for(i=0;i<28;i++)patterns[i]=255^status_hearts[i];}
void game_render_heart(Byte *frame,Byte phase){Byte y;const Byte *heart=status_hearts+(phase?14:0);
 for(y=0;y<7;y++){frame[(152+(Word)y)*32+15]=255^heart[y];frame[(152+(Word)y)*32+16]=255^heart[7+y];}
}
Byte game_render_status_progress(Game *g,Byte *frame,Byte phase,
                                 GameRenderProgress progress,void *context){Byte name[32],n,e;
 DrawProgress drawProgress={g,frame,progress,context,0};
 memset(frame+152*32,255,8*32);
 if(progress){e=draw_checkpoint(&drawProgress);if(e)return drawProgress.error;}
 n=game_object_name(g,g->hand,name);e=status_name(frame,name,n,0,&drawProgress);if(e)return e;
 n=game_object_name(g,g->rightHand,name);e=status_name(frame,name,n,32-n,&drawProgress);if(e)return e;
 game_render_heart(frame,phase);
 if(progress){e=draw_checkpoint(&drawProgress);if(e)return drawProgress.error;}
 return 0;
}
void game_render_status(Game *g,Byte *frame,Byte phase){
 (void)game_render_status_progress(g,frame,phase,0,0);
}
void game_render_input(Byte *frame,const char *input,const Byte *underlay){
 memcpy(frame+GAME_INPUT_OFFSET,underlay,GAME_INPUT_BYTES);input_line(frame,input);
}
Byte game_render_input_progress(Game *g,Byte *frame,const char *input,const Byte *underlay,
                                GameRenderProgress progress,void *context){Byte row,e;
 DrawProgress drawProgress={g,frame,progress,context,0};
 for(row=0;row<7;row++){
  memcpy(frame+GAME_INPUT_OFFSET+(Word)row*32,underlay+(Word)row*32,32);
  if(progress){e=draw_checkpoint(&drawProgress);if(e)return drawProgress.error;}
 }
 e=text_progress(frame,input,184,&drawProgress);if(e)return e;
 /* HUMAN:M$CURS underline after the final character. */
 {Byte n=0,y;while(input[n]&&n<31)n++;
  if(n<32)for(y=0;y<7;y++)frame[(184+(Word)y)*32+n]=five(font+28*5,5+y*5)<<2;}
 if(progress){e=draw_checkpoint(&drawProgress);if(e)return drawProgress.error;}
 return 0;
}
Byte game_render_with_progress(Game *g,Byte *frame,const char *input,
                              const char *message,GameRenderProgress progress,void *context){
 static const Byte scales[]={200,128,80,50,31,20,12,8,4,2};
 Byte r=g->row,c=g->col,range,side,feature,relative,light=0,magic=0,i,which,e;int cr;DrawProgress drawProgress={g,frame,progress,context,0};
 if(progress){Word at;
  /* The original frame clear is semantically atomic only to the unfinished
   * logical image. Clear 256-byte pieces so the already displayed heart can
   * follow a native edge while this private work proceeds. */
  for(at=0;at<FRAME_BYTES;at+=256){
   memset(frame+at,0,256);e=draw_checkpoint(&drawProgress);if(e)return drawProgress.error;
  }
 }else memset(frame,0,FRAME_BYTES);
 if(g->torch){light=ocb(g,g->torch)[7];magic=ocb(g,g->torch)[8];}
 if(!g->faint&&!g->dead)for(range=0;range<10;range++){
  for(side=0;side<3;side++){relative=draw_order[side];feature=(cell(g,r,c)>>(((g->dir+relative)&3)*2))&3;
   if(feature==2){e=draw(frame,side*4+2,scales[range],magic,range,&drawProgress);if(e)return e;feature=3;}
   e=draw(frame,side*4+feature,scales[range],light,range,&drawProgress);if(e)return e;
  }
  cr=creature(g,r,c);if(cr>=0){e=draw(frame,19+g->creatures[cr][13],scales[range],g->creatures[cr][2]?magic:light,range,&drawProgress);if(e)return e;}
  for(side=0;side<2;side++){relative=side?1:3;which=(g->dir+relative)&3;
   if(!((cell(g,r,c)>>(which*2))&3)){cr=creature(g,r+dr[which],c+dc[which]);if(cr>=0){e=draw(frame,13+side,scales[range],g->creatures[cr][2]?magic:light,range,&drawProgress);if(e)return e;}}}
  which=12;for(i=0;i<sizeof(vertical);i+=3)if(vertical[i+1]==r&&vertical[i+2]==c){which=15+vertical[i];break;}
  e=draw(frame,which,scales[range],light,range,&drawProgress);if(e)return e;
  /* VIEW52: OFIND order, FWDOBJ indexed by class, magic then regular.
   * SETFAX consumes MAGFLG on the first DRAWIT; both passes use same vectors.
   * Do this before VIEW60 stops at a forward wall/door. No orientation field. */
  for(i=0;i<g->count;i++)if(on_floor(g->objects[i],r,c)){
   which=23+g->objects[i][10];e=draw(frame,which,scales[range],magic,range,&drawProgress);if(e)return e;
   e=draw(frame,which,scales[range],light,range,&drawProgress);if(e)return e;
  }
  /* A foreground observer may update only the already displayed heart. It
   * must not upload this incomplete logical dungeon or alter game state. */
  if(progress){e=progress(g,frame,context);if(e)return e;}
  if((cell(g,r,c)>>(g->dir*2))&3)break;r+=dr[g->dir];c+=dc[g->dir];
 }
 e=game_render_status_progress(g,frame,0,progress,context);if(e)return e;
 if(progress){e=progress(g,frame,context);if(e)return e;}
 e=text_progress(frame,message,168,&drawProgress);if(e)return e;
 input_line(frame,input);
 if(progress){e=progress(g,frame,context);if(e)return e;}
 /* Progress may temporarily paint the authoritative live heart into this
  * unfinished frame. The completed logical render retains its phase-zero
  * status template; the presenter applies the latest phase before flipping. */
 game_render_heart(frame,0);
 return 0;
}
void game_render(Game *g,Byte *frame,const char *input,const char *message){
 (void)game_render_with_progress(g,frame,input,message,0,0);
}

/* PEXAM:EXAMIN/PRTOBJ/PCRLF; COMDAT:TXTEXA (32*19 chars);
 * COMTXT:TXTDPB/TXTCR/TXTSCR; TXTSER:TXTCHR. No display-only object list.
 * Level zero VDGINV=0. Only the active torch name is inverse, seven scanlines;
 * the eighth scanline is not written by TXTDPB. Scroll after each OUTCHR. */
#ifndef DOD_COMMAND_OVERLAY
typedef struct { Byte *frame;Word cursor;Byte inverse,pair; } ExamineText;
static void examine_char(ExamineText *t,Byte c){Byte y;Word at;
 if(c==31)t->cursor=(t->cursor+32)&0xffe0;
 else {at=(t->cursor/32)*256+(t->cursor&31);
  for(y=0;y<7;y++)t->frame[at+(Word)y*32]=(five(font+c*5,5+y*5)<<2)^t->inverse;
  ++t->cursor;
 }
 if(t->cursor>=608){memmove(t->frame,t->frame+256,18*256);memset(t->frame+18*256,t->inverse,256);t->cursor=576;}
}
static void examine_string(ExamineText *t,const char *s){Byte c;
 while(*s){c=*s++;examine_char(t,c=='^'?31:c=='!'?27:c==' '?0:c-'A'+1);}
}
static void examine_object(Game *g,ExamineText *t,Word p){Byte name[32],n,i;
 n=game_object_name(g,p,name);for(i=0;i<n;i++)examine_char(t,name[i]);t->inverse=0;
 t->pair=!t->pair;if(t->pair)t->cursor=(t->cursor+16)&0xfff0;else examine_char(t,31);
}
void game_render_examine(Game *g,Byte *frame,const char *input,const char *message){ExamineText t;Byte i;Word p;
 memset(frame,0,FRAME_BYTES);t.frame=frame;t.cursor=10;t.inverse=0;t.pair=0;
 examine_string(&t,"IN THIS ROOM^");
 /* COMCRE:CFIND scans all 32 slots and requires P.CCUSE, regardless of type.
  * Presence only: no name, AI, random calls, scheduler or creature mutation. */
 for(i=0;i<32;i++)if(g->creatures[i][12]&&g->creatures[i][15]==g->row&&g->creatures[i][16]==g->col){
  t.cursor+=11;examine_string(&t,"!CREATURE!^");break;}
 for(i=0;i<g->count;i++)if(on_floor(g->objects[i],g->row,g->col))examine_object(g,&t,OBASE+(Word)i*14);
 if(t.pair){examine_char(&t,31);t.pair=0;}
 for(i=0;i<32;i++)examine_char(&t,27);
 t.cursor+=12;examine_string(&t,"BACKPACK^");
 for(p=g->bag;p;p=getword(ocb(g,p))){if(p==g->torch)t.inverse=255;examine_object(g,&t,p);}
 game_render_status(g,frame,0);text(frame,message,168);input_line(frame,input);
}
#endif
