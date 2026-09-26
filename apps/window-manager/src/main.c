#include <cmoc.h>
#include "ui.h"

static Byte cancelled;

int main(int argc, char **argv)
{
    WindowInfo original;
    WindowColors test, observed;
    Byte error=0, restoreError=0, changed=0, mode=0, signal;
    Word tick;
    Registers unused;
    unused.x=0;
    /* Inspect by default; all mutation modes require explicit selection. */
    if (argc > 2) return ERR_ARGUMENT;
    if (argc == 2) {
        if (!strcmp(argv[1], "demo")) mode=1;
        else if (!strcmp(argv[1], "fault")) mode=2;
        else if (!strcmp(argv[1], "cancel")) mode=3;
        else { ui_line("Usage: wmview [demo|fault|cancel]"); return ERR_ARGUMENT; }
    }
    cancelled=0;
    error=os_intercept(&cancelled);
    if (error) goto cleanup;
    error=window_query(1, &original);
    if (error) goto cleanup;
    error=ui_info(&original);
    if (error || !mode) goto cleanup;
    error=window_demo_colors(&original, &test);
    if (error || os_signal_value(&cancelled)) goto cleanup;
    /* Set before the first write: even partial failures attempt restoration. */
    changed=1;
    error=window_set_colors(1, &test);
    if (error) goto cleanup;
    error=window_colors(1, &observed);
    if (error) goto cleanup;
    if (!window_same_colors(&test, &observed)) { error=ERR_WRITE; goto cleanup; }
    error=ui_colors("DEMO ACTIVE", &observed);
    if (error) goto cleanup;
    error=ui_line("Temporary colors; restoring after 600 ticks. Abort/interrupt cancels.");
    if (error) goto cleanup;
    for (tick=0; tick<600 && !os_signal_value(&cancelled); ++tick) {
        /* Explicit diagnostics exercise real OS errors and real intercept delivery. */
        if (tick==120 && mode==2) {
            error=os_getstat(255, SS_SCTYP, &unused);
            if (!error) error=ERR_WRITE;
            goto cleanup;
        }
        if (tick==120 && mode==3) {
            error=os_cancel_self();
            if (error) goto cleanup;
        }
        error=os_sleep(1);
        if (error) goto cleanup;
    }
cleanup:
    if (changed) {
        restoreError=window_restore(1, &original.colors);
        if (!restoreError) {
            Byte outputError=ui_colors("RESTORED (verified)", &original.colors);
            if (!error) error=outputError;
        } else ui_result("RESTORE FAILED", restoreError);
    }
    signal=os_signal_value(&cancelled);
    if (signal) ui_result("Cancelled by signal", signal);
    if (error) ui_result("OS-9 error", error);
    if (restoreError) return restoreError;
    if (error) return error;
    return signal ? signal : 0;
}
