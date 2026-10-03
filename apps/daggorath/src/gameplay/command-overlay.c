/* Low-frequency Daggorath command subsystem for the Level II Sbrtn+Objct
 * overlay.  This file has no mutable globals, no host imports and no C
 * runtime calls: all durable state belongs to the resident Game supplied in
 * DagOverlayContextV1.  Source fidelity is retained from the pre-extraction
 * game.c implementation, itself pinned to PATTK/PGET/PEXAM/PARSER. */
#include "overlay-api.h"
#include "overlay_data.h"
#include "audio/audio.h"

#define OBASE 0x0b15
#define FRAME_BYTES 6144

static const signed char dr[]={-1,0,1,0},dc[]={0,1,0,-1};
static Word getword(const Byte *p){return (Word)p[0]*256+p[1];}
static void putword(Byte *p,Word w){p[0]=w>>8;p[1]=w&255;}
static Byte *ocb(Game *g,Word p){return g->objects[(p-OBASE)/14];}
static Byte valid_ocb(const Game *g,Word p){return p>=OBASE&&p<OBASE+(Word)g->count*14&&((p-OBASE)%14)==0;}
static Byte cell(Game *g,int r,int c){return r<0||r>31||c<0||c>31?255:g->maze[(Word)r*32+c];}
static Byte on_floor(const Byte *o,Byte r,Byte c){return !o[4]&&!o[5]&&o[2]==r&&o[3]==c;}

static Byte random_byte(Game *g){Byte n,i,b,carry,next;
 for(n=0;n<8;n++){b=g->seed[2]&0xe1;carry=0;for(i=0;i<8;i++){carry^=b&1;b>>=1;}
  for(i=0;i<3;i++){next=g->seed[i]>>7;g->seed[i]=(g->seed[i]<<1)|carry;carry=next;}}
 return g->seed[0];
}
static void clear_combat(GameCombat *c){Byte i;
 if(!c)return;
 for(i=0;i<sizeof(*c);i++)((Byte *)c)[i]=0;
 c->target=255;
}
static void combat_event(GameCombat *c,Byte event){if(c&&c->eventCount<3)c->events[c->eventCount++]=event;}
static Byte equal(const char *a,const char *b){while(*a&&*a==*b){++a;++b;}return *a==*b;}

/* PARSER.ASM GETTOK/PARSE0/PAROBJ/PARHND.  The byte tables are a deliberately
 * small generated subset: no broad game resource table is copied into this
 * callable module. */
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
static Word scale16(Word value,Byte radix){
 /* Exact unsigned product/128 without pulling CMOC's 32-bit helpers. */
 Word low=0,old;Byte high=0;
 while(radix--){old=low;low=(Word)(low+value);if(low<old)++high;}
 return (Word)(((Word)high<<9)|(low>>7));
}

