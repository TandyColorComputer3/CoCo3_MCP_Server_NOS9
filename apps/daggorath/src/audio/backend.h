#include "audio.h"
Byte backend_open(const char *profile,Byte *signal);
Byte backend_play(Byte sound,Byte gain);
Byte backend_stop(void);
Byte backend_drain(void);
Byte backend_close(void);
