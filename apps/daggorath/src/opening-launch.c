#include "phase-chain.h"
/* One public process identity. Wizard owns the graphics path; the following
 * phase receives that same path through F$Chain's parameter area. */
int main(void)
{
    /* Level II F$Chain unlinks the old primary module before copying U/Y.
     * Keep the parameter bytes in process data, never module rodata. */
    static char args[]="opening\r";
    return dod_chain("/d1/dodwiz",args,8);
}