static Byte bag_command(Game *g,const char *s,const DagOverlayServices *services){Byte t[33],specific=0;int cmd,dir,kind,cls;Word p,previous,*hand;Byte i,*o;
 s=token(s,t);cmd=classify(t,parser_commands,sizeof(parser_commands)/16);
 if(cmd!=PAR_PULL&&cmd!=PAR_STOW&&cmd!=PAR_GET&&cmd!=PAR_DROP)return GAME_INVALID;
 s=token(s,t);dir=classify(t,parser_directions,sizeof(parser_directions)/16);
 if(dir==PAR_LT)hand=&g->hand;else if(dir==PAR_RT)hand=&g->rightHand;else return GAME_INVALID;
 if(cmd==PAR_DROP){if(!*hand)return GAME_INVALID;o=ocb(g,*hand);*hand=0;
  o[5]=0;o[2]=g->row;o[3]=g->col;o[4]=g->level;
  g->weight=(Word)(g->weight+(signed char)(Byte)(0-object_weights[o[10]]));services->health(services->opaque,g);return GAME_OK;}
 if(cmd==PAR_STOW){if(!*hand)return GAME_INVALID;
  putword(ocb(g,*hand),g->bag);g->bag=*hand;*hand=0;return GAME_OK;}
 if(*hand)return GAME_INVALID;
 s=token(s,t);kind=classify(t,status_generics,sizeof(status_generics)/16);
 if(kind>=0)cls=status_generics_classes[kind];
 else {specific=1;kind=classify(t,status_adjectives,sizeof(status_adjectives)/16);if(kind<0)return GAME_INVALID;
  cls=status_adjectives_classes[kind];token(s,t);dir=classify(t,status_generics,sizeof(status_generics)/16);
  if(dir<0||status_generics_classes[dir]!=cls)return GAME_INVALID;}
 if(cmd==PAR_GET){for(i=0;i<g->count;i++){o=g->objects[i];
   if(on_floor(o,g->row,g->col)&&(specific?o[9]==kind:o[10]==cls)){
    *hand=OBASE+(Word)i*14;++o[5];g->weight=(Word)(g->weight+object_weights[o[10]]);services->health(services->opaque,g);return GAME_OK;}}
  return GAME_INVALID;}
 previous=0;p=g->bag;
 while(p&&(specific?ocb(g,p)[9]!=kind:ocb(g,p)[10]!=cls)){previous=p;p=getword(ocb(g,p));}
 if(!p)return GAME_INVALID;
 if(previous)putword(ocb(g,previous),getword(ocb(g,p)));else g->bag=getword(ocb(g,p));
 *hand=p;if(g->torch==p)g->torch=0;return GAME_OK;
}
static Byte display_command(const char *s){Byte t[33];int cmd;
 token(s,t);cmd=classify(t,parser_commands,sizeof(parser_commands)/16);
 return cmd==PAR_EXAM?GAME_VIEW_EXAMINE:cmd==PAR_LOOK?GAME_VIEW_DUNGEON:GAME_VIEW_KEEP;
}
static const char *message_for(const char *s,Byte result){Byte t[33];int cmd;
 token(s,t);cmd=classify(t,parser_commands,sizeof(parser_commands)/16);
 if((cmd==PAR_GET||cmd==PAR_DROP||cmd==PAR_EXAM||cmd==PAR_LOOK)&&result!=GAME_FAINT)return result==GAME_INVALID?"???":"";
 return result==GAME_BLOCKED?"BLOCKED":result==GAME_INVALID?"UNKNOWN COMMAND":result==GAME_FAINT?"FAINT":"OK";
}

