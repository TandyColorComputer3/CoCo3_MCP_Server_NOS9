#include <cmoc.h>
#include "platform.h"
Byte window_open(Byte *fd);
Byte window_close(Byte fd);
/* Public escape packets: NitrOS-9 Windowing System, DWSet/Select/DWEnd,
 * ScaleSw/Palette/SetDPtr/Box/Line. Confirmed against upstream CoWin L0027
 * at f470fa52. GrfDrv L086A.25 selects 640x200 for type 5, 25 rows.
 * Query cells/type before selecting; live screenshot verifies pixel extent. */
static const Byte setup[]={27,0x20,5,0,0,80,25,1,0,0,
    27,0x35,0,27,0x31,0,0,27,0x31,1,63};
static const Byte selectWindow[]={27,0x21};
static const Byte endWindow[]={27,0x24};
/* Full screen outline plus centered 256x192 rectangle and diagonals.
 * Side bands stay black. Actual coordinates are 16-bit big endian. */
static const Byte pattern[]={
    27,0x40,0,0,0,0,27,0x48,2,127,0,199,
    27,0x40,0,192,0,4,27,0x48,1,191,0,195,
    27,0x40,0,192,0,4,27,0x44,1,191,0,195,
    27,0x40,1,191,0,4,27,0x44,0,192,0,195};
int main(int argc,char **argv)
{
    Byte fd=255, error=0, cleanup=0, signal=0, defined=0;
    Word i; Registers r; int cancel=0, fail=0;
    if(argc>2)return ERR_ARGUMENT;
    if(argc==2){cancel=!strcmp(argv[1],"cancel");fail=!strcmp(argv[1],"error");if(!cancel&&!fail)return ERR_ARGUMENT;}
    error=os_intercept(&signal);if(error)return error;
    error=window_open(&fd);if(error)return error;
    error=os_write(fd,setup,sizeof(setup));if(error)goto done;
    defined=1;
    memset(&r,0,sizeof(r));error=os_getstat(fd,SS_SCTYP,&r);if(error)goto done;
    if(r.a!=5){error=ERR_ARGUMENT;goto done;}
    memset(&r,0,sizeof(r));error=os_getstat(fd,SS_SCSIZ,&r);if(error)goto done;
    if(r.x!=80||r.y!=25){error=ERR_ARGUMENT;goto done;}
    error=os_write(fd,pattern,sizeof(pattern));if(error)goto done;
    error=os_write(fd,selectWindow,sizeof(selectWindow));if(error)goto done;
    for(i=0;i<600;++i){
        if(i==180&&cancel){error=os_cancel_self();if(error)break;}
        if(i==180&&fail){error=ERR_ARGUMENT;break;}
        error=os_signal_value(&signal);if(error)break;
        error=os_sleep(1);if(error)break;
    }
done:
    /* Always select original stdout Term, end only the owned window, close it.
     * No terminal options, font, palette or dimensions on Term are modified. */
    cleanup=os_write(1,selectWindow,sizeof(selectWindow));
    if(defined){Byte e=os_write(fd,endWindow,sizeof(endWindow));if(!cleanup)cleanup=e;}
    {Byte e=window_close(fd);if(!cleanup)cleanup=e;}
    if(cleanup)return cleanup;
    {const char *msg="GFXPROBE TERM RESTORED type5 80x25\r";
     Byte e=os_write(1,msg,strlen(msg));if(e)return e;}
    return error;
}
