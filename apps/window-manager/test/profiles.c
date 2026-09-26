#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "m2.h"
#include "fileio.h"
static WindowColors colors;
static Byte *signalFlag;
static int sets,reads,partial,readback,restoreFail,writeFail,closeFail,openFail,cancelAt,sleeps,uiFail;
static Byte type;
static char output[16000];
static Word outputLength;
typedef struct {char path[81],data[180];Word length,pos;} File;
static File files[16];
static int fileCount;
Byte os_getstat(Byte p,Byte fn,Registers *r) {
    assert(p==1);
    if(fn==SS_SCTYP)r->a=type;
    else if(fn==SS_SCSIZ){r->x=80;r->y=25;}
    else if(fn==SS_FBRGS){++reads;r->a=colors.foreground;r->b=colors.background;r->x=colors.border;
        if(readback && reads==2)r->a^=1;}
    else assert(0);
    return 0;
}
Byte os_devname(Byte p,char *b){assert(p==1);strcpy(b,"Term");return 0;}
Byte os_intercept(Byte *p){signalFlag=p;return 0;}
Byte os_signal_value(Byte *p){return *p;}
Byte os_cancel_self(void){*signalFlag=3;return 0;}
Byte os_sleep(Word n){assert(n==1);++sleeps;if(cancelAt && sleeps==cancelAt)*signalFlag=2;return 0;}
Byte os_write(Byte p,const void *b,Word n) {
    const Byte *s=b;
    if(p>=3){File *f=&files[p-3];if(writeFail)return ERR_WRITE;memcpy(f->data,s,n);f->length=n;return 0;}
    assert(p==1);
    if(n==6 && s[0]==27){
        assert(s[1]==0x32 && s[3]==27 && s[4]==0x33);++sets;
        if(restoreFail && sets==2)return ERR_WRITE;
        colors.foreground=s[2];if(partial && sets==1)return ERR_WRITE;
        colors.background=s[5];return 0;
    }
    assert(!(n==9 && s[0]==27)); /* Profiles must NEVER write a border packet. */
    if(uiFail && n>=7 && !memcmp(s,"APPLIED",7))return ERR_WRITE;
    assert(outputLength+n<sizeof(output));memcpy(output+outputLength,s,n);outputLength+=n;output[outputLength]=0;return 0;
}
Byte file_open(const char *p,Byte create,Byte *fd){
    int i;if(openFail)return 214;
    for(i=0;i<fileCount;++i)if(!strcmp(files[i].path,p))break;
    if(create){if(i<fileCount)return 218;assert(fileCount<16);strcpy(files[i].path,p);files[i].length=0;++fileCount;}
    else if(i==fileCount)return 216;
    files[i].pos=0;*fd=i+3;return 0;
}
Byte file_read(Byte fd,void *b,Word n,Word *got){
    File *f=&files[fd-3];Word left=f->length-f->pos;
    if(!left){*got=0;return FILE_EOF;}
    if(n>7)n=7; /* Exercise short successful reads. */
    if(n>left)n=left;memcpy(b,f->data+f->pos,n);f->pos+=n;*got=n;return 0;
}
Byte file_close(Byte fd){(void)fd;return closeFail ? 245 : 0;}
static void reset(void){
    memset(files,0,sizeof(files));fileCount=0;colors.foreground=0;colors.background=1;colors.border=9;
    sets=reads=partial=readback=restoreFail=writeFail=closeFail=openFail=cancelAt=sleeps=uiFail=0;
    outputLength=0;output[0]=0;type=2;
}
static int run(int n,char *a,char *b,char *c,char *d,char *e){char *v[]={"wmview",a,b,c,d,e,0};return m2_main(n,v);}
static void seed(void){assert(run(6,"create","night","1","0","/d1/p")==0);}
static void baseline(void){assert(colors.foreground==0 && colors.background==1 && colors.border==9);}
int main(void){
    WindowProfile p,keep; char buf[161];Word n;
    reset();assert(run(4,"capture","entry","/d1/orig",0,0)==0);
    assert(profile_load("/d1/orig",&p)==0 && !strcmp(p.name,"entry") && p.foreground==0 && p.background==1);
    assert(run(3,"show","/d1/orig",0,0,0)==0);assert(sets==0);
    assert(run(4,"capture","entry","/d1/orig",0,0)==218);baseline();
    seed();assert(run(4,"apply","/d1/p","/d1/undo",0,0)==0);
    assert(colors.foreground==1 && colors.background==0 && colors.border==9);
    assert(profile_load("/d1/undo",&p)==0 && p.foreground==0 && p.background==1);
    assert(run(3,"revert","/d1/undo",0,0,0)==0);baseline();
    assert(run(4,"apply","/d1/p","/d1/undo2",0,0)==0);
    assert(run(3,"revert","/d1/undo2",0,0,0)==0);baseline();
    assert(run(4,"apply","/d1/p","/d1/undo",0,0)==218);baseline();
    assert(run(3,"show","/d1/missing",0,0,0)==216);
    assert(run(6,"create","bad","16","0","/d1/bad")==ERR_ARGUMENT);
    assert(run(6,"create","bad name","1","0","/d1/bad")==ERR_ARGUMENT);
    reset();seed();assert(run(3,"preview","/d1/p",0,0,0)==0);baseline();assert(sleeps==600);
    reset();seed();assert(run(4,"preview","/d1/p","cancel",0,0)==3);baseline();
    reset();seed();cancelAt=5;assert(run(3,"preview","/d1/p",0,0,0)==2);baseline();
    reset();seed();partial=1;assert(run(4,"apply","/d1/p","/d1/u",0,0)==245);baseline();
    reset();seed();readback=1;assert(run(4,"apply","/d1/p","/d1/u",0,0)==245);baseline();
    reset();seed();uiFail=1;assert(run(4,"apply","/d1/p","/d1/u",0,0)==245);baseline();
    reset();seed();partial=restoreFail=1;assert(run(4,"apply","/d1/p","/d1/u",0,0)==245);
    assert(strstr(output,"RESTORE FAILED") && !strstr(output,"REVERTED"));
    reset();seed();writeFail=1;assert(run(4,"apply","/d1/p","/d1/u",0,0)==245);baseline();assert(sets==0);
    reset();seed();closeFail=1;assert(run(4,"apply","/d1/p","/d1/u",0,0)==245);baseline();assert(sets==0);
    reset();seed();openFail=1;assert(run(3,"show","/d1/p",0,0,0)==214);
    reset();seed();type=5;assert(run(4,"apply","/d1/p","/d1/u",0,0)==187);assert(sets==0);
    reset();seed();assert(profile_load("/d1/p",&p)==0);n=profile_format(&p,buf);keep=p;
    assert(profile_parse(buf,n,&p)==0);
    {const char *bad[]={"WindowProfile 2\rname=a\rforeground=1\rbackground=0\rend\r",
        "WindowProfile 1\rname=a\rforeground=16\rbackground=0\rend\r",
        "WindowProfile 1\rname=a\rforeground=1\rbackground=0\r",
        "WindowProfile 1\rname=a\rforeground=1\rbackground=0\rend\rjunk",
        "WindowProfile 1\rname=a\rforeground=1\rfont=0\rend\r"};unsigned i;
        for(i=0;i<sizeof(bad)/sizeof(bad[0]);++i){assert(profile_parse(bad[i],strlen(bad[i]),&p)==187);assert(!memcmp(&p,&keep,sizeof(p)));}}
    {const char *lf="WindowProfile 1\nname=a\nforeground=1\nbackground=0\nend\n";
        assert(profile_parse(lf,strlen(lf),&p)==0);}
    reset();assert(run(2,"caps",0,0,0,0)==0);assert(strstr(output,"SCREEN-SCOPED"));
    assert(run(2,"fonts",0,0,0,0)==0);assert(strstr(output,"NOT resident") && strstr(output,"catalog-only"));assert(sets==0);
    puts("M2 profiles: persistence, grammar, apply/revert, repetition, failure/cancel and catalog cases passed");return 0;
}
