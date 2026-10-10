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
#ifdef DAG_COMPAT_C6
#define DAG_COMPAT_PRESENT_ROWS 8
#define DAG_COMPAT_PRESENT_SOURCE_BYTES 256
#define DAG_COMPAT_PRESENT_TARGET_BYTES 512
#else
#define DAG_COMPAT_PRESENT_ROWS DAG_COMPAT_STRIP_ROWS
#define DAG_COMPAT_PRESENT_SOURCE_BYTES DAG_COMPAT_STRIP_SOURCE_BYTES
#define DAG_COMPAT_PRESENT_TARGET_BYTES DAG_COMPAT_STRIP_TARGET_BYTES
#endif

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
#ifdef DAG_COMPAT_C6
    Byte (*progress)(void *);
    void *progressContext;
    Word residentY;
    Byte progressError;
#endif
} DagCompatRenderContext;

/* Reentrant adapter for the original VECTOR DDA. Coordinates are already
 * transformed by VCTLST; all mutable VECTOR scratch remains caller-owned. */
typedef struct {
    Byte *state;
    Byte *logicalFrame;
    int x0,y0,x1,y1;
    Byte fade;
} DagCompatLineContext;

/* COMDAT.ASM TXTPRI / COMTXT.ASM text state, adapted to one caller-owned
 * logical framebuffer. Cursor is the original 0..127 character index. */
typedef struct {
    Byte *logicalFrame;
    Word cursor;
    Byte inverse;
    Byte operation;
    Byte value;
} DagCompatTextContext;

/* C3 uses the same source glyph table and seven-scanline deposit operation for
 * the status row and PREPARE's EXAMINE cell space.  `cell` is a full-screen
 * 32-column cell index (0..767), rather than the primary area's 0..127 cursor. */
typedef struct {
    Byte *logicalFrame;
    Word cell;
    Byte inverse;
    Byte value;
} DagCompatCellContext;

typedef struct {
    Byte *state;
    Byte *logicalFrame;
    Byte fade;
} DagCompatWizardContext;

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
Byte dag_compat_vector_line(DagCompatLineContext *context);
void dag_compat_expand_strip(const Byte *source, Byte *target);
void dag_compat_expand_slice(const Byte *source, Byte *target);
Byte dag_compat_text_apply(DagCompatTextContext *context);
Byte dag_compat_text_cell(DagCompatCellContext *context);
Byte dag_compat_render_wizard(DagCompatWizardContext *context);

#endif
