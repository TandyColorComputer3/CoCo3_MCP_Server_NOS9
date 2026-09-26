#include <cmoc.h>
#include "ipc.h"
/* One outstanding request keeps both pipes well below their verified 256-byte
 * capacity. submit queues; receive is an explicit synchronization point.
 * Client owns a direct child; caller must not concurrently reap that child. */
Byte audio_start(AudioClient *c,const char *service,const char *profile){
 Byte saved0=255,saved1=255,n,e=0,r;char args[40];Byte hello[8];
 memset(c,0,sizeof(*c));c->command=c->reply=255;
 if(strlen(profile)>30)return AUDIO_BAD;
 strcpy(args,profile);strcat(args,"\r");
 e=ipc_open("/pipe",&c->command);if(e)goto fail;
 e=ipc_open("/pipe",&c->reply);if(e)goto fail;
 e=ipc_dup(0,&saved0);if(e)goto fail;e=ipc_dup(1,&saved1);if(e)goto fail;
 e=ipc_close(0);if(e)goto fail;e=ipc_dup(c->command,&n);if(e||n!=0){if(!e)e=AUDIO_IO;goto restore;}
 e=ipc_close(1);if(e)goto restore;e=ipc_dup(c->reply,&n);if(e||n!=1){if(!e)e=AUDIO_IO;goto restore;}
 e=ipc_fork(service,args,&c->pid);
restore:
 ipc_close(0);r=ipc_dup(saved0,&n);if(!e)e=r;
 ipc_close(1);r=ipc_dup(saved1,&n);if(!e)e=r;
 if(saved0!=255){ipc_close(saved0);saved0=255;}
 if(saved1!=255){ipc_close(saved1);saved1=255;}
 if(e)goto fail;
 c->opened=1;
 e=ipc_read(c->reply,hello,8);if(e)goto fail;
 if(hello[0]!=AUDIO_MAGIC||hello[1]!=AUDIO_VERSION||hello[2]!=0||hello[5]!=0)e=AUDIO_BAD;
 else e=hello[4];
 if(!e)return 0;
fail:
 if(saved0!=255)ipc_close(saved0);if(saved1!=255)ipc_close(saved1);
 if(c->pid){Byte pid,status;ipc_signal(c->pid,3);ipc_wait(&pid,&status);}
 if(c->command!=255)ipc_close(c->command);if(c->reply!=255)ipc_close(c->reply);
 c->opened=0;return e;
}
Byte audio_submit(AudioClient *c,Byte op,Byte sound,Byte gain){Byte f[8],e;
 if(!c->opened)return AUDIO_BAD;if(c->pending)return AUDIO_BUSY;
 audio_frame(f,op,sound,gain,++c->sequence);e=audio_validate(f);if(e)return e;
 e=os_write(c->command,f,8);if(!e)c->pending=op;return e;
}
Byte audio_receive(AudioClient *c){Byte f[8],e;if(!c->pending)return AUDIO_BAD;
 e=ipc_read(c->reply,f,8);if(e)return e;
 if(f[0]!=AUDIO_MAGIC||f[1]!=AUDIO_VERSION||f[2]!=c->pending||f[5]!=c->sequence||f[6]||f[7])return AUDIO_BAD;
 c->pending=0;return f[4];
}
Byte audio_finish(AudioClient *c){Byte e=0,r,pid,status;
 if(!c->opened)return AUDIO_BAD;
 if(c->pending)e=audio_receive(c);
 if(!e)e=audio_submit(c,AUDIO_SHUTDOWN,0,0);
 if(!e)e=audio_receive(c);
 if(e)ipc_signal(c->pid,3);
 r=ipc_wait(&pid,&status);if(!e)e=r;if(!e&&pid!=c->pid)e=AUDIO_BAD;if(!e)e=status;
 ipc_close(c->command);ipc_close(c->reply);c->opened=0;return e;
}
Byte audio_cancel(AudioClient *c){Byte e,pid,status,r;
 if(!c->opened)return AUDIO_BAD;e=ipc_signal(c->pid,3);
 r=ipc_wait(&pid,&status);if(!e)e=r;if(!e&&pid!=c->pid)e=AUDIO_BAD;if(!e)e=status;
 ipc_close(c->command);ipc_close(c->reply);c->opened=0;return e;
}
