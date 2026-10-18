#ifndef PPGRAPHICS_H
#define PPGRAPHICS_H
#include "platform.h"
Byte pp_graphics_open(Byte *path, Byte *actualType, Word *actualWidth, Word *actualHeight);
Byte pp_graphics_draw(Byte path, Word x, Word y);
Byte pp_graphics_close(Byte path);
#endif
