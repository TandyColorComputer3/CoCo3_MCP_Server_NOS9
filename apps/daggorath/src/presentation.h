#include "platform.h"
Byte screen_open(void);
Byte screen_prepare(const Byte *frame);
/* Refresh only status (rows 152..159) and command input (184..190) in the
 * already-owned mapped GP buffer; preserve the dungeon image. */
Byte screen_prepare_ui(const Byte *frame);
Byte screen_flip(void);
Byte screen_present(const Byte *frame);
Byte screen_present_ui(const Byte *frame);
/* During a partial scene render, change only the two original heart glyphs
 * in the last complete mapped image; all dungeon and other UI pixels stay. */
Byte screen_present_heart(const Byte *frame);
Byte screen_close(void);
Byte screen_path(void);
