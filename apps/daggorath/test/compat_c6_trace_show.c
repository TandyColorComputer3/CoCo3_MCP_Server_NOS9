#include <cmoc.h>
#include "audio/ipc.h"
Byte os_write(Byte,const void *,Word);
static Byte *text(Byte *p,const char *s){while(*s)*p++=*s++;return p;}
static Byte *decimal(Byte *p,Word value)
{Word divisor=10000;Byte started=0,digit;while(divisor){digit=(Byte)(value/divisor);
 if(digit||started||divisor==1){*p++=(Byte)('0'+digit);started=1;}value=(Word)(value%divisor);divisor/=10;}return p;}
int main(void)
{Byte path,e,header[16],row[10],line[160],*p;Word count,i,sum=0,zero=0,one=0,two=0;
 Word mismatch=0,failed=0,sequence=0,scenes=0,previous=0,generation;
 e=ipc_open("/d1/hbtrace",&path);if(e)return e;e=ipc_read(path,header,16);if(e)goto done;
 if(header[0]!='D'||header[1]!='H'||header[2]!='B'||header[3]!='1'){e=187;goto done;}
 count=((Word)header[4]<<8)|header[5];
 for(i=0;i<count;i++){e=ipc_read(path,row,10);if(e)goto done;generation=((Word)row[0]<<8)|row[1];
  if(i&&generation!=(Word)(previous+1))sequence++;previous=generation;
  if(row[4]!=row[5])mismatch++;if(!(row[9]&1))failed++;if(row[9]>>1)scenes++;
  sum+=row[8];if(!row[8])zero++;else if(row[8]==1)one++;else if(row[8]==2)two++;}
 p=text(line,"C6TRACE n=");p=decimal(p,count);p=text(p," maxWake=");p=decimal(p,header[6]);
 p=text(p," maxHeart=");p=decimal(p,header[7]);p=text(p," maxScene=");p=decimal(p,header[8]);
 p=text(p," maxLatency=");p=decimal(p,header[9]);p=text(p," miss=");p=decimal(p,header[10]);
 p=text(p," collapse=");p=decimal(p,header[11]);p=text(p," dup=");p=decimal(p,header[12]);*p++=13;
 e=os_write(1,line,(Word)(p-line));if(e)goto done;
 p=text(line,"C6DIST sumTicks=");p=decimal(p,sum);p=text(p," zero=");p=decimal(p,zero);
 p=text(p," one=");p=decimal(p,one);p=text(p," two=");p=decimal(p,two);
 p=text(p," sceneEdges=");p=decimal(p,scenes);p=text(p," mismatch=");p=decimal(p,mismatch);
 p=text(p," fail=");p=decimal(p,failed);p=text(p," seq=");p=decimal(p,sequence);*p++=13;
 e=os_write(1,line,(Word)(p-line));
done:{Byte x=ipc_close(path);if(!e)e=x;}return e;
}
