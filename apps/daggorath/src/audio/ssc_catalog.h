#ifndef DOD_SSC_CATALOG_H
#define DOD_SSC_CATALOG_H
#include "audio.h"
#define SSC_CATALOG_CAPACITY 64
/* Builds one source-ID-specific AY interpretation for SSC buffer 6.
 * Existing M1/M2 IDs 0, 13 and 14 remain in their preloaded buffers. */
Byte ssc_catalog_build(Byte sound,Byte fast,Byte *data,Byte *length);
const char *ssc_catalog_name(Byte sound);
#endif
