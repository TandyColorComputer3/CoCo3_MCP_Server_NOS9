#include <assert.h>
#include <stdio.h>
#include "backend.h"
#include "ssc_io.h"
static Byte bytes[512],flag;static unsigned count;
Byte ssc_read_io(Word a){(void)a;return 0xe0;}
void ssc_write_io(Word a,Byte b){if(a==0xff7e)bytes[count++]=b;}
Byte os_sleep(Word n){(void)n;return 0;}
Byte os_signal_value(Byte *p){return *p;}
int main(void){unsigned i;int checks=0;
 const unsigned sq[]={145,113,81,49},whoop[]={994,867,738,609,482,353,225,97},phaser[]={269,225,181,141,97,53};
 const unsigned duration[]={17,14,12,10,8,5,3,1};
 assert(!backend_open("ssc-mame-fast",&flag)&&count==308&&bytes[0]==0x8f&&bytes[1]==55);++checks;
 for(i=0;i<4;i++)assert(bytes[3+4*i]==12&&bytes[4+4*i]*256+bytes[5+4*i]==sq[i]&&bytes[6+4*i]==0);
 assert(bytes[19]==0&&bytes[20]==0&&bytes[21]==1&&bytes[22]==0&&bytes[23]==255);++checks;
 assert(bytes[24]==0x99);for(i=0;i<8;i++)assert(bytes[25+4*i]==12&&bytes[26+4*i]*256+bytes[27+4*i]==whoop[i]&&bytes[28+4*i]==duration[i]);
 assert(bytes[57]==0&&bytes[58]==0&&bytes[59]==1&&bytes[60]==0&&bytes[61]==255);++checks;
 assert(bytes[62]==0x8a);for(i=0;i<60;i++)assert(bytes[63+4*i]==12&&bytes[64+4*i]*256+bytes[65+4*i]==phaser[i%6]&&bytes[66+4*i]==0);
 assert(bytes[303]==0&&bytes[304]==0&&bytes[305]==1&&bytes[306]==0&&bytes[307]==255);++checks;
 assert(!backend_close());count=0;assert(!backend_open("ssc-slow",&flag)&&bytes[1]==27&&bytes[5]==72&&bytes[9]==56&&bytes[13]==40&&bytes[17]==24);++checks;
 assert(!backend_close());printf("%d M2 recipe/timer checks passed\n",checks);return 0;}
