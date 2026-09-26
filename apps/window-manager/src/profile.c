#include <cmoc.h>
#include "profile.h"

Byte profile_name(const char *name)
{
    Word n=0;
    if (!name || !*name) return ERR_ARGUMENT;
    while (*name) {
        char c=*name++;
        if (++n>PROFILE_NAME || !((c>='a' && c<='z') || (c>='A' && c<='Z')
            || (c>='0' && c<='9') || c=='_' || c=='-')) return ERR_ARGUMENT;
    }
    return 0;
}
Byte profile_color(const char *s, Byte *color)
{
    Word v=0, n=0;
    if (!s || !*s) return ERR_ARGUMENT;
    while (*s) {
        if (*s<'0' || *s>'9' || ++n>2) return ERR_ARGUMENT;
        v=v*10+(*s++-'0');
    }
    if (v>15) return ERR_ARGUMENT;
    *color=(Byte)v;
    return 0;
}
Byte profile_supported(const WindowInfo *info)
{
    return ((info->type==1 || info->type==2) && info->columns && info->rows
        && info->colors.foreground<=15 && info->colors.background<=15) ? 0 : ERR_ARGUMENT;
}
Byte profile_capture(const char *name, const WindowInfo *info, WindowProfile *p)
{
    if (profile_name(name) || profile_supported(info)) return ERR_ARGUMENT;
    memset(p,0,sizeof(*p));
    strcpy(p->name,name); p->foreground=info->colors.foreground;
    p->background=info->colors.background; return 0;
}
/* Strict, bounded five-line grammar. CR, LF and CR/LF accepted; no NUL,
 * duplicate/unknown fields, trailing bytes, missing end marker or newer version.
 * Parse into a temporary so an error never publishes a partial profile. */
Byte profile_parse(const char *text, Word size, WindowProfile *p)
{
    char lines[5][40];
    WindowProfile value;
    Word pos=0, i, n;
    memset(&value,0,sizeof(value));
    if (!size || size>PROFILE_LIMIT) return ERR_ARGUMENT;
    for (i=0;i<5;++i) {
        n=0;
        while (pos<size && text[pos]!=13 && text[pos]!=10) {
            Byte c=(Byte)text[pos++];
            if (c<32 || c>126 || n>=39) return ERR_ARGUMENT;
            lines[i][n++]=(char)c;
        }
        lines[i][n]=0;
        if (pos==size) return ERR_ARGUMENT;
        if (text[pos++]==13 && pos<size && text[pos]==10) ++pos;
    }
    if (pos!=size || strcmp(lines[0],"WindowProfile 1") || strcmp(lines[4],"end")
        || strncmp(lines[1],"name=",5) || strncmp(lines[2],"foreground=",11)
        || strncmp(lines[3],"background=",11) || profile_name(lines[1]+5)
        || profile_color(lines[2]+11,&value.foreground)
        || profile_color(lines[3]+11,&value.background)) return ERR_ARGUMENT;
    strcpy(value.name,lines[1]+5); *p=value; return 0;
}
Word profile_format(const WindowProfile *p, char *buffer)
{
    if (profile_name(p->name) || p->foreground>15 || p->background>15) return 0;
    /* Explicit CR is native OS-9 text; avoid CMOC newline conversion ambiguity. */
    sprintf(buffer,"WindowProfile 1\rname=%s\rforeground=%u\rbackground=%u\rend\r",
        p->name,(unsigned)p->foreground,(unsigned)p->background);
    return strlen(buffer);
}
