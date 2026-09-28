/* Disposable GShell launch-contract probe; not Daggorath production code.
 * F$ID/F$GPrDsc/SS.DevNm/SS.ScTyp/SS.ScSiz: official upstream defs/os9.d
 * f470fa52; EOU runtime result is the evidence for this target. */
#include <cmoc.h>
typedef unsigned char Byte;
typedef unsigned short Word;
typedef struct { Byte a,b; Word x,y; } Registers;
static Byte id,err,desc[512];
static Byte os_getstat(Byte path, Byte function, Registers *r){
 Byte error;
 asm {
  pshs y,u
  lda :path
  ldb :function
  ldx :r
  ldx 2,x
  os9 $8d
  bcs @failed
  ldu :r
  sta ,u
  stb 1,u
  stx 2,u
  sty 4,u
  clrb
@failed puls u,y
  stb :error
 }
 return error;
}
static Byte getid(void){
 Byte pid;
 asm {
  pshs y,u
  os9 $0c
  sta :pid
  puls u,y
 }
 return pid;
}
static Byte getdesc(Byte pid, Byte *buffer){
 Byte error;
 asm {
  pshs y,u
  lda :pid
  ldx :buffer
  os9 $18
  bcs @bad
  clrb
@bad puls u,y
  stb :error
 }
 return error;
}
static void devname(Byte path,char *buf){
 Registers r;Byte e,i;
 memset(buf,0,33);r.x=(Word)buf;
 e=os_getstat(path,0x0e,&r);
 if(e){printf("PATH %u DEV ERR %u\r",path,e);return;}
 for(i=0;i<32;i++){
  if((Byte)buf[i]&128){buf[i]&=127;buf[i+1]=0;break;}
 }
 printf("PATH %u DEV %s\r",path,buf);
}
static void pathinfo(Byte path){
 Registers r;Byte e;char name[33];
 devname(path,name);
 memset(&r,0,sizeof(r));e=os_getstat(path,0x93,&r);
 if(e)printf("PATH %u TYPE ERR %u\r",path,e);
 else printf("PATH %u TYPE %u\r",path,r.a);
 memset(&r,0,sizeof(r));e=os_getstat(path,0x26,&r);
 if(e)printf("PATH %u SIZE ERR %u\r",path,e);
 else printf("PATH %u SIZE %u %u\r",path,r.x,r.y);
}
static Byte create_log(Byte *fd){
 const char *path="/dd/gshell-log";Byte error,number;
 asm {
  ldx :path
  lda #2
  ldb #3
  os9 $83
  bcs @failed
  sta :number
  clrb
@failed
  stb :error
 }
 if(!error)*fd=number;
 return error;
}
static Byte write_log(Byte fd,const void *buffer,Word length){
 Byte error;
 asm {
  lda :fd
  ldx :buffer
  pshs y
  ldy :length
  os9 $8a
  bcs @failed
  clrb
@failed
  puls y
  stb :error
 }
 return error;
}
static Byte close_log(Byte fd){
 Byte error;
 asm {
  lda :fd
  os9 $8f
  bcs @failed
  clrb
@failed
  stb :error
 }
 return error;
}
static Byte report[160];
static void build_report(void){
 Byte i,j,e;Registers r;char nm[33];
 memset(report,0,sizeof(report));
 report[0]='G';report[1]='S';report[2]='H';report[3]='1';
 report[4]=id;report[5]=err?255:desc[1];report[6]=err?255:desc[172];report[7]=err;
 for(i=0;i<3;i++){
  Byte off=16+i*40;
  memset(nm,0,sizeof(nm));r.x=(Word)nm;
  e=os_getstat(i,0x0e,&r);report[off]=e;
  if(!e){for(j=0;j<32;j++){
    report[off+8+j]=(Byte)nm[j]&127;
    if((Byte)nm[j]&128)break;
  }}
  memset(&r,0,sizeof(r));e=os_getstat(i,0x93,&r);
  report[off+1]=e;report[off+2]=r.a;
  memset(&r,0,sizeof(r));e=os_getstat(i,0x26,&r);
  report[off+3]=e;report[off+4]=(Byte)(r.x>>8);report[off+5]=(Byte)(r.x&255);
  report[off+6]=(Byte)(r.y>>8);report[off+7]=(Byte)(r.y&255);
 }
}
int main(void){
 Byte *buffer=desc,fd,e;Word ticks=301;
 id=getid();err=getdesc(id,buffer);
 printf("MCP GSHELL PROBE\rPID %u DESC %u\r",id,err);
 if(!err)printf("DESC PID %u PARENT %u SEL %u\r",desc[0],desc[1],desc[172]);
 pathinfo(0);pathinfo(1);pathinfo(2);
 build_report();e=create_log(&fd);
 if(!e){e=write_log(fd,report,sizeof(report));if(!e)e=close_log(fd);else close_log(fd);}
 printf("LOG STATUS %u\r",e);
 printf("WAITING 300 TICKS\r");
 asm {
  ldx :ticks
  os9 $0a
 }
 return 0;
}
