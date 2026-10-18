#include <cmoc.h>
#include "ipc.h"
#include "fileio.h"
#include "ppgraphics.h"

#define MAGIC 0xb6
#define VERSION 1
#define FRAME 16
#define OP_PRESENT 5
#define OP_SHUTDOWN 7
#define ERR_PROTOCOL 187
#define ERR_CONTROL 188
#define WAIT_POLLS 720
#define DIAG_PRE_WAIT 1
#define DIAG_POST_WAIT 2
#define DIAG_WAIT_VALIDATED 3
#define DIAG_CLEANUP_ENTER 4
#define DIAG_GRAPHICS_CLOSE 5
#define DIAG_COMMAND_CLOSE 6
#define DIAG_REPLY_CLOSE 7
#define DIAG_PARENT_LOG_CLOSE 8
#define DIAG_CLEANUP_COMPLETE 9
#define DIAG_PARENT_RETURN 10
#define DIAG_RESULT_VALID 0x01
#define DIAG_WAIT_FIELDS_VALID 0x02
#define DIAG_CARRY_SET 0x04
#define DIAG_PID_MATCH 0x08
#define DIAG_STATUS_ZERO 0x10
#define DIAG_PRIOR_IO_ERROR 0x20
static const char logName[]="/d1/ppparent3.log";
static const char childArgs[]="1 26 4 5\r";
static Byte cmdPath=255,replyPath=255,graphicsPath=255,logPath=255;
static Byte waitPid,waitStatus,childPid,childStarted=0;
/* Private D.12 records are separate 9-byte files, never appended to the
 * malformed verbose parent log. Parent-only state; no signal handler uses it. */
static Byte diagIOFailed=0;
static Byte diagRecord[9];
static const char diagHex[]="0123456789ABCDEF";
static Byte diag_checkpoint(Byte event,Byte result,Byte parent,Byte wpid,
                            Byte wstatus,Byte child,Byte flags){
 char name[13]="/d1/p12x.log";
 Byte e,closeError,fd=255,i;
 Word check=0;
 name[7]=diagHex[event];
 diagRecord[0]=0xd1;diagRecord[1]=event;diagRecord[2]=result;
 diagRecord[3]=parent;diagRecord[4]=wpid;diagRecord[5]=wstatus;
 diagRecord[6]=child;
 if(diagIOFailed)flags|=DIAG_PRIOR_IO_ERROR;
 diagRecord[7]=flags;
 for(i=0;i<8;i++)check^=diagRecord[i];
 diagRecord[8]=(Byte)check;
 e=file_open(name,1,&fd);
 if(e){diagIOFailed=1;return e;}
 e=os_write(fd,diagRecord,9);
 closeError=file_close(fd);
 if(e){diagIOFailed=1;return e;}
 if(closeError){diagIOFailed=1;return closeError;}
 return 0;
}

static Byte log_frame(Byte fd,const char *tag,const Byte *frame){Byte i;char line[64];Word n=0;
 while(tag[n]&&n<12){line[n]=tag[n];n++;}line[n++]='=';
 for(i=0;i<FRAME;i++){static const char hex[]="0123456789ABCDEF";line[n++]=hex[frame[i]>>4];line[n++]=hex[frame[i]&15];if(i+1<FRAME)line[n++]=' ';}
 line[n++]='\r';line[n++]='\n';return os_write(fd,line,n);}
static Byte log_pair(Byte fd,const char *tag,Byte a,Byte b,Byte c){static const char hex[]="0123456789ABCDEF";char line[32];Byte n=0;Word i;
 for(i=0;tag[i]&&i<14;i++)line[n++]=tag[i];line[n++]='=';
 line[n++]=hex[a>>4];line[n++]=hex[a&15];line[n++]=' ';
 line[n++]=hex[b>>4];line[n++]=hex[b&15];line[n++]=' ';
 line[n++]=hex[c>>4];line[n++]=hex[c&15];line[n++]='\r';line[n++]='\n';return os_write(fd,line,n);}
/* Preserved verbatim in ABI, argument bytes, path inheritance, priority,
 * F$Sleep(2), and standard-path restoration from accepted wakekick.c. */
