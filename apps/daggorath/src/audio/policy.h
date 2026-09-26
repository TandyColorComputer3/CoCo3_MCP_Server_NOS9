#ifndef DOD_AUDIO_POLICY_H
#define DOD_AUDIO_POLICY_H
#include "audio.h"
/* Port policy, not a claim that the synchronous original mixed sounds.
 * SOUNDS.ASM PHASER repeats ten sweeps; WHOOP is a single scroll sweep. */
#define VOICE_IDLE 0
#define VOICE_TRANSIENT 1
#define VOICE_REPEATING 2
#define VOICE_REPLACING 3
#define VOICE_STOPPED 4
#define VOICE_SHUTDOWN 5
#define DEC_START 1
#define DEC_RETRIGGER 2
#define DEC_REPLACE 3
#define DEC_IGNORE 4
#define DEC_COALESCE 5
#define DEC_STOP 6
#define DEC_COMPLETE 7

typedef struct { Byte phase,event,decision; } AudioVoice;
static Byte audio_priority(Byte event) {
 return event==AUDIO_PHASER?3:event==AUDIO_WHOOP?2:1;
}
static Byte audio_decide(AudioVoice *v,Byte event) {
 Byte active=v->phase==VOICE_TRANSIENT||v->phase==VOICE_REPEATING;
 if(!active)return v->decision=DEC_START;
 if(v->event==event)return v->decision=event==AUDIO_PHASER?DEC_COALESCE:DEC_RETRIGGER;
 return v->decision=audio_priority(event)<audio_priority(v->event)?DEC_IGNORE:DEC_REPLACE;
}
static void audio_begin(AudioVoice *v,Byte event) {
 v->event=event;v->phase=event==AUDIO_PHASER?VOICE_REPEATING:VOICE_TRANSIENT;
}
#endif
