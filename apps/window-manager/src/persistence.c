#include <cmoc.h>
#include "profile.h"
#include "fileio.h"

Byte profile_path(const char *s)
{
    Word n=0, slashes=0, part=0;
    if (!s || *s!='/') return ERR_ARGUMENT;
    while (*s) {
        char c=*s++;
        if (++n>80) return ERR_ARGUMENT;
        if (c=='/') { if (n>1 && !part) return ERR_ARGUMENT; ++slashes; part=0; }
        else {
            if (!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')
                ||c=='_'||c=='-'||c=='.')) return ERR_ARGUMENT;
            ++part;
        }
    }
    return slashes>=2 && part ? 0 : ERR_ARGUMENT;
}
Byte profile_load(const char *path, WindowProfile *p)
{
    char buffer[PROFILE_LIMIT+1];
    Byte fd, error, closeError;
    Word count=0, got;
    if (profile_path(path)) return ERR_ARGUMENT;
    error=file_open(path,0,&fd);
    if (error) return error;
    do {
        got=0;
        error=file_read(fd,buffer+count,PROFILE_LIMIT+1-count,&got);
        if (error==FILE_EOF) { error=0; break; }
        if (error) break;
        if (!got || got>PROFILE_LIMIT+1-count) { error=ERR_ARGUMENT; break; }
        count+=got;
        if (count>PROFILE_LIMIT) { error=ERR_ARGUMENT; break; }
    } while (1);
    closeError=file_close(fd);
    if (error) return error;
    if (closeError) return closeError;
    return profile_parse(buffer,count,p);
}
Byte profile_save(const char *path, const WindowProfile *p)
{
    char buffer[PROFILE_LIMIT+1];
    Byte fd, error, closeError;
    Word size=profile_format(p,buffer);
    if (!size || profile_path(path)) return ERR_ARGUMENT;
    /* I$Create, never delete/truncate/retry an existing file. Upstream copy.asm
     * L0450 explicitly requires delete for replacement; we do not do that. */
    error=file_open(path,1,&fd);
    if (error) return error;
    error=os_write(fd,buffer,size);
    closeError=file_close(fd);
    /* A failed write/close can leave a partial new file. Preserve it, report
     * error and refuse reuse; never delete something on a guessed path. */
    return error ? error : closeError;
}
