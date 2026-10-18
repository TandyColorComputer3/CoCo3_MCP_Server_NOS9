#include <cmoc.h>
#include "ackread.h"
#include "fileio.h"
#include "ipc.h"
#include "platform.h"

#define ERR_DIAG_IO 0xfe

static const char hexDigits[] = "0123456789ABCDEF";
/* One synchronous child owns these diagnostic buffers; signal handler does not use them. */
static char traceName[20];
static char traceRecord[160];
static void set_trace_name(const char *name)
{
    Word i = 0;
    while (name[i] && i < sizeof(traceName) - 1) { traceName[i] = name[i]; ++i; }
    traceName[i] = 0;
}
static void put_text(char *out, Word *used, const char *text)
{
    while (*text) out[(*used)++] = *text++;
}
static void put_hex8(char *out, Word *used, Byte value)
{
    out[(*used)++] = hexDigits[value >> 4];
    out[(*used)++] = hexDigits[value & 15];
}
static void put_hex16(char *out, Word *used, Word value)
{
    put_hex8(out, used, (Byte)(value >> 8));
    put_hex8(out, used, (Byte)value);
}
static Byte persist(const char *name, const char *record, Word length)
{
    Byte error, closeError, fd = 255;
    error = file_open(name, 1, &fd);
    if (error) return error;
    error = os_write(fd, record, length);
    closeError = file_close(fd);
    return error ? error : closeError;
}
static void read_record_name(Byte opcode, Byte attempt, Byte after)
{
    /* /d1/p3r<op><attempt-hi><attempt-lo><b|a>.log; unique per read. */
    static const char templateName[] = "/d1/p3r000b.log";
    Word i;
    for (i = 0; templateName[i]; ++i) traceName[i] = templateName[i];
    traceName[i] = 0;
    traceName[7] = hexDigits[opcode & 15];
    traceName[8] = hexDigits[(attempt >> 4) & 15];
    traceName[9] = hexDigits[attempt & 15];
    traceName[10] = after ? 'a' : 'b';
}
Byte pp_persist_pre_ack(Byte pid, Byte path, Byte opcode, Byte sequence,
                        Byte frameLength)
{
    Word n = 0;
    set_trace_name("/d1/ppc3p0.log");
    traceName[9] = hexDigits[sequence & 15];
    put_text(traceRecord, &n, "PRE_ACK_READ marker=01 pid="); put_hex8(traceRecord, &n, pid);
    put_text(traceRecord, &n, " path="); put_hex8(traceRecord, &n, path);
    put_text(traceRecord, &n, " op="); put_hex8(traceRecord, &n, opcode);
    put_text(traceRecord, &n, " seq="); put_hex8(traceRecord, &n, sequence);
    put_text(traceRecord, &n, " len="); put_hex8(traceRecord, &n, frameLength);
    put_text(traceRecord, &n, " buffer=UNINITIALIZED_STACK\r\n");
    return persist(traceName, traceRecord, n);
}
Byte pp_persist_validation(Byte opcode, Byte sequence, Byte mismatchMask,
                           Byte status, const Byte *frame)
{
    Word n = 0;
    Byte i;
    const char *name;
    if (sequence == 1) name = "/d1/ppc3v1.log";
    else if (sequence == 2) name = "/d1/ppc3v2.log";
    else return ERR_DIAG_IO;
    set_trace_name(name);
    put_text(traceRecord, &n, "ACK_VALIDATE op="); put_hex8(traceRecord, &n, opcode);
    put_text(traceRecord, &n, " seq="); put_hex8(traceRecord, &n, sequence);
    put_text(traceRecord, &n, " mismatch="); put_hex8(traceRecord, &n, mismatchMask);
    put_text(traceRecord, &n, " status="); put_hex8(traceRecord, &n, status);
    put_text(traceRecord, &n, " frame=");
    for (i = 0; i < 16; ++i) put_hex8(traceRecord, &n, frame[i]);
    put_text(traceRecord, &n, "\r\n");
    return persist(traceName, traceRecord, n);
}
Byte pp_persist_shutdown_begin(const Byte *frame)
{
    Word n = 0;
    Byte i;
    set_trace_name("/d1/ppc3shut.log");
    put_text(traceRecord, &n, "SHUTDOWN_EXCHANGE_BEGIN frame=");
    for (i = 0; i < 16; ++i) put_hex8(traceRecord, &n, frame[i]);
    put_text(traceRecord, &n, "\r\n");
    return persist(traceName, traceRecord, n);
}
Byte pp_persist_exit(Byte lastOperation, Byte syscallError,
                     Byte appError, Byte shutdownAttempted,
                     Byte shutdownAckReceived, Byte requestedStatus)
{
    Word n = 0;
    set_trace_name("/d1/ppc3exit.log");
    put_text(traceRecord, &n, "PRE_MAIN_RETURN last="); put_hex8(traceRecord, &n, lastOperation);
    put_text(traceRecord, &n, " syscall="); put_hex8(traceRecord, &n, syscallError);
    put_text(traceRecord, &n, " app="); put_hex8(traceRecord, &n, appError);
    put_text(traceRecord, &n, " shutdownAttempt="); put_hex8(traceRecord, &n, shutdownAttempted);
    put_text(traceRecord, &n, " shutdownAck="); put_hex8(traceRecord, &n, shutdownAckReceived);
    put_text(traceRecord, &n, " requestedStatus="); put_hex8(traceRecord, &n, requestedStatus);
    put_text(traceRecord, &n, " mainReturn=NEXT\r\n");
    return persist(traceName, traceRecord, n);
}
static Byte persist_io_record(Byte opcode, Byte sequence, Byte attempt,
                              Byte after, Byte path, Word requested,
                              Word accumulated, Byte carry, Byte rawCc,
                              Byte rawA, Byte rawB, Word rawY,
                              Byte yValid, Byte rawError,
                              const Byte *data, Byte dataCount)
{
    Word n = 0;
    Byte i;
    read_record_name(opcode, attempt, after);
    if (!after) {
        put_text(traceRecord, &n, "READ_BEGIN op="); put_hex8(traceRecord, &n, opcode);
        put_text(traceRecord, &n, " seq="); put_hex8(traceRecord, &n, sequence);
        put_text(traceRecord, &n, " attempt="); put_hex8(traceRecord, &n, attempt);
        put_text(traceRecord, &n, " path="); put_hex8(traceRecord, &n, path);
        put_text(traceRecord, &n, " req="); put_hex16(traceRecord, &n, requested);
        put_text(traceRecord, &n, " acc="); put_hex16(traceRecord, &n, accumulated);
        put_text(traceRecord, &n, " remaining-buffer=UNINITIALIZED\r\n");
    } else {
        put_text(traceRecord, &n, "READ_RESULT op="); put_hex8(traceRecord, &n, opcode);
        put_text(traceRecord, &n, " seq="); put_hex8(traceRecord, &n, sequence);
        put_text(traceRecord, &n, " attempt="); put_hex8(traceRecord, &n, attempt);
        put_text(traceRecord, &n, " path="); put_hex8(traceRecord, &n, path);
        put_text(traceRecord, &n, " req="); put_hex16(traceRecord, &n, requested);
        put_text(traceRecord, &n, " carry="); put_hex8(traceRecord, &n, carry);
        put_text(traceRecord, &n, " cc="); put_hex8(traceRecord, &n, rawCc);
        put_text(traceRecord, &n, " A="); put_hex8(traceRecord, &n, rawA);
        put_text(traceRecord, &n, " B="); put_hex8(traceRecord, &n, rawB);
        put_text(traceRecord, &n, " Yraw="); put_hex16(traceRecord, &n, rawY);
        put_text(traceRecord, &n, " Yvalid="); put_hex8(traceRecord, &n, yValid);
        put_text(traceRecord, &n, " acc="); put_hex16(traceRecord, &n, accumulated);
        put_text(traceRecord, &n, " error="); put_hex8(traceRecord, &n, rawError);
        put_text(traceRecord, &n, " data=");
        for (i = 0; i < dataCount; ++i) put_hex8(traceRecord, &n, data[i]);
        put_text(traceRecord, &n, "\r\n");
    }
    return persist(traceName, traceRecord, n);
}