static Byte attack(Game *g,const char *s,GameCombat *combat,const DagOverlayServices *services){Byte t[33],*weapon=0,*creature=0,i,index=15,mgo=0,pho=5,kind=0,cls=4,random;
 Word *hand,power,remaining,value,energy,magic,physical,next;int adjustment,score;Byte ring=0;
 clear_combat(combat);
 s=token(s,t);if(classify(t,parser_commands,sizeof(parser_commands)/16)!=PAR_ATTK)return 255;
 token(s,t);i=(Byte)classify(t,parser_directions,sizeof(parser_directions)/16);
 if(i==PAR_LT)hand=&g->hand;else if(i==PAR_RT)hand=&g->rightHand;else return GAME_INVALID;
 if(*hand){if(!valid_ocb(g,*hand))return GAME_INVALID;weapon=ocb(g,*hand);kind=weapon[9];cls=weapon[10];mgo=weapon[12];pho=weapon[13];}
 energy=scale16(g->power,(Byte)(((Word)mgo+pho)>>3));g->damage=(Word)(g->damage+energy);
 if(combat)combat->energy=energy;combat_event(combat,(Byte)(AUDIO_GLUGLG+cls));
 if(weapon&&kind>=19&&kind<=21){ring=1;if(--weapon[6]==0)weapon[9]=22;}
 for(i=0;i<32;i++)if(g->creatures[i][12]&&g->creatures[i][15]==g->row&&g->creatures[i][16]==g->col){creature=g->creatures[i];break;}
 if(!creature){services->health(services->opaque,g);return GAME_OK;}if(combat)combat->target=i;
 if(!ring){
  remaining=(Word)(getword(creature)-getword(creature+10));value=(Word)(remaining<<2);
  do{Word old=value;value=(Word)(value-g->power);if(old<g->power)break;--index;}while(index);
  adjustment=index>=3?(index-3)*10:-(3-index)*25;
  random=random_byte(g);if(combat)++combat->rngCalls;score=(int)random+adjustment-127;
  if(combat)combat->hitValue=(Word)score;
  if(score<0){services->health(services->opaque,g);return GAME_OK;}
  if(!g->torch||!valid_ocb(g,g->torch)||ocb(g,g->torch)[9]==24){random=random_byte(g);if(combat)++combat->rngCalls;if(random&3){services->health(services->opaque,g);return GAME_OK;}}
 }
 if(combat)combat->hit=1;combat_event(combat,AUDIO_KLINK);
 power=g->power;magic=scale16(scale16(power,mgo),creature[3]);physical=scale16(scale16(power,pho),creature[5]);
 value=(Word)(getword(creature+10)+magic+physical);putword(creature+10,value);
 if(combat)combat->damage=(Word)(magic+physical);
 if(value<getword(creature)){services->health(services->opaque,g);return GAME_OK;}
 if(combat)combat->killed=1;next=getword(creature+8);for(i=0;next&&i<72;i++){
  if(!valid_ocb(g,next))break;weapon=ocb(g,next);weapon[5]=0;weapon[2]=creature[15];weapon[3]=creature[16];next=getword(weapon);
 }
 creature[12]=0;combat_event(combat,AUDIO_BANG);
 next=(Word)(g->power+(getword(creature)>>3));g->power=(next&0x8000)?(Word)(0x7f00|(next&255)):next;
 services->health(services->opaque,g);return GAME_OK;
}
static Byte command(Game *g,const char *s,GameCombat *combat,const DagOverlayServices *services){Word *hand;Byte *o,attackResult;int r,c;Byte result=GAME_OK;
 clear_combat(combat);if(g->faint||g->dead)return GAME_FAINT;
 attackResult=attack(g,s,combat,services);if(attackResult!=255)return attackResult;
 if(equal(s,"TURN LEFT"))g->dir=(g->dir-1)&3;
 else if(equal(s,"TURN RIGHT"))g->dir=(g->dir+1)&3;
 else if(equal(s,"TURN AROUND"))g->dir=(g->dir+2)&3;
 else if(equal(s,"MOVE")){r=g->row+dr[g->dir];c=g->col+dc[g->dir];if(cell(g,r,c)==255)result=GAME_BLOCKED;else {g->row=r;g->col=c;}
  g->damage=(Word)(g->damage+(g->weight>>3)+3);services->health(services->opaque,g);
 }else if(equal(s,"USE LEFT")||equal(s,"USE RIGHT")){hand=equal(s,"USE LEFT")?&g->hand:&g->rightHand;if(!*hand||ocb(g,*hand)[10]!=5)return GAME_INVALID;
  g->torch=*hand;o=ocb(g,*hand);putword(o,g->bag);g->bag=*hand;*hand=0;
 }else if(!display_command(s)){if(bag_command(g,s,services)!=GAME_OK)return GAME_INVALID;}
 g->lit=g->torch!=0;return result;
}

typedef struct { Byte *frame;Word cursor;Byte inverse,pair;const Byte *font;
 Byte (*progress)(void *);void *progressContext;Byte progressError; } ExamineText;
