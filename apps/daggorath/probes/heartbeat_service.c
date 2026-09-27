/* Bounded production-API integration harness. Children/services are resident
 * and forked before the heartbeat owner path exists. No guest file writes.
 * Wizard concurrency is a stress test, not intro heartbeat semantics. */
#include <cmoc.h>
#include "ipc.h"
#include "native_heartbeat.h"
static Byte signalFlag;
static NativeHeartbeat h={255,0};
static Byte pause_ticks(Word ticks){Byte e=os_sleep(ticks);return e?e:os_signal_value(&signalFlag);}
static Byte show(const char *label){NativeHeartbeatState s;Byte e=native_heartbeat_query(&h,&s);if(!e)printf("HB %s active=%u enabled=%u rate=%u remaining=%u fault=%u\r",label,s.active,s.enabled,s.rateByte,s.remainingByte,s.fault);return e?e:s.fault;}
/* F$Wait has no PID selector (upstream kernel/fwait.asm). The coordinator
 * owns observer and worker; the SSC worker owns only dodaudio. Never let
 * production audio_finish compete with an observer for a terminated child. */
static Byte reap_children(Byte *observer,Byte *peer){Byte e=0,r,pid,status;
 while(*observer||*peer){
  r=ipc_wait(&pid,&status);if(r)return r;
  if(pid==*observer){printf("REAP observer=%u status=%u\r",pid,status);*observer=0;}
  else if(pid==*peer){printf("REAP worker/window=%u status=%u\r",pid,status);*peer=0;}
  else {printf("UNEXPECTED CHILD %u\r",pid);return 187;}
  if(!e)e=status;
 }return e;
}
static Byte own_pid(void){Byte me;asm { os9 $0c
 sta :me
 }return me;}
int main(int argc,char **argv){Byte e=0,r,obs=0,wiz=0,status,pid,ssc=0,i;AudioClient audio;NativeHeartbeat second={255,0};
 const char *mode=argc==3&&!strcmp(argv[1],"worker")?argv[2]:argc==2?argv[1]:"slow";
 Byte worker=argc==3&&!strcmp(argv[1],"worker");char childargs[32];
 e=os_intercept(&signalFlag);if(e)return e;
 if(strcmp(mode,"slow")&&strcmp(mode,"fast")&&strcmp(mode,"transition")&&strcmp(mode,"freeze")&&strcmp(mode,"repeat")&&strcmp(mode,"squeak")&&strcmp(mode,"whoop")&&strcmp(mode,"phaser")&&strcmp(mode,"wizard")&&strcmp(mode,"cancel")&&strcmp(mode,"error")&&strcmp(mode,"duplicate")&&strcmp(mode,"cycles")&&strcmp(mode,"kill"))return 187;
 /* Never fork a helper after acquiring /dhb; normal shutdown additionally
  * uses explicit release, independent of inherited/duplicated references. */
 if(!worker){
  e=ipc_fork("dodsnd","observer-long\r",&obs);if(e)return e;
  printf("OWNER %u OBSERVER %u\r",own_pid(),obs);
  if(!strcmp(mode,"squeak")||!strcmp(mode,"whoop")||!strcmp(mode,"phaser")){
   strcpy(childargs,"worker ");strcat(childargs,mode);strcat(childargs,"\r");
   e=ipc_fork("hbtest",childargs,&wiz);
   printf("OWNER %u WORKER %u FORK %u\r",own_pid(),wiz,e);
   r=reap_children(&obs,&wiz);if(!e)e=r;
   printf("COORDINATOR CHILDREN 0 EXIT %u\r",e);return e;
  }
 }
 if(!strcmp(mode,"wizard")){e=ipc_fork("dodwiz","\r",&wiz);if(e)goto done;}
 if(!strcmp(mode,"squeak")||!strcmp(mode,"whoop")||!strcmp(mode,"phaser")){
  e=audio_start(&audio,"/d1/dodaudio","ssc-mame-fast");if(e)goto done;ssc=1;
  printf("AUDIO OWNER %u CHILD %u OBSERVER CHILDREN 0\r",own_pid(),audio.pid);
 }
 e=native_heartbeat_open(&h);if(e)goto done;
 e=native_heartbeat_rate(&h,!strcmp(mode,"slow")||!strcmp(mode,"transition")?46:3);if(e)goto done;
 e=native_heartbeat_enable(&h);if(e)goto done;e=show("enabled");if(e)goto done;
 if(!strcmp(mode,"transition")){
  e=pause_ticks(15);if(e)goto done;e=show("before-rate");if(e)goto done;
  e=native_heartbeat_rate(&h,3);if(e)goto done;e=show("after-rate");if(e)goto done;
 }
 if(!strcmp(mode,"freeze")||!strcmp(mode,"repeat")){
  for(i=0;i<(!strcmp(mode,"repeat")?5:1);i++){
   e=pause_ticks(10);if(e)goto done;e=native_heartbeat_disable(&h);if(e)goto done;
   e=show("frozen");if(e)goto done;e=pause_ticks(45);if(e)goto done;
   e=show("still-frozen");if(e)goto done;e=native_heartbeat_enable(&h);if(e)goto done;
  }
 }
 if(!strcmp(mode,"duplicate")){
  r=native_heartbeat_open(&second);printf("DUPLICATE %u\r",r);if(r!=250){e=187;goto done;}
 }
 if(ssc){
  for(i=0;i<3;i++){
   e=pause_ticks(45);if(e)goto done;
   e=audio_submit(&audio,AUDIO_PLAY,!strcmp(mode,"squeak")?AUDIO_SQUEAK:!strcmp(mode,"whoop")?AUDIO_WHOOP:AUDIO_PHASER,255);if(e)goto done;
   e=audio_receive(&audio);if(e)goto done;e=pause_ticks(60);if(e)goto done;
   e=audio_submit(&audio,AUDIO_STOP,0,0);if(e)goto done;e=audio_receive(&audio);if(e)goto done;e=show("after-ssc-stop");if(e)goto done;
  }
  printf("AUDIO FINISH EXPECTED %u\r",audio.pid);e=audio_finish(&audio);printf("AUDIO REAP STATUS %u\r",e);ssc=0;if(e)goto done;e=show("after-ssc-close");if(e)goto done;
 }
 if(!strcmp(mode,"cycles")){
  for(i=0;i<4;i++){e=pause_ticks(30);if(e)goto done;e=native_heartbeat_close(&h);if(e)goto done;e=native_heartbeat_open(&h);if(e)goto done;e=native_heartbeat_rate(&h,3);if(e)goto done;e=native_heartbeat_enable(&h);if(e)goto done;}
 }
 e=pause_ticks(!strcmp(mode,"cancel")||!strcmp(mode,"error")||!strcmp(mode,"kill")?90:600);if(e)goto done;
 if(!strcmp(mode,"cancel")){os_cancel_self();e=os_signal_value(&signalFlag);if(!e)e=3;}
 if(!strcmp(mode,"error"))e=187;
 if(!strcmp(mode,"kill")){Byte me;asm { os9 $0c
 sta :me
 }ipc_signal(me,0);return 187;}
 if(!e)e=show("final");
done:r=native_heartbeat_close(&second);if(!e)e=r;r=native_heartbeat_close(&h);if(!e)e=r;
 if(ssc){r=audio_finish(&audio);if(!e)e=r;}
 r=reap_children(&obs,&wiz);if(!e)e=r;
 printf("HB CLOSED EXIT %u\r",e);return e;
}
