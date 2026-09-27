#ifndef DOD_HEARTBEAT_H
#define DOD_HEARTBEAT_H
#include "platform.h"
/* Source: daggorath-reference a94326f, HUPDAT.ASM:HUPD00..HUPD90.
 * This isolated calculation accepts living-player inputs only. It does not
 * perform the source's faint/death/display/recovery side effects.
 * INC T6 includes the final borrowing subtraction: quotient is floor()+1.
 * All products fit the source's 24 bits for these unsigned 16-bit inputs. */
static Byte heartbeat_player_interval(Word power,Word damage,Word *interval) {
 if(!power||damage>power)return 0;
 *interval=(Word)((64UL*power)/(power+2UL*damage)+1UL-19UL);
 return 1;
}
/* Semantic countdown only; no AY, SSC, waveform synthesis or OS calls.
 * COMMON.ASM:CLK30 decrements HEARTC and reloads HEARTR only on an edge.
 * A rate update therefore does not restart the current countdown.
 * The original byte value zero denotes 256 decrements, not 'disabled'. */
typedef struct { Word interval,remaining;Byte enabled,level; } HeartbeatState;
static void heartbeat_init(HeartbeatState *s) {
 s->interval=256;s->remaining=1;s->enabled=0;s->level=0;
}
static void heartbeat_update(HeartbeatState *s,Byte intervalByte) {
 s->interval=intervalByte?intervalByte:256;s->enabled=1;
}
/* Source HBEATF gate freezes countdown/level. Backend silence on stop is a
 * separate lifecycle requirement, not a claim that WIZIX clears the PIA bit. */
static void heartbeat_disable(HeartbeatState *s) {s->enabled=0;}
/* Advance semantic time in bulk. Return edge count for observation, never a
 * request to replay a backlog of audible pulses after a scheduling delay. */
static Word heartbeat_advance(HeartbeatState *s,Word ticks) {
 Word edges;
 if(!s->enabled||!ticks)return 0;
 if(ticks<s->remaining){s->remaining-=ticks;return 0;}
 ticks-=s->remaining;edges=1+ticks/s->interval;
 s->remaining=s->interval-ticks%s->interval;
 s->level^=(Byte)(edges&1);return edges;
}
#endif
