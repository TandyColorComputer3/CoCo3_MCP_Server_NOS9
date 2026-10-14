/*
 * Disposable Stage 3E mixed-scene workload.
 * Uses the existing synchronous Daggorath broker/presenter client API.
 * One caller means one outstanding 16-byte broker request at a time.
 */
#include <cmoc.h>
#include "compat/present.h"
#include "compat/heartbeat-broker.h"
#include "gameplay/opening-heartbeat.h"
#include "audio/ipc.h"

#ifndef MSH_RATE
#define MSH_RATE 46
#endif
#ifndef MSH_PRESENTATIONS
#define MSH_PRESENTATIONS 8
#endif

#if MSH_RATE != 46 && MSH_RATE != 45 && MSH_RATE != 41
#error MSH_RATE must be 46, 45, or 41
#endif
#if MSH_PRESENTATIONS < 1 || MSH_PRESENTATIONS > 250
#error MSH_PRESENTATIONS must be from 1 through 250
#endif

static Byte logicalFrame[DAG_COMPAT_FRAME_BYTES];
static Byte heartPatterns[28];
static DagHeartbeatBroker broker;
static DagCompatPresentTiming timing;
static Word presentationsDone, sceneAttempts, sceneAccepted, staleScenes;
static unsigned long requests, requestAcks, snapshotAcks;
static Byte lastSequence, modulesOpen, presenterOpen, brokerStarted;
static Byte firstError;
Byte os_write(Byte path, const void *buffer, Word size);

static Word word_delta(Word now, Word before)
{ return now >= before ? now - before : (Word)(65535U - before + now + 1U); }

/* Status rows are logical rows 152..159: presenter destination rows 156..163.
 * The existing C6 presenter patches the 28 heart bytes before the status
 * request. The deterministic pattern varies by presentation and column. */
static void make_scene(Word ordinal)
{
    Word y, x;
    for (y = 0; y < DAG_COMPAT_HEIGHT; ++y) {
        for (x = 0; x < DAG_COMPAT_STRIDE; ++x) {
            Byte v = (Byte)(0x81U ^ (Byte)(x * 13U + y * 7U + ordinal * 3U));
            if (y >= 152 && y <= 159)
                v = (Byte)(0xA5U ^ (Byte)(x * 9U + y * 5U + ordinal * 17U));
            logicalFrame[(Word)(y * DAG_COMPAT_STRIDE + x)] = v;
        }
    }
}

/* Called by dag_compat_present_progress only after a completed strip. A
 * sequence delta of 1 is a scene transaction. The overlapping status strip
 * performs one SNAPSHOT plus one SCENE for each stale retry attempt, so its
 * successful callback reports 2, 4, or 6 completed transactions. */
static Byte account_present_transactions(void *unused)
{
    Byte delta = (Byte)(broker.sequence - lastSequence);
    Word writesDelta = word_delta(broker.sceneWrites, (Word)sceneAccepted);
    (void)unused;
    if (delta == 0) return 0;
    if (delta == 1) {
        if (writesDelta != 1) return 188;
        ++sceneAttempts;
        ++sceneAccepted;
    } else {
        Byte tries;
        if ((delta & 1) || delta > 6) return 188;
        tries = (Byte)(delta / 2);
        if (writesDelta > tries) return 188;
        requests += delta;
        requestAcks += delta;
        snapshotAcks += tries;
        sceneAttempts = (Word)(sceneAttempts + tries);
        sceneAccepted = (Word)(sceneAccepted + writesDelta);
        staleScenes = (Word)(staleScenes + tries - writesDelta);
        lastSequence = broker.sequence;
        return 0;
    }
    requests += delta;
    requestAcks += delta;
    lastSequence = broker.sequence;
    return 0;
}

static Byte account_single_success(Byte before)
{
    Byte delta = (Byte)(broker.sequence - before);
    if (delta != 1) return 188;
    ++requests;
    ++requestAcks;
    lastSequence = broker.sequence;
    return 0;
}

