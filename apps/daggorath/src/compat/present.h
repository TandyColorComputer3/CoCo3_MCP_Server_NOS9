#ifndef DAGGORATH_COMPAT_PRESENT_H
#define DAGGORATH_COMPAT_PRESENT_H
#include "compat.h"

typedef struct {
    Word conversionTicks;
    Word transferTicks;
    Word mappedLength;
} DagCompatPresentTiming;

Byte dag_compat_present_open(void);
Byte dag_compat_present(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES],
                        DagCompatPresentTiming *timing);
Byte dag_compat_present_close(void);

#endif
