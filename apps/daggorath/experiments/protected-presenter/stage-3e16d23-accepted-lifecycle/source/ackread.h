#ifndef PP_ACKREAD_H
#define PP_ACKREAD_H
#include "platform.h"
#define PP_TRACE_LOG_FAILURE 0xfe
Byte pp_persist_pre_ack(Byte pid, Byte path, Byte opcode, Byte sequence,
                        Byte frameLength);
Byte pp_persist_validation(Byte opcode, Byte sequence, Byte mismatchMask,
                           Byte status, const Byte *frame);
Byte pp_persist_shutdown_begin(const Byte *frame);
Byte pp_persist_exit(Byte lastOperation, Byte syscallError,
                     Byte appError, Byte shutdownAttempted,
                     Byte shutdownAckReceived, Byte requestedStatus);
Byte ipc_read_observed(Byte path, Byte *data, Word count, Word *transferred,
                       Byte opcode, Byte sequence, Byte pid,
                       Byte *attemptCount);
#endif
