#ifndef WM_WINDOW_H
#define WM_WINDOW_H
#include "platform.h"
/* Process-local snapshot, not a serialized profile or a kernel structure. */
typedef struct { Byte foreground, background, border; } WindowColors;
typedef struct {
    Byte type;
    Word columns, rows;
    WindowColors colors;
    char device[33];
    Byte identityKnown;
} WindowInfo;
Byte window_query(Byte path, WindowInfo *info);
Byte window_colors(Byte path, WindowColors *colors);
Byte window_set_colors(Byte path, const WindowColors *colors);
Byte window_same_colors(const WindowColors *a, const WindowColors *b);
Byte window_demo_colors(const WindowInfo *info, WindowColors *test);
/* Every attempted mutation must pass through this restoration path. */
Byte window_restore(Byte path, const WindowColors *original);
#endif
