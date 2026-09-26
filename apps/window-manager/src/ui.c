#include <cmoc.h>
#include "ui.h"

Byte ui_line(const char *text)
{
    Byte error = os_write(1, text, strlen(text));
    /* CMOC sprintf converts LF to CR (stdlib/putchar_a.asm). Keep line
     * terminators out of formatted strings; write raw CR/LF explicitly. */
    static const Byte newline[2]={13,10};
    return error ? error : os_write(1, newline, 2);
}

Byte ui_colors(const char *label, const WindowColors *c)
{
    char line[128];
    /* Only fixed internal labels and three bytes are formatted here. */
    sprintf(line, "%s: foreground=%u background=%u border=%u",
        label, (unsigned)c->foreground, (unsigned)c->background, (unsigned)c->border);
    return ui_line(line);
}

Byte ui_info(const WindowInfo *info)
{
    char line[144];
    Byte error;
    const char *device=info->device;
    const char *kind = info->type == 1 || info->type == 2 ? "hardware text" :
        info->type >= 5 && info->type <= 8 ? "graphics" : "unclassified";
    if (!info->identityKnown) device="unknown";
    sprintf(line, "Window M1 - stdout path 1, device %s", device);
    error = ui_line(line);
    if (error) return error;
    sprintf(line, "type=%u (%s), area=%u columns x %u rows",
        (unsigned)info->type, kind, (unsigned)info->columns, (unsigned)info->rows);
    error = ui_line(line);
    if (error) return error;
    return ui_colors("Original", &info->colors);
}

Byte ui_result(const char *label, Byte code)
{
    char line[80];
    sprintf(line, "%s: %u", label, (unsigned)code);
    return ui_line(line);
}
