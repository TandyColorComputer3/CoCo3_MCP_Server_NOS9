#include <assert.h>
#include <stdio.h>
#include "policy.h"
int main(void){AudioVoice v={VOICE_IDLE,0,0};Byte f[8];int n=0;
 audio_frame(f,AUDIO_PLAY,AUDIO_WHOOP,255,1);assert(!audio_validate(f));++n;
 audio_frame(f,AUDIO_PLAY,AUDIO_PHASER,255,2);assert(!audio_validate(f));++n;
 assert(audio_decide(&v,AUDIO_SQUEAK)==DEC_START);audio_begin(&v,AUDIO_SQUEAK);assert(v.phase==VOICE_TRANSIENT);++n;
 assert(audio_decide(&v,AUDIO_SQUEAK)==DEC_RETRIGGER);++n;
 assert(audio_decide(&v,AUDIO_WHOOP)==DEC_REPLACE);audio_begin(&v,AUDIO_WHOOP);++n;
 assert(audio_decide(&v,AUDIO_SQUEAK)==DEC_IGNORE&&v.event==AUDIO_WHOOP);++n;
 assert(audio_decide(&v,AUDIO_PHASER)==DEC_REPLACE);audio_begin(&v,AUDIO_PHASER);assert(v.phase==VOICE_REPEATING);++n;
 assert(audio_decide(&v,AUDIO_PHASER)==DEC_COALESCE);++n;
 assert(audio_decide(&v,AUDIO_WHOOP)==DEC_IGNORE);++n;
 v.phase=VOICE_STOPPED;assert(audio_decide(&v,AUDIO_SQUEAK)==DEC_START);++n;
 v.phase=VOICE_IDLE;v.decision=DEC_COMPLETE;assert(audio_decide(&v,AUDIO_WHOOP)==DEC_START);++n;
 printf("%d M2 semantic policy checks passed\n",n);return 0;}
