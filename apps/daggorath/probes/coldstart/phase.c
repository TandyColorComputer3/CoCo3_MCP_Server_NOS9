/* Disposable phase signal: ordinary process-owned RAM only, observed by Lua.
 * Four NOPs identify the store. No OS/hardware memory is modified. */
#include "platform.h"
static Byte mark[512];
void phase(Byte value){Word address=(((Word)mark+255)&0xff00)+0x24;
 asm {
 ldx :address
 lda :value
 sta ,x
 nop
 nop
 nop
 nop
 }
}