static Byte bind_child(Byte self,Byte *child,Byte *started){Byte e,n,saved0=255,saved1=255;
 e=ipc_open("/pipe",&cmdPath);if(e)return e;e=ipc_open("/pipe",&replyPath);if(e)return e;
 e=ipc_dup(0,&saved0);if(e)return e;e=ipc_dup(1,&saved1);if(e)return e;
 e=ipc_close(0);if(e)return e;e=ipc_dup(cmdPath,&n);if(e||n!=0)return e?e:ERR_PROTOCOL;
 e=ipc_close(1);if(e)return e;e=ipc_dup(replyPath,&n);if(e||n!=1)return e?e:ERR_PROTOCOL;
 e=ipc_priority(self,128);if(e)return e;
 e=ipc_fork("ppclient3",childArgs,child);if(e)return e;*started=1;
 e=os_sleep(2);if(e)return e;e=ipc_priority(self,192);if(e)return e;
 e=ipc_close(0);if(e)return e;e=ipc_dup(saved0,&n);if(e||n!=0)return e?e:ERR_PROTOCOL;
 e=ipc_close(1);if(e)return e;e=ipc_dup(saved1,&n);if(e||n!=1)return e?e:ERR_PROTOCOL;
 e=ipc_close(saved0);if(e)return e;return ipc_close(saved1);
}
static Byte receive_frame(Byte *frame){Byte e,ready;Word i;
 for(i=0;i<WAIT_POLLS;i++){e=ipc_ready(cmdPath,&ready);if(e)return e;if(ready)return ipc_read(cmdPath,frame,FRAME);e=os_sleep(1);if(e)return e;}
 return ERR_CONTROL;
}
static Byte send_ack(const Byte *request,Byte status){Byte ack[FRAME],e;memset(ack,0,FRAME);ack[0]=MAGIC;ack[1]=VERSION;ack[2]=request[2];ack[3]=request[3];ack[4]=status;
 e=log_frame(logPath,"ACK",ack);if(e)return e;
 e=ipc_write(replyPath,ack,7);return e?e:ipc_write(replyPath,ack+7,FRAME-7);
}
static Byte validate_present(const Byte *f){Byte i;
 if(f[0]!=MAGIC||f[1]!=VERSION||f[2]!=OP_PRESENT||f[3]!=1)return ERR_PROTOCOL;
 if(f[4]!=0||f[5]!=0||f[6]!=0||f[7]!=64||f[8]!=0||f[9]!=64||f[10]!=8||f[11]!=8||f[12]!=1)return ERR_PROTOCOL;
 for(i=13;i<FRAME;i++)if(f[i])return ERR_PROTOCOL;
 /* Target geometry is valid, within the selected 80x25 type-5 window, and
  * uses rows 64..71, outside protected heartbeat rows 156..162. */
 if(f[7]+8>640||f[9]+8>200||f[9]>=152)return ERR_PROTOCOL;
 return 0;
}
static Byte validate_shutdown(const Byte *f){Byte i;
 if(f[0]!=MAGIC||f[1]!=VERSION||f[2]!=OP_SHUTDOWN||f[3]!=2)return ERR_PROTOCOL;
 for(i=4;i<FRAME;i++)if(f[i])return ERR_PROTOCOL;
 return 0;
}
static Byte wait_child(Byte parent){Byte e,rawResult,rawPid,rawStatus,validFlags;
 diag_checkpoint(DIAG_PRE_WAIT,0,parent,255,255,childPid,0);
 e=ipc_wait(&waitPid,&waitStatus);
 /* Save all F$Wait results before diagnostic I/O can change machine state. */
 rawResult=e;rawPid=e?255:waitPid;rawStatus=e?255:waitStatus;
 validFlags=DIAG_RESULT_VALID;
 if(e)validFlags|=DIAG_CARRY_SET;
 else {validFlags|=DIAG_WAIT_FIELDS_VALID;
  if(rawPid==childPid)validFlags|=DIAG_PID_MATCH;
  if(rawStatus==0)validFlags|=DIAG_STATUS_ZERO;
 }
 diag_checkpoint(DIAG_POST_WAIT,rawResult,parent,rawPid,rawStatus,childPid,validFlags);
 if(e)return e;
 log_pair(logPath,"FWAIT",waitPid,waitStatus,childPid);
 if(waitPid!=childPid){e=ERR_CONTROL;validFlags=DIAG_RESULT_VALID|DIAG_WAIT_FIELDS_VALID;
  if(waitStatus==0)validFlags|=DIAG_STATUS_ZERO;
  diag_checkpoint(DIAG_WAIT_VALIDATED,e,parent,waitPid,waitStatus,childPid,validFlags);return e;}
 if(waitStatus!=0){e=waitStatus;validFlags=DIAG_RESULT_VALID|DIAG_WAIT_FIELDS_VALID|DIAG_PID_MATCH;
  diag_checkpoint(DIAG_WAIT_VALIDATED,e,parent,waitPid,waitStatus,childPid,validFlags);return e;}
 validFlags=DIAG_RESULT_VALID|DIAG_WAIT_FIELDS_VALID|DIAG_PID_MATCH|DIAG_STATUS_ZERO;
 diag_checkpoint(DIAG_WAIT_VALIDATED,0,parent,waitPid,waitStatus,childPid,validFlags);
 return 0;
}
static Byte cleanup(Byte error,Byte parent){Byte e;diag_checkpoint(DIAG_CLEANUP_ENTER,error,parent,255,255,childPid,DIAG_RESULT_VALID);if(childStarted){
  if(error){ipc_signal(childPid,2);}
  e=wait_child(parent);if(!error)error=e;childStarted=0;
 }
 if(graphicsPath!=255){e=pp_graphics_close(graphicsPath);log_pair(logPath,"GRAPHICS_CLOSE",e,0,0);diag_checkpoint(DIAG_GRAPHICS_CLOSE,e,parent,255,255,childPid,DIAG_RESULT_VALID);if(!error)error=e;graphicsPath=255;}
 if(cmdPath!=255){e=ipc_close(cmdPath);log_pair(logPath,"CMD_CLOSE",e,0,0);diag_checkpoint(DIAG_COMMAND_CLOSE,e,parent,255,255,childPid,DIAG_RESULT_VALID);if(!error)error=e;cmdPath=255;}
 if(replyPath!=255){e=ipc_close(replyPath);log_pair(logPath,"REPLY_CLOSE",e,0,0);diag_checkpoint(DIAG_REPLY_CLOSE,e,parent,255,255,childPid,DIAG_RESULT_VALID);if(!error)error=e;replyPath=255;}
 if(logPath!=255){log_pair(logPath,"PARENT_EXIT",error,0,0);e=file_close(logPath);logPath=255;diag_checkpoint(DIAG_PARENT_LOG_CLOSE,e,parent,255,255,childPid,DIAG_RESULT_VALID);if(!error)error=e;}
 diag_checkpoint(DIAG_CLEANUP_COMPLETE,error,parent,255,255,childPid,DIAG_RESULT_VALID);
 return error;
}
int main(void){Byte e,self,requestNo,status,opError,first=0;Byte frame[FRAME];
 e=ipc_self_pid(&self);if(e){diag_checkpoint(DIAG_PARENT_RETURN,e,255,255,255,255,DIAG_RESULT_VALID);return e;}
 e=ipc_priority(self,192);if(e){diag_checkpoint(DIAG_PARENT_RETURN,e,self,255,255,255,DIAG_RESULT_VALID);return e;}
 e=file_open(logName,1,&logPath);if(e){diag_checkpoint(DIAG_PARENT_RETURN,e,self,255,255,255,DIAG_RESULT_VALID);return e;}log_pair(logPath,"PARENT_ENTRY",self,0,0);
 e=pp_graphics_open(&graphicsPath);log_pair(logPath,"WINDOW_SETUP",e,graphicsPath,0);if(e){e=cleanup(e,self);diag_checkpoint(DIAG_PARENT_RETURN,e,self,255,255,childPid,DIAG_RESULT_VALID);return e;}
 /* A distinct, low-screen activation marker is drawn by the sole owner. The
  * bounded hold permits a completed-frame observer to verify it before fork. */
 e=pp_graphics_draw(graphicsPath,64,32);log_pair(logPath,"ACTIVATION_PUT",e,64,32);if(e){e=cleanup(e,self);diag_checkpoint(DIAG_PARENT_RETURN,e,self,255,255,childPid,DIAG_RESULT_VALID);return e;}
 e=os_sleep(180);log_pair(logPath,"ACTIVATION_HOLD",e,0,0);if(e){e=cleanup(e,self);diag_checkpoint(DIAG_PARENT_RETURN,e,self,255,255,childPid,DIAG_RESULT_VALID);return e;}
 e=bind_child(self,&childPid,&childStarted);log_pair(logPath,"FFORK_CHILD",e,childPid,0);if(e){e=cleanup(e,self);diag_checkpoint(DIAG_PARENT_RETURN,e,self,255,255,childPid,DIAG_RESULT_VALID);return e;}
 e=ipc_signal(childPid,1);log_pair(logPath,"CLIENT_START",e,childPid,1);if(e){e=cleanup(e,self);diag_checkpoint(DIAG_PARENT_RETURN,e,self,255,255,childPid,DIAG_RESULT_VALID);return e;}
 for(requestNo=0;requestNo<2;requestNo++){
  e=receive_frame(frame);if(e){first=e;log_pair(logPath,"RECV_ERROR",e,requestNo,0);break;}
  log_frame(logPath,"REQUEST",frame);status=0;
  if(requestNo==0){opError=validate_present(frame);if(opError)status=1;else {opError=pp_graphics_draw(graphicsPath,((Word)frame[6]<<8)|frame[7],((Word)frame[8]<<8)|frame[9]);if(opError)status=1;}
   log_pair(logPath,"PRESENT_PUT",opError,frame[7],frame[9]);
  }else {opError=validate_shutdown(frame);if(opError)status=1;log_pair(logPath,"SHUTDOWN_REQUEST",opError,frame[2],frame[3]);}
  e=send_ack(frame,status);log_pair(logPath,"ACK_SENT",e,frame[2],frame[3]);if(e){first=e;break;}
  if(status&&!first)first=ERR_CONTROL;
 }
 e=wait_child(self);log_pair(logPath,"FWAIT_RESULT",e,waitPid,waitStatus);if(!first)first=e;childStarted=0;
 e=cleanup(first,self);diag_checkpoint(DIAG_PARENT_RETURN,e,self,255,255,childPid,DIAG_RESULT_VALID);return e;
}
