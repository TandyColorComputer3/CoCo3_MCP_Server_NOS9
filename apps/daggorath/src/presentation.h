#include "platform.h"
Byte screen_open(void);
Byte screen_prepare(const Byte *frame);
Byte screen_flip(void);
Byte screen_present(const Byte *frame);
Byte screen_close(void);
