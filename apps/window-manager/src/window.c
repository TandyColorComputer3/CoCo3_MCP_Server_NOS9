#include "window.h"

Byte window_colors(Byte path, WindowColors *colors)
{
    Registers r;
    Byte error;
    r.x=0;
    error = os_getstat(path, SS_FBRGS, &r);
    if (error) return error;
    colors->foreground = r.a;
    colors->background = r.b;
    colors->border = (Byte) r.x;
    return 0;
}

Byte window_query(Byte path, WindowInfo *info)
{
    Registers r;
    Byte error, i;
    info->identityKnown = 0;
    r.x=0;
    error = os_getstat(path, SS_SCTYP, &r);
    if (error) return error;
    info->type = r.a;
    error = os_getstat(path, SS_SCSIZ, &r);
    if (error) return error;
    info->columns = r.x;
    info->rows = r.y;
    error = window_colors(path, &info->colors);
    if (error) return error;
    /* IOMan copies at most 32 bytes, OS-9 high-bit terminated name. */
    for (i=0; i<33; ++i) info->device[i] = 0;
    if (!os_devname(path, info->device)) {
        for (i=0; i<32; ++i) {
            Byte c = (Byte) info->device[i];
            if (!c) break;
            info->device[i] = (c & 127) >= 32 && (c & 127) < 127 ? c & 127 : '?';
            if (c & 128) { ++i; break; }
        }
        info->device[i] = 0;
        info->identityKnown = i != 0;
    }
    return 0;
}

Byte window_set_colors(Byte path, const WindowColors *colors)
{
    /* EOU gfx2_ver1.asm L05A5/L05B6/L05BA/L05CA, raw I$Write.
     * These change drawing attributes. They do not recolor old text. */
    Byte packet[9];
    packet[0]=27; packet[1]=0x32; packet[2]=colors->foreground;
    packet[3]=27; packet[4]=0x33; packet[5]=colors->background;
    packet[6]=27; packet[7]=0x34; packet[8]=colors->border;
    return os_write(path, packet, sizeof(packet));
}

Byte window_same_colors(const WindowColors *a, const WindowColors *b)
{
    return a->foreground == b->foreground && a->background == b->background
        && a->border == b->border;
}

Byte window_demo_colors(const WindowInfo *info, WindowColors *test)
{
    /* Mutation is deliberately restricted to hardware text in M1.
     * CoWin L0AD5 identifies types 1/2; L0B01 masks text palette indices. */
    if ((info->type != 1 && info->type != 2) || !info->columns || !info->rows
        || info->colors.foreground > 15 || info->colors.background > 15)
        return ERR_ARGUMENT;
    test->foreground = info->colors.background;
    test->background = info->colors.foreground;
    if (test->foreground == test->background) test->foreground ^= 1;
    test->border = info->colors.border == 0 ? 1 : 0;
    return 0;
}

Byte window_restore(Byte path, const WindowColors *original)
{
    Byte error;
    WindowColors readback;
    error = window_set_colors(path, original);
    if (error) return error;
    error = window_colors(path, &readback);
    if (error) return error;
    return window_same_colors(original, &readback) ? 0 : ERR_WRITE;
}

/* Profile operations intentionally never send the screen-scoped border code. */
Byte window_set_pair(Byte path, Byte foreground, Byte background)
{
    Byte packet[6];
    packet[0]=27; packet[1]=0x32; packet[2]=foreground;
    packet[3]=27; packet[4]=0x33; packet[5]=background;
    return os_write(path,packet,6);
}
Byte window_restore_pair(Byte path, const WindowColors *original)
{
    WindowColors actual;
    Byte error=window_set_pair(path,original->foreground,original->background);
    if (error) return error;
    error=window_colors(path,&actual);
    if (error) return error;
    return actual.foreground==original->foreground && actual.background==original->background
        ? 0 : ERR_WRITE;
}
