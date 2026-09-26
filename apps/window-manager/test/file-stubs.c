/* M1 lifecycle tests do not invoke persistence; satisfy the new command dispatcher. */
#include "fileio.h"
Byte file_open(const char *p, Byte c, Byte *f) {(void)p;(void)c;(void)f;return 208;}
Byte file_read(Byte f, void *b, Word s, Word *g) {(void)f;(void)b;(void)s;*g=0;return 208;}
Byte file_close(Byte f) {(void)f;return 208;}
