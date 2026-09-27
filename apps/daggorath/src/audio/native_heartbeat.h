#ifndef DOD_NATIVE_HEARTBEAT_H
#define DOD_NATIVE_HEARTBEAT_H
#include "platform.h"
/* A directly owned OS-9 device path, not an SSC channel or process-local IRQ.
 * Preload helpers before claim. EOU F$Fork inherits only paths 0..2; do not
 * duplicate this device onto stdio. I$Dup shares path references, and final
 * reference close controls implicit teardown. Explicit close releases first.
 * No active HALT floppy I/O is within the current timing guarantee.
 */
typedef struct { Byte path, opened; } NativeHeartbeat;
typedef struct { Byte active,enabled,rateByte,remainingByte,fault; } NativeHeartbeatState;
Byte native_heartbeat_open(NativeHeartbeat *h);
Byte native_heartbeat_rate(NativeHeartbeat *h,Word rateByte);
Byte native_heartbeat_enable(NativeHeartbeat *h);
Byte native_heartbeat_disable(NativeHeartbeat *h);
Byte native_heartbeat_query(NativeHeartbeat *h,NativeHeartbeatState *state);
Byte native_heartbeat_close(NativeHeartbeat *h);
#endif
