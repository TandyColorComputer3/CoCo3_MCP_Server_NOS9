#ifndef WM_UI_H
#define WM_UI_H
#include "window.h"
Byte ui_info(const WindowInfo *info);
Byte ui_colors(const char *label, const WindowColors *colors);
Byte ui_line(const char *text);
Byte ui_result(const char *label, Byte code);
#endif
