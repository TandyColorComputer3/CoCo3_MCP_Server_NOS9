#include "audio.h"
Byte backend_open(const char *profile,Byte *signal);
Byte backend_play(Byte sound,Byte gain);
Byte backend_stop(void);
Byte backend_drain(void);
Byte backend_close(void);

/* Service-internal observability; never backend channels/registers in the IPC. */
Byte backend_phase(void);
Byte backend_decision(void);
