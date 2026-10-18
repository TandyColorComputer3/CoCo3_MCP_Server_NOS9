#include <cmoc.h>
#include "ipc.h"
#include "fileio.h"
#include "ackread.h"
#include "pp25c_protocol.h"

#define MAGIC 0xb6
#define VERSION 1
#define FRAME 16
#define OP_PRESENT 5
#define OP_SHUTDOWN 7
#define ERR_PROTOCOL 187
#define ERR_CONTROL 188
extern Byte sleep_signal(void);
static const char logName[]="/d1/ppclient25c.log";

static Byte log_frame(Byte fd,const char *tag,const Byte *frame){Byte i;char line[64];Word n=0;
 while(tag[n]&&n<12){line[n]=tag[n];n++;}line[n++]='=';
 for(i=0;i<FRAME;i++){static const char hex[]="0123456789ABCDEF";line[n++]=hex[frame[i]>>4];line[n++]=hex[frame[i]&15];if(i+1<FRAME)line[n++]=' ';}
 line[n++]='\r';line[n++]='\n';return os_write(fd,line,n);}
static Byte log_pair(Byte fd,const char *tag,Byte a,Byte b,Byte c){static const char hex[]="0123456789ABCDEF";char line[32];Byte n=0;Word i;
 for(i=0;tag[i]&&i<14;i++)line[n++]=tag[i];line[n++]='=';
 line[n++]=hex[a>>4];line[n++]=hex[a&15];line[n++]=' ';
 line[n++]=hex[b>>4];line[n++]=hex[b&15];line[n++]=' ';
 line[n++]=hex[c>>4];line[n++]=hex[c&15];line[n++]='\r';line[n++]='\n';return os_write(fd,line,n);}
static Byte write_split(Byte *frame){Byte e;e=ipc_write(0,frame,5);return e?e:ipc_write(0,frame+5,FRAME-5);}
static Byte lastOperation=0, lastSyscallError=0, lastAppError=0;
static Byte shutdownAttempted=0, shutdownAckReceived=0, selfPid=0;
static Byte exchange(Byte fd,Byte *frame,Byte op,Byte seq,Byte *status){Byte ack[FRAME],e,mismatch=0,traceError;Word got=0;Byte attempts=0;
 e=write_split(frame);if(e){lastSyscallError=e;return e;}lastOperation=1;
 /* Persist and close this marker before any potentially blocking I$Read. */
 e=pp_persist_pre_ack(selfPid,1,op,seq,FRAME);if(e){lastAppError=0xfe;return PP_TRACE_LOG_FAILURE;}
 e=log_pair(fd,"PRE_ACK_MARKER_OK",op,seq,1);if(e){lastAppError=0xfe;return PP_TRACE_LOG_FAILURE;}
 e=ipc_read_observed(1,ack,FRAME,&got,op,seq,selfPid,&attempts);
 traceError=log_pair(fd,"POST_ACK_READ",e,(Byte)got,attempts);if(traceError){lastAppError=0xfe;return PP_TRACE_LOG_FAILURE;}
 if(e){lastSyscallError=e;lastOperation=2;return e;}lastOperation=2;
 traceError=log_frame(fd,"ACK",ack);if(traceError){lastAppError=0xfe;return PP_TRACE_LOG_FAILURE;}
 if(ack[0]!=MAGIC)mismatch|=1;if(ack[1]!=VERSION)mismatch|=2;
 if(ack[2]!=op)mismatch|=4;if(ack[3]!=seq)mismatch|=8;
 traceError=pp_persist_validation(op,seq,mismatch,ack[4],ack);
 if(traceError){lastAppError=0xfe;return PP_TRACE_LOG_FAILURE;}
 traceError=log_pair(fd,"ACK_VALIDATE",mismatch,ack[4],seq);
 if(traceError){lastAppError=0xfe;return PP_TRACE_LOG_FAILURE;}
 if(mismatch)return ERR_PROTOCOL;
 *status=ack[4];lastOperation=3;return 0;
}

int main(int argc,char **argv){Byte e,pid,fd=255,first=0,status,logError;Byte frame[FRAME];
 if(argc!=5)return ERR_PROTOCOL;
 e=ipc_self_pid(&pid);if(e)return e;selfPid=pid;e=ipc_priority(pid,128);if(e)return e;
 e=file_open(logName,1,&fd);if(e)return e;
 e=log_pair(fd,"CLIENT_ENTRY",pid,(Byte)argc,0);if(e){first=e;goto done;}
 e=sleep_signal();if(e){first=e;log_pair(fd,"START_SIGNAL_ERR",e,0,0);goto done;}
 log_pair(fd,"START_SIGNAL",pid,1,0);
 /* Preserve the D.1 one-shot observer hold. */
 e=os_sleep(120);if(e){first=e;log_pair(fd,"OBSERVER_HOLD_ERR",e,0,0);goto done;}
 pp25c_make_present(frame);
 log_frame(fd,"REQUEST",frame);e=exchange(fd,frame,OP_PRESENT,1,&status);
 if(e){first=e;log_pair(fd,"PRESENT_EXCHANGE_ERR",e,0,0);}else {log_pair(fd,"PRESENT_ACK_STATUS",status,1,0);if(status)first=ERR_CONTROL;}
 /* Preserve D.1 timing and exactly one shutdown transaction. */
 e=os_sleep(120);if(e&&!first)first=e;log_pair(fd,"SHUTDOWN_DELAY",e,0,0);
 memset(frame,0,FRAME);frame[0]=MAGIC;frame[1]=VERSION;frame[2]=OP_SHUTDOWN;frame[3]=2;
 log_frame(fd,"REQUEST",frame);shutdownAttempted=1;
 e=pp_persist_shutdown_begin(frame);
 if(e){lastAppError=0xfe;if(!first)first=PP_TRACE_LOG_FAILURE;goto done;}
 e=log_pair(fd,"SHUTDOWN_EXCHANGE_BEGIN",OP_SHUTDOWN,2,0);
 if(e){lastAppError=0xfe;if(!first)first=PP_TRACE_LOG_FAILURE;goto done;}
 e=exchange(fd,frame,OP_SHUTDOWN,2,&status);
 if(e&&!first)first=e;else if(!e){shutdownAckReceived=1;log_pair(fd,"SHUTDOWN_ACK_STATUS",status,2,0);if(status&&!first)first=ERR_CONTROL;}
 done:
 log_pair(fd,"CLIENT_EXIT",first,lastOperation,lastSyscallError);
 e=file_close(fd);if(!first&&e)first=e;
 logError=pp_persist_exit(lastOperation,lastSyscallError,lastAppError,
                          shutdownAttempted,shutdownAckReceived,first);
 if(logError&&!first)first=PP_TRACE_LOG_FAILURE;
 return first;
}
