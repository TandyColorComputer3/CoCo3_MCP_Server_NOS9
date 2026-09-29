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
#define AUDIO_RATTLE 1
#define AUDIO_GROWL 2
#define AUDIO_BEOOP 3
#define AUDIO_KLANK 4
#define AUDIO_GRAWL 5
#define AUDIO_PSSST 6
#define AUDIO_KKLANK 7
#define AUDIO_PSSHT 8
#define AUDIO_SNARL 9
#define AUDIO_WIZARD_1 10
#define AUDIO_WIZARD_2 11
#define AUDIO_GLUGLG 12
#define AUDIO_PHASER 13 /* SOUNDS.ASM A$RING, ring attack/incantation */
#define AUDIO_WHOOP 14 /* SOUNDS.ASM A$SCRO, scroll */
#define AUDIO_CLANG 15
#define AUDIO_WHOOSH 16
#define AUDIO_CHUCK 17
#define AUDIO_KLINK 18
#define AUDIO_CLANK 19
#define AUDIO_THUD 20
#define AUDIO_BANG 21
#define AUDIO_KABOOM 22
#define AUDIO_LAST_SOURCE_ID AUDIO_KABOOM
#define AUDIO_PLAY 1
#define AUDIO_STOP 2
#define AUDIO_DRAIN 3
#define AUDIO_SHUTDOWN 4
/* Version-1 compatibility: PLAY retains its M1/M2 three-sound allowlist.
 * CATALOG_PLAY explicitly opts into the full SOUNDS.ASM dispatch range. */
#define AUDIO_CATALOG_PLAY 5
#define AUDIO_BAD 187
#define AUDIO_UNSUPPORTED 208
#define AUDIO_BUSY 209
#define AUDIO_IO 245
#define AUDIO_EOF 211
typedef struct { Byte command, reply, pid, sequence, pending, opened; } AudioClient;
#define AUDIO_OPTIONAL_NEW 0
#define AUDIO_OPTIONAL_READY 1
#define AUDIO_OPTIONAL_DISABLED 2
Byte audio_validate(const Byte *frame);
void audio_frame(Byte *frame, Byte op, Byte sound, Byte gain, Byte sequence);
Byte audio_start(AudioClient *client, const char *service, const char *profile);
Byte audio_submit(AudioClient *client, Byte op, Byte sound, Byte gain);
Byte audio_receive(AudioClient *client);
Byte audio_finish(AudioClient *client);
Byte audio_cancel(AudioClient *client);
void audio_present_optional(AudioClient *client, Byte *state,
                            const Byte *events, Byte eventCount,
                            const char *service, const char *profile);
#endif
