#include <assert.h>
#include <stdio.h>
#include "heartbeat.h"
int main(void) {
 HeartbeatState s;Word j=999;unsigned checks=0;
 assert(heartbeat_player_interval(160,0,&j)&&j==46);++checks;
 assert(heartbeat_player_interval(160,40,&j)&&j==24);++checks;
 assert(heartbeat_player_interval(160,80,&j)&&j==14);++checks;
 assert(heartbeat_player_interval(160,128,&j)&&j==6);++checks;
 assert(heartbeat_player_interval(160,160,&j)&&j==3);++checks;
 assert(heartbeat_player_interval(65535,65535,&j)&&j==3);++checks;
 j=999;assert(!heartbeat_player_interval(0,0,&j)&&j==999);++checks;
 assert(!heartbeat_player_interval(160,161,&j)&&j==999);++checks;
 heartbeat_init(&s);assert(!heartbeat_advance(&s,100)&&s.remaining==1);++checks;
 heartbeat_update(&s,46);assert(heartbeat_advance(&s,1)==1&&s.level==1&&s.remaining==46);++checks;
 assert(!heartbeat_advance(&s,20)&&s.remaining==26);++checks;
 heartbeat_update(&s,3);assert(s.remaining==26);++checks;
 heartbeat_update(&s,3);assert(s.remaining==26);++checks;
 assert(!heartbeat_advance(&s,25)&&s.remaining==1);++checks;
 assert(heartbeat_advance(&s,1)==1&&s.remaining==3&&s.level==0);++checks;
 heartbeat_disable(&s);assert(!heartbeat_advance(&s,100)&&s.remaining==3);++checks;
 heartbeat_update(&s,3);assert(heartbeat_advance(&s,30)==10&&s.remaining==3&&s.level==0);++checks;
 heartbeat_init(&s);heartbeat_update(&s,0);assert(heartbeat_advance(&s,1)==1&&s.remaining==256);++checks;
 assert(!heartbeat_advance(&s,255)&&heartbeat_advance(&s,1)==1&&s.level==0);++checks;
 /* Bulk advancement must match literal byte-decrement source behavior. */
 for(unsigned rate=0;rate<256;rate++) {
  Byte counter=1,level=0;unsigned edges=0;
  heartbeat_init(&s);heartbeat_update(&s,(Byte)rate);
  for(unsigned t=0;t<1000;t++)if(!--counter){counter=(Byte)rate;level^=1;++edges;}
  assert(heartbeat_advance(&s,1000)==edges&&s.level==level);
  assert(s.remaining==(counter?counter:256));
 }++checks;
 printf("%u heartbeat source-semantic checks passed (including all 256 rate bytes)\n",checks);
 return 0;
}
