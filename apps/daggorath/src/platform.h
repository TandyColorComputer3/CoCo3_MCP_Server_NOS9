/* Reused from apps/window-manager/src/platform.h; same verified CMOC/OS-9 ABI. */
#ifndef WM_PLATFORM_H
#define WM_PLATFORM_H
/* Constants verified against nitros9-reference defs/os9.d at f470fa52.
 * SS.DevNm is handled by IOMan; CoWin handles the three window GetStats. */
typedef unsigned char Byte;
typedef unsigned short Word;
enum { SS_DEVNM=0x0e, SS_SCSIZ=0x26, SS_SCTYP=0x93, SS_FBRGS=0x96,
       ERR_ARGUMENT=187, ERR_WRITE=245, SIGNAL_ABORT=2, SIGNAL_INTERRUPT=3 };
typedef struct { Byte a, b; Word x, y; } Registers;
Byte os_map_buffer(Byte path, Word groupBuffer, Byte map, Byte **buffer, Word *length);
Byte os_setstat(Byte path, Byte function, Registers *r);
Byte os_clock(Word *ticks, Byte validate);
Byte os_getstat(Byte path, Byte function, Registers *r);
Byte os_devname(Byte path, char *buffer);
Byte os_write(Byte path, const void *buffer, Word size);
Byte os_intercept(Byte *signal);
Byte os_intercept_audio(Byte *cancelAndNotice);
Byte os_signal_value(Byte *signal);
Byte os_sleep(Word ticks);
Byte os_cancel_self(void);
#endif
