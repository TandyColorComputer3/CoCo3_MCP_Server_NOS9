#ifndef C6_COWIN_GRAPHICS_H
#define C6_COWIN_GRAPHICS_H
#include "platform.h"
Byte graphics_open(Byte *path);
Byte graphics_heart(Byte path,Byte phase);
Byte graphics_scene(Byte path,Byte destinationY,Byte status,Byte phase);
Byte graphics_close(Byte path);
extern Byte cowinDiag[64];
#endif