Byte ipc_read_observed(Byte path, Byte *data, Word count, Word *transferred,
                       Byte opcode, Byte sequence, Byte pid,
                       Byte *attemptCount)
{
    Byte error, rawA, rawB, rawCc, carry, attempt = 0;
    Word rawY, total = 0, request;
    Byte *readAt;
    *transferred = 0;
    *attemptCount = 0;
    while (total < count) {
        request = count - total;
        readAt = data + total;
        ++attempt;
        if (attempt > 16) return ERR_DIAG_IO;
        error = persist_io_record(opcode, sequence, attempt, 0, path,
                                  request, total, 0, 0, 0, 0, 0, 0, 0,
                                  data + total, 0);
        if (error) return ERR_DIAG_IO;
        /* I$Read entry A=path, X=buffer, Y=requested bytes. EOU Technical
         * Reference, Chapter 9, I$Read: on success Y=actual bytes; on error
         * CC.C=1 and B=error. Capture raw registers/CC before any C call. */
        asm {
            pshs y
            lda :path
            ldx :readAt
            ldy :request
            os9 $89
            sta :rawA
            stb :rawB
            sty :rawY
            tfr cc,a
            sta :rawCc
            bcs @readerr
            clrb
@readerr
            puls y
            stb :error
        }
        carry = (Byte)(rawCc & 1);
        ++*attemptCount;
        if (carry) {
            error = persist_io_record(opcode, sequence, attempt, 1, path,
                                      request, total, carry, rawCc, rawA, rawB,
                                      rawY, 0, error, data + total, 0);
            *transferred = total;
            return error ? ERR_DIAG_IO : (Byte)rawB;
        }
        if (rawY > request) {
            error = persist_io_record(opcode, sequence, attempt, 1, path,
                                      request, total, carry, rawCc, rawA, rawB,
                                      rawY, 0, 0, data + total, 0);
            *transferred = total;
            return error ? ERR_DIAG_IO : AUDIO_IO;
        }
        error = persist_io_record(opcode, sequence, attempt, 1, path,
                                  request, total, carry, rawCc, rawA, rawB,
                                  rawY, 1, 0, data + total, (Byte)rawY);
        if (error) return ERR_DIAG_IO;
        if (!rawY) {
            *transferred = total;
            return AUDIO_IO;
        }
        total += rawY;
    }
    *transferred = total;
    return 0;
}
