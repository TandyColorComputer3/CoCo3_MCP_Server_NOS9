#ifndef DOD_AUDIO_H
#define DOD_AUDIO_H
#include "platform.h"
/* Versioned semantic wire format; no hardware registers cross this boundary.
 * ID 0: SOUNDS.ASM SNDTAB A$SQK0 -> SQUEAK (spider), original gain 0..255.
 * M1 accepts full gain only; unsupported parameters fail explicitly. */
#define AUDIO_VERSION 1
#define AUDIO_FRAME 8
#define AUDIO_MAGIC 0xda
#define AUDIO_SQUEAK 0
#define AUDIO_PHASER 13 /* SOUNDS.ASM A$RING, ring attack/incantation */
#define AUDIO_WHOOP 14 /* SOUNDS.ASM A$SCRO, scroll */
#define AUDIO_PLAY 1
#define AUDIO_STOP 2
#define AUDIO_DRAIN 3
#define AUDIO_SHUTDOWN 4
#define AUDIO_BAD 187
#define AUDIO_UNSUPPORTED 208
#define AUDIO_BUSY 209
#define AUDIO_IO 245
#define AUDIO_EOF 211
typedef struct { Byte command, reply, pid, sequence, pending, opened; } AudioClient;
Byte audio_validate(const Byte *frame);
void audio_frame(Byte *frame, Byte op, Byte sound, Byte gain, Byte sequence);
Byte audio_start(AudioClient *client, const char *service, const char *profile);
Byte audio_submit(AudioClient *client, Byte op, Byte sound, Byte gain);
Byte audio_receive(AudioClient *client);
Byte audio_finish(AudioClient *client);
Byte audio_cancel(AudioClient *client);
#endif
