#include <cmoc.h>
#include "m2.h"
#include "ui.h"
typedef struct { Byte buffer,width,height; const char *name,*file; } FontEntry;
#include "font-data.h"
Byte font_catalog(void)
{
    Word i;
    Byte e;
    char line[96];
    e=ui_line("EOU /SYS startup files + fontlist.txt, frozen 2026-09-26; group 200.");
    if(e)return e;
    e=ui_line("50 file buffers; catalog has 49. NOT resident enumeration/current font.");
    if(e)return e;
    e=ui_line("ID(decimal) cells name file; 0x0 = dimensions UNVERIFIED in startup files");
    if(e)return e;
    for(i=0;i<sizeof(fontEntries)/sizeof(fontEntries[0]);++i) {
        const FontEntry *f=&fontEntries[i];
        sprintf(line,"%u %ux%u %s %s",(unsigned)f->buffer,(unsigned)f->width,
            (unsigned)f->height,f->name,f->file);
        e=ui_line(line);if(e)return e;
    }
    return ui_line("File evidence only. Shared graphics fonts; no selection or loading performed.");
}
