#include "platform.h"
void screen_set_heart_patterns(const Byte heartPatterns[28]);
Byte screen_open(void);
Byte screen_prepare(const Byte *frame);
/* Queue the full frame for six 512x32 strips. screen_flip expands each strip
 * in bounded eight-row units and services the callback between units/writes. */
Byte screen_prepare_progress(const Byte *frame,Byte (*progress)(void *),void *context);
/* Queue only the two 512x32 strips containing status (rows 152..159) and
 * command input (184..190); preserved pixels remain exact from frame. */
Byte screen_prepare_ui(const Byte *frame);
Byte screen_prepare_ui_progress(const Byte *frame,Byte (*progress)(void *),void *context);
Byte screen_flip(void);
Byte screen_present(const Byte *frame);
Byte screen_present_ui(const Byte *frame);
Byte screen_present_ui_progress(const Byte *frame,Byte (*progress)(void *),void *context);
/* During a partial scene render, change only the two original heart glyphs
 * through the owned 32x7 GP buffer; all dungeon and other UI pixels stay. */
Byte screen_present_heart(const Byte *frame);
Byte screen_close(void);
Byte screen_path(void);
