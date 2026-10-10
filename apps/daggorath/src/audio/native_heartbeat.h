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
typedef struct { Byte active,enabled,rateByte,remainingByte,fault,phase; unsigned long edgeGeneration; } NativeHeartbeatState;
enum {
 NATIVE_HEARTBEAT_ARMED=0,
 NATIVE_HEARTBEAT_GENERATION_PENDING=1,
 NATIVE_HEARTBEAT_REQUEST_PENDING=2
};
Byte native_heartbeat_open(NativeHeartbeat *h);
Byte native_heartbeat_rate(NativeHeartbeat *h,Word rateByte);
Byte native_heartbeat_enable(NativeHeartbeat *h);
/* Atomically stops new native generations. Existing generations remain the
 * caller's responsibility until visibly reconciled. */
Byte native_heartbeat_disable(NativeHeartbeat *h);
Byte native_heartbeat_query(NativeHeartbeat *h,NativeHeartbeatState *state);
/* Atomic read-only snapshot of VIRQ-owned edge generation and output phase. */
Byte native_heartbeat_snapshot(NativeHeartbeat *h,NativeHeartbeatState *state);
Byte native_heartbeat_take_ticks(NativeHeartbeat *h,Word *videoTicks);
#ifdef DAG_HEARTBEAT_CONTROL_PLANE
/* Advisory wake control. Generation and request state remain authoritative. */
Byte native_heartbeat_arm(NativeHeartbeat *h,unsigned long expectedGeneration,
                          Byte *armResult);
Byte native_heartbeat_disarm(NativeHeartbeat *h);
Byte native_heartbeat_observe(NativeHeartbeat *h,unsigned long generation);
/* KICK is intentionally usable through a non-owner path. */
Byte native_heartbeat_kick(Byte path);
Byte native_heartbeat_claim_request(NativeHeartbeat *h,Byte *pending);
#endif
Byte native_heartbeat_close(NativeHeartbeat *h);
#endif
