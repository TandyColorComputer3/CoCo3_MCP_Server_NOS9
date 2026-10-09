#include "platform.h"
#define AUDIO_IO 245
Byte ipc_open(const char *name,Byte *path);
Byte ipc_close(Byte path);
Byte ipc_dup(Byte path,Byte *copy);
Byte ipc_read(Byte path,Byte *data,Word count);
Byte ipc_fork(const char *name,const char *args,Byte *pid);
Byte ipc_wait(Byte *pid,Byte *status);
Byte ipc_signal(Byte pid,Byte signal);
Byte ipc_self_pid(Byte *pid);
Byte ipc_ready(Byte path,Byte *ready);
Byte ipc_priority(Byte pid,Byte priority);
