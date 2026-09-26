#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "backend.h"
#include "policy.h"
#include "ssc_io.h"
static Byte ports[65536],bytes[1024],flag;static unsigned count,resets,ticks,expire;
Byte ssc_read_io(Word a){return ports[a];}
void ssc_write_io(Word a,Byte b){ports[a]=b;if(a==0xff7e){bytes[count++]=b;ports[a]=0xe0;}if(a==0xff7d)++resets;}
Byte os_sleep(Word n){ticks+=n;if(expire&&ticks>=expire)ports[0xff7e]|=32;return 0;}
Byte os_signal_value(Byte *p){return *p;}
static void active(void){ports[0xff7e]=0xc0;}
int main(void){unsigned before;int n=0;AudioVoice v={0,0,0};
 /* Exercise shared inline policy functions too, with no duplicate policy mock. */
 assert(audio_decide(&v,0)==DEC_START);audio_begin(&v,0);
 ports[0xff7e]=0xe0;assert(!backend_open("ssc-mame-fast",&flag));
 assert(count==308&&bytes[24]==0x99&&bytes[61]==0xff&&bytes[62]==0x8a&&bytes[307]==0xff);++n;
 assert(!backend_play(AUDIO_WHOOP,255));active();assert(backend_phase()==VOICE_TRANSIENT);++n;
 before=count;assert(!backend_play(AUDIO_SQUEAK,255));assert(count==before&&backend_decision()==DEC_IGNORE);++n;
 assert(!backend_play(AUDIO_PHASER,255));assert(bytes[before]==0xcf&&bytes[before+1]==0xca&&count==before+2);active();assert(backend_phase()==VOICE_REPEATING);++n;
 before=count;assert(!backend_play(AUDIO_PHASER,255));assert(count==before&&backend_decision()==DEC_COALESCE);++n;
 assert(!backend_stop()&&bytes[count-1]==0xcf&&backend_phase()==VOICE_STOPPED);++n;
 before=count;assert(!backend_play(AUDIO_SQUEAK,255));active();assert(!backend_play(AUDIO_SQUEAK,255));assert(count==before+3&&bytes[before+1]==0xcf&&bytes[before+2]==0xd8);++n;
 active();expire=ticks+5;assert(!backend_drain()&&backend_phase()==VOICE_STOPPED);++n;
 expire=0;assert(!backend_play(AUDIO_PHASER,255));ports[0xff7e]=0xe0;
 assert(backend_phase()==VOICE_IDLE);assert(!backend_play(AUDIO_SQUEAK,255)&&backend_decision()==DEC_START);++n;
 active();assert(backend_drain()==246&&backend_phase()==VOICE_STOPPED);++n;
 assert(!backend_close()&&backend_phase()==VOICE_SHUTDOWN&&resets==2);++n;
 printf("%d M2 backend state/channel checks passed\n",n);return 0;}
