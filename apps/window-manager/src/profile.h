#ifndef WM_PROFILE_H
#define WM_PROFILE_H
#include "window.h"
/* Version 1 is hardware-text foreground/background only. No border/font. */
#define PROFILE_LIMIT 160
#define PROFILE_NAME 24
typedef struct { char name[PROFILE_NAME+1]; Byte foreground, background; } WindowProfile;
Byte profile_name(const char *name);
Byte profile_color(const char *value, Byte *color);
Byte profile_parse(const char *text, Word size, WindowProfile *profile);
Word profile_format(const WindowProfile *profile, char *buffer);
Byte profile_capture(const char *name, const WindowInfo *info, WindowProfile *profile);
Byte profile_supported(const WindowInfo *info);
Byte profile_load(const char *path, WindowProfile *profile);
Byte profile_save(const char *path, const WindowProfile *profile);
Byte profile_path(const char *path);
Byte window_set_pair(Byte path, Byte foreground, Byte background);
Byte window_restore_pair(Byte path, const WindowColors *original);
#endif
