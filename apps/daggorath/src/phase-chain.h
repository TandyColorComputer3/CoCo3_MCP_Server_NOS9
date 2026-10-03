#ifndef DOD_PHASE_CHAIN_H
#define DOD_PHASE_CHAIN_H
#include "platform.h"
/* F$Chain replaces this process. Parameters are OS-9 command-line bytes,
 * including the terminal CR. Destructive failures may not return. */
Byte dod_chain(const char *module,const char *parameters,Word length);
#endif
