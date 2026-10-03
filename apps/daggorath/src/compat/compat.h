#ifndef DAGGORATH_COMPAT_H
#define DAGGORATH_COMPAT_H
#include "../platform.h"

/* Original cartridge domain. Physical 512-wide conversion is presenter-only. */
#define DAG_COMPAT_WIDTH 256
#define DAG_COMPAT_HEIGHT 192
#define DAG_COMPAT_STRIDE 32
#define DAG_COMPAT_FRAME_BYTES 6144
#define DAG_COMPAT_STATE_BYTES 3333
#define DAG_COMPAT_STRIP_ROWS 32
#define DAG_COMPAT_STRIP_SOURCE_BYTES 1024
#define DAG_COMPAT_STRIP_TARGET_BYTES 2048

/* Original $0200..$0F04 state is based at compatibility offset zero. */
#define DAG_COMPAT_ORIGINAL_BASE 0x0200
#define DAG_COMPAT_CCB_BASE 0x01D4
#define DAG_COMPAT_CCB_END 0x03F4
#define DAG_COMPAT_NEIBOR 0x07F4
#define DAG_COMPAT_DROW 0x007C
#define DAG_COMPAT_PDIR 0x0023
#define DAG_COMPAT_RANGE 0x008B
#define DAG_COMPAT_HLFSTP 0x0073
#define DAG_COMPAT_BAKSTP 0x0074
#define DAG_COMPAT_VXSCAL 0x004F
#define DAG_COMPAT_VYSCAL 0x0050

typedef struct {
    Byte *state;
    Byte *logicalFrame;
    Byte mode;
} DagCompatRenderContext;

/* COMDAT.ASM TXTPRI / COMTXT.ASM text state, adapted to one caller-owned
 * logical framebuffer. Cursor is the original 0..127 character index. */
typedef struct {
    Byte *logicalFrame;
    Word cursor;
    Byte inverse;
    Byte operation;
    Byte value;
} DagCompatTextContext;

enum {
    DAG_COMPAT_TEXT_CLEAR_PRIMARY = 0,
    DAG_COMPAT_TEXT_CLEAR_STATUS = 1,
    DAG_COMPAT_TEXT_CLEAR_VIEWPORT = 2,
    DAG_COMPAT_TEXT_CLEAR_SCREEN = 3,
    DAG_COMPAT_TEXT_CHARACTER = 4,
    DAG_COMPAT_TEXT_ERASE_CHARACTER = 5
};

enum {
    DAG_COMPAT_GIANT_FORWARD = 0,
    DAG_COMPAT_GIANT_RIGHT_PEEK = 1
};

Byte dag_compat_render_giant(DagCompatRenderContext *context);
void dag_compat_expand_strip(const Byte *source, Byte *target);
Byte dag_compat_text_apply(DagCompatTextContext *context);

#endif
