#ifndef WM_FILEIO_H
#define WM_FILEIO_H
#include "platform.h"
/* defs/os9.d and level1/cmds/copy.asm: EOF=$D3, Create existing=$DA. */
#define FILE_EOF 211
Byte file_open(const char *path, Byte create, Byte *fd);
Byte file_read(Byte fd, void *buffer, Word size, Word *got);
Byte file_close(Byte fd);
#endif
