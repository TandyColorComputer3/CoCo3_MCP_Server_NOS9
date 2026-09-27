#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "gameplay/game.h"
#include "logical.h"
static Game g,other;static Byte frame[FRAME_BYTES];
int main(int argc,char **argv){unsigned i,count=0;Byte r,c,dir;Word damage;unsigned long sum=0;
 game_init(&g,0);game_init(&other,0);assert(!memcmp(&g,&other,sizeof(g)));
 assert(g.row==16&&g.col==11&&g.dir==0&&g.power==160&&g.damage==0&&g.rate==46);
 assert(g.count==65&&g.creatureCount==24);assert(!g.torch&&!g.hand&&g.bag);
 for(i=0;i<1024;i++)count+=g.maze[i]!=255;assert(count==500);
 game_render(&g,frame,"","");for(i=0;i<152*32;i++)assert(frame[i]==0);
 assert(game_command(&g,"GET LEFT TORCH")==GAME_INVALID);
 assert(game_command(&g,"USE LEFT")==GAME_INVALID);
 assert(game_command(&g,"PULL LEFT TORCH")==GAME_OK);assert(g.hand&&!g.torch);
 assert(game_command(&g,"PULL LEFT TORCH")==GAME_INVALID);
 assert(game_command(&g,"USE LEFT")==GAME_OK);assert(g.torch&&!g.hand);
 game_render(&g,frame,"","");for(i=0;i<152*32;i++)sum+=frame[i];assert(sum>0);
 assert(game_command(&g,"TURN LEFT")==GAME_OK&&g.dir==3);
 assert(game_command(&g,"TURN RIGHT")==GAME_OK&&g.dir==0);
 assert(game_command(&g,"TURN AROUND")==GAME_OK&&g.dir==2);
 r=g.row;c=g.col;dir=g.dir;damage=g.damage;
 /* Check legalness against packed source maze, without inventing a corridor. */
 for(i=0;i<4;i++){static const int dr[]={-1,0,1,0},dc[]={0,1,0,-1};int rr=r+dr[i],cc=c+dc[i];Byte expected;
  g.row=r;g.col=c;g.dir=i;g.damage=0;g.faint=0;g.dead=0;
  expected=rr<0||rr>31||cc<0||cc>31||g.maze[rr*32+cc]==255?GAME_BLOCKED:GAME_OK;
  assert(game_command(&g,"MOVE")==expected);assert(g.damage==7);
  assert(g.row==(expected==GAME_OK?rr:r)&&g.col==(expected==GAME_OK?cc:c));
 }
 game_init(&g,0);g.damage=7;game_tick(&g,46);assert(g.damage==6);
 game_init(&g,0);game_command(&g,"PULL LEFT TORCH");game_command(&g,"USE LEFT");game_tick(&g,3600);assert(g.objects[(g.torch-0xb15)/14][6]==14);
 game_init(&g,0);
 if(argc==2){FILE *f=fopen(argv[1],"wb");assert(f);fwrite(g.maze,1,1024,f);fwrite(g.objects,1,1008,f);fwrite(g.creatures,1,544,f);fclose(f);}
 puts("gameplay source-state checks passed");return 0;
}
