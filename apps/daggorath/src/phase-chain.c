#include "phase-chain.h"
static Byte dod_chain_release(const char *moduleName,Byte moduleType);
/* NitrOS-9 level1/modules/kernel/fchain.asm, Level II: F$Chain replaces
 * the process before linking/loading its target. On some errors the caller
 * is condemned, so no caller-owned module reference may survive this call. */
Byte dod_chain(const char *module,const char *parameters,Word length)
{
    Byte error,type,n=0;Word i;char path[32],copy[256];
    while(module[n]&&n<sizeof(path)){path[n]=module[n];n++;}
    if(!n||n==sizeof(path)||length>255||(!parameters&&length))return ERR_ARGUMENT;
    path[n]=0;
    /* Level II F$Chain first unlinks the old primary module and later copies
     * U/Y. Always give it bytes in process data, even when a caller passes
     * a C string literal from the old program module. */
    if(length)for(i=0;i<length;i++)copy[i]=parameters[i];
    else copy[0]=0;
    /* Preflight existence/type while the caller can still report errors.
     * F$NMLoad returns the module type in A; release its reference before
     * entering destructive F$Chain. The latter loads by NUL path itself. */
    asm {
        pshs y,u
        leax :path
        os9 $22
        bcs @load_failed
        sta :type
        clrb
@load_failed
        puls u,y
        stb :error
    }
    if(error)return error;
    {const char *name=module;
     for(i=0;i<n;i++)if(module[i]=='/')name=module+i+1;
     error=dod_chain_release(name,type);
     if(error)return error;}
    if(type!=0x11)return ERR_ARGUMENT;
    asm {
        pshs y,u
        lda #$11
        ldb #1
        leax :path
        ldy :length
        leau :copy
        os9 $05
        bcs @failed
        clrb
@failed
        puls u,y
        stb :error
    }
    return error;
}
static Byte dod_chain_release(const char *moduleName,Byte moduleType)
{
    Byte error,n=0;char name[32];
    while(moduleName[n]&&n<sizeof(name)){name[n]=moduleName[n];n++;}
    if(!n||n==sizeof(name))return ERR_ARGUMENT;
    name[n-1]|=128;
    asm {
        pshs y,u
        lda :moduleType
        leax :name
        os9 $1d
        bcs @release_failed
        clrb
@release_failed
        puls u,y
        stb :error
    }
    return error;
}