static Byte five(const Byte *p,Word bit){Byte n=0,i;for(i=0;i<5;i++,bit++)n=(n<<1)|((p[bit/8]>>(7-bit%8))&1);return n;}
static void examine_char(ExamineText *t,Byte c){Byte y;Word at,i;
 if(c==31)t->cursor=(t->cursor+32)&0xffe0;
 else {at=(t->cursor/32)*256+(t->cursor&31);for(y=0;y<7;y++)t->frame[at+(Word)y*32]=(five(t->font+c*5,5+y*5)<<2)^t->inverse;++t->cursor;}
 if(t->cursor>=608){for(i=0;i<18*256;i++)t->frame[i]=t->frame[i+256];for(i=18*256;i<19*256;i++)t->frame[i]=t->inverse;t->cursor=576;}
 if(!t->progressError&&t->progress)t->progressError=t->progress(t->progressContext);
}
static void examine_string(ExamineText *t,const char *s){Byte c;while(*s){c=*s++;examine_char(t,c=='^'?31:c=='!'?27:c==' '?0:c-'A'+1);}}
static void examine_object(Game *g,ExamineText *t,Word p,const DagOverlayServices *services){Byte name[32],n,i;
 n=services->object_name(services->opaque,g,p,name);for(i=0;i<n;i++)examine_char(t,name[i]);t->inverse=0;t->pair=!t->pair;if(t->pair)t->cursor=(t->cursor+16)&0xfff0;else examine_char(t,31);
}
static void text(Byte *frame,const char *s,Byte row,const Byte *font){Byte col=0,c,y;while(*s&&col<32){c=*s++;c=c>='A'&&c<='Z'?c-'A'+1:c=='?'?29:0;for(y=0;y<7;y++)frame[((Word)row+y)*32+col]=five(font+c*5,5+y*5)<<2;++col;}}
static void input_line(Byte *frame,const char *input,const Byte *font){Byte n=0,y;text(frame,input,184,font);while(input[n]&&n<31)n++;if(n<32)for(y=0;y<7;y++)frame[(184+(Word)y)*32+n]=five(font+28*5,5+y*5)<<2;}
static Byte examine(Game *g,Byte *frame,const char *input,const char *message,const DagOverlayServices *services,
 Byte (*progress)(void *),void *progressContext){ExamineText t;Byte i;Word p;
 for(p=0;p<FRAME_BYTES;p++)frame[p]=0;t.frame=frame;t.cursor=10;t.inverse=0;t.pair=0;t.font=font;
 t.progress=progress;t.progressContext=progressContext;t.progressError=0;
 examine_string(&t,"IN THIS ROOM^");
 for(i=0;i<32;i++)if(g->creatures[i][12]&&g->creatures[i][15]==g->row&&g->creatures[i][16]==g->col){t.cursor+=11;examine_string(&t,"!CREATURE!^");break;}
 for(i=0;i<g->count;i++)if(on_floor(g->objects[i],g->row,g->col))examine_object(g,&t,OBASE+(Word)i*14,services);
 if(t.pair){examine_char(&t,31);t.pair=0;}for(i=0;i<32;i++)examine_char(&t,27);t.cursor+=12;examine_string(&t,"BACKPACK^");
 for(p=g->bag;p;p=getword(ocb(g,p))){if(p==g->torch)t.inverse=255;examine_object(g,&t,p,services);}
 services->render_status(services->opaque,g,frame,0);text(frame,message,168,font);input_line(frame,input,font);
 return t.progressError;
}

/* Called only by the module entry shim.  An ABI failure occurs before command
 * execution, so the resident Game cannot be partially mutated by setup. */
Byte dod_overlay_execute(DagOverlayContextV1 *ctx){
 if(!ctx||ctx->abiVersion!=DOD_OVERLAY_ABI_V1||ctx->contextSize!=sizeof(*ctx)||!ctx->game||!ctx->services||ctx->services->version!=DOD_OVERLAY_ABI_V1||ctx->services->size!=sizeof(DagOverlayServices)||!ctx->services->opaque||!ctx->services->health||!ctx->services->object_name||!ctx->services->render_status)return 187;
 if(ctx->operation==DOD_OVERLAY_COMMAND){ctx->result=command(ctx->game,ctx->command,ctx->combat,ctx->services);ctx->view=display_command(ctx->command);ctx->outputMessage=message_for(ctx->command,ctx->result);return 0;}
 if(ctx->operation==DOD_OVERLAY_EXAMINE)return examine(ctx->game,ctx->frame,ctx->input,ctx->message,
   ctx->services,ctx->progress,ctx->progressContext);
 return 187;
}