static Byte write_summary(void)
{
    Byte line[128], *p = line, path, e;
    const char *label;
    unsigned long n;
    label = "MSH rate="; while (*label) *p++ = *label++;
    *p++ = (Byte)('0' + MSH_RATE / 10); *p++ = (Byte)('0' + MSH_RATE % 10);
    label = " frames="; while (*label) *p++ = *label++;
    n = presentationsDone; { unsigned long d = 1000; Byte started=0;
        while(d){Byte v=(Byte)(n/d);if(v||started||d==1){*p++=(Byte)('0'+v);started=1;}n%=d;d/=10;}}
    label = " scene="; while (*label) *p++ = *label++;
    n = sceneAttempts; { unsigned long d = 100000; Byte started=0;
        while(d){Byte v=(Byte)(n/d);if(v||started||d==1){*p++=(Byte)('0'+v);started=1;}n%=d;d/=10;}}
    label = " accepted="; while (*label) *p++ = *label++;
    n = sceneAccepted; { unsigned long d = 100000; Byte started=0;
        while(d){Byte v=(Byte)(n/d);if(v||started||d==1){*p++=(Byte)('0'+v);started=1;}n%=d;d/=10;}}
    label = " req="; while (*label) *p++ = *label++;
    n = requests; { unsigned long d = 1000000; Byte started=0;
        while(d){Byte v=(Byte)(n/d);if(v||started||d==1){*p++=(Byte)('0'+v);started=1;}n%=d;d/=10;}}
    label = " ack="; while (*label) *p++ = *label++;
    n = requestAcks; { unsigned long d = 1000000; Byte started=0;
        while(d){Byte v=(Byte)(n/d);if(v||started||d==1){*p++=(Byte)('0'+v);started=1;}n%=d;d/=10;}}
    label = " snap="; while (*label) *p++ = *label++;
    n = snapshotAcks; { unsigned long d = 100000; Byte started=0;
        while(d){Byte v=(Byte)(n/d);if(v||started||d==1){*p++=(Byte)('0'+v);started=1;}n%=d;d/=10;}}
    label = " stale="; while (*label) *p++ = *label++;
    n = staleScenes; { unsigned long d = 100000; Byte started=0;
        while(d){Byte v=(Byte)(n/d);if(v||started||d==1){*p++=(Byte)('0'+v);started=1;}n%=d;d/=10;}}
    label = " err="; while (*label) *p++ = *label++;
    *p++ = (Byte)('0' + firstError / 100);
    *p++ = (Byte)('0' + (firstError / 10) % 10);
    *p++ = (Byte)('0' + firstError % 10);
    *p++ = 13;
    e = ipc_open("/term", &path);
    if (e) return e;
    e = os_write(path, line, (Word)(p - line));
    { Byte closeError = ipc_close(path); if (!e) e = closeError; }
    return e;
}

int main(void)
{
    Byte e = 0, cleanupError, before;
    Word ordinal;
    memset(&broker, 0, sizeof(broker));
    memset(logicalFrame, 0, sizeof(logicalFrame));
    memset(heartPatterns, 0, sizeof(heartPatterns));
    /* Same compact two-phase control heart used by the existing C6 broker
     * target; the tested invariant is status exclusion, not artwork fidelity. */
    heartPatterns[3] = heartPatterns[10] = 0x3c;
    heartPatterns[17] = heartPatterns[24] = 0x7e;
    dag_compat_present_set_hearts(heartPatterns);

    e = dag_compat_present_open();
    if (e) goto done;
    presenterOpen = 1;
    e = opening_heartbeat_modules_open();
    if (e) goto done;
    modulesOpen = 1;
    e = dag_broker_start(&broker, dag_compat_present_path(), "/d1/dodhbp", MSH_RATE);
    if (e) goto done;
    brokerStarted = 1;
    /* start() performs the fixed-frame HELLO/rate/enable startup and validates
     * each response; only RATE and ENABLE consume request sequence numbers. */
    if (broker.sequence != 2) { e = 188; goto done; }
    requests = requestAcks = 2;
    lastSequence = broker.sequence;
    dag_compat_present_set_broker(&broker);
    before = broker.sequence;
    e = dag_broker_snapshot(&broker);
    if (e) goto done;
    e = account_single_success(before);
    if (e) goto done;
    ++snapshotAcks;

    for (ordinal = 0; ordinal < MSH_PRESENTATIONS; ++ordinal) {
        make_scene(ordinal);
        e = dag_compat_present_progress(logicalFrame, &timing,
                                        account_present_transactions, 0);
        if (e) goto done;
        ++presentationsDone;
    }

    before = broker.sequence;
    e = dag_broker_stats(&broker);
    if (e) goto done;
    e = account_single_success(before);
    if (e) goto done;
    if (presentationsDone != MSH_PRESENTATIONS || sceneAccepted == 0) {
        e = 188;
        goto done;
    }

done:
    firstError = e;
    if (brokerStarted) {
        Byte closeResult = dag_broker_close(&broker);
        if (!closeResult) { ++requests; ++requestAcks; } /* shutdown ACK */
        else if (!firstError) firstError = closeResult;
        brokerStarted = 0;
    } else if (broker.pid) {
        cleanupError = dag_broker_cancel(&broker);
        if (!firstError) firstError = cleanupError;
    }
    dag_compat_present_set_broker(0);
    if (modulesOpen) {
        cleanupError = opening_heartbeat_modules_close();
        if (!firstError) firstError = cleanupError;
    }
    if (presenterOpen) {
        cleanupError = dag_compat_present_close();
        if (!firstError) firstError = cleanupError;
    }
    e = write_summary();
    if (!firstError) firstError = e;
    return firstError;
}
