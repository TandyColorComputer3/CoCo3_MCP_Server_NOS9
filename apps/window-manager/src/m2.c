#include <cmoc.h>
#include "m2.h"
#include "ui.h"
static Byte m2_cancelled;

Byte m2_command(const char *s)
{
    return !strcmp(s,"capture") || !strcmp(s,"create") || !strcmp(s,"show")
        || !strcmp(s,"apply") || !strcmp(s,"revert") || !strcmp(s,"preview")
        || !strcmp(s,"caps") || !strcmp(s,"fonts") || !strcmp(s,"help");
}
static Byte profile_print(const WindowProfile *p)
{
    char line[96];
    sprintf(line,"Profile v1 %s: foreground=%u background=%u",p->name,
        (unsigned)p->foreground,(unsigned)p->background);
    return ui_line(line);
}
static Byte capabilities(void)
{
    static const char *lines[]={
        "Current window capabilities (CoWin; probe errors remain authoritative):",
        "fg/bg: query/set/restore; v1 application limited to hardware text 1/2",
        "border: query/set, SCREEN-SCOPED; excluded from profiles",
        "palette: public query/set, SCREEN-SCOPED; M2 has no palette editor",
        "type/area: query; screen-mode change requires recreation (not M2)",
        "font: graphics selection only; current-font query UNVERIFIED/UNSUPPORTED",
        "font restore: only known owned baseline; M2 NEVER changes fonts",
        "font catalog: file evidence, NOT enumeration of resident buffers",
        "cursor: settable; public CoWin query/restore unverified; unchanged"
    };
    Byte i,e;
    for(i=0;i<9;++i) { e=ui_line(lines[i]); if(e) return e; }
    return 0;
}
static Byte help(void)
{
    static const char *lines[]={
        "wmview | caps | fonts | help",
        "wmview capture NAME /device/newfile",
        "wmview create NAME FG BG /device/newfile",
        "wmview show /device/profile",
        "wmview apply /device/profile /device/NEW-undo",
        "wmview revert /device/undo",
        "wmview preview /device/profile [cancel]",
        "Apply/revert KEEP verified fg/bg on success. Errors restore entry colors.",
        "Preview always restores. Saves NEVER replace files. Use RBF file paths.",
        "Only hardware text types 1/2; border/font/palette unchanged."
    };
    Byte i,e;
    for(i=0;i<10;++i) {e=ui_line(lines[i]);if(e)return e;}
    return 0;
}
int m2_main(int argc, char **argv)
{
    WindowInfo original;
    WindowProfile requested, undo;
    WindowColors actual;
    Byte error=0, restoreError=0, changed=0, commit=0, signal=0;
    Byte preview=0, cancelTest=0;
    Word tick;
    const char *action=argv[1];
    m2_cancelled=0;
    error=os_intercept(&m2_cancelled);
    if(error) goto done;
    if(!strcmp(action,"help")) {error=argc==2 ? help() : ERR_ARGUMENT;goto done;}
    if(!strcmp(action,"fonts")) {error=argc==2 ? font_catalog() : ERR_ARGUMENT;goto done;}
    if(!strcmp(action,"caps")) {
        if(argc!=2) {error=ERR_ARGUMENT;goto done;}
        error=window_query(1,&original);
        if(!error) error=ui_info(&original);
        if(!error) error=capabilities();
        goto done;
    }
    if(!strcmp(action,"create")) {
        if(argc!=6 || profile_name(argv[2]) || profile_color(argv[3],&requested.foreground)
            || profile_color(argv[4],&requested.background)) {error=ERR_ARGUMENT;goto done;}
        strcpy(requested.name,argv[2]);
        if(os_signal_value(&m2_cancelled)) goto done;
        error=profile_save(argv[5],&requested);
        if(!error) error=profile_print(&requested);
        goto done;
    }
    if(!strcmp(action,"capture")) {
        if(argc!=4) {error=ERR_ARGUMENT;goto done;}
        error=window_query(1,&original);
        if(!error) error=profile_capture(argv[2],&original,&requested);
        if(!error && !os_signal_value(&m2_cancelled)) error=profile_save(argv[3],&requested);
        if(!error) error=profile_print(&requested);
        goto done;
    }
    preview=!strcmp(action,"preview");
    if((!strcmp(action,"apply") && argc!=4)
        || ((!strcmp(action,"show") || !strcmp(action,"revert")) && argc!=3)
        || (preview && (argc<3 || argc>4 || (argc==4 && strcmp(argv[3],"cancel"))))) {
        error=ERR_ARGUMENT;goto done;
    }
    cancelTest=preview && argc==4;
    error=profile_load(argv[2],&requested);
    if(error) goto done;
    error=profile_print(&requested);
    if(error || !strcmp(action,"show")) goto done;
    error=window_query(1,&original);
    if(!error) error=profile_supported(&original);
    if(error || os_signal_value(&m2_cancelled)) goto done;
    if(!strcmp(action,"apply")) {
        error=profile_capture("undo",&original,&undo);
        if(!error) error=profile_save(argv[3],&undo);
        if(error) goto done;
        error=ui_line("Undo profile created before application; keep it for explicit revert.");
        if(error) goto done;
    }
    if(os_signal_value(&m2_cancelled)) goto done;
    changed=1; /* Before first byte: partial writes must roll back. */
    error=window_set_pair(1,requested.foreground,requested.background);
    if(!error) error=window_colors(1,&actual);
    if(!error && (actual.foreground!=requested.foreground || actual.background!=requested.background)) error=ERR_WRITE;
    if(error) goto done;
    error=ui_colors("APPLIED (verified fg/bg; border unchanged)",&actual);
    if(error) goto done;
    if(preview) {
        error=ui_line("Preview active; automatic revert after 600 timed polls.");
        if(error) goto done;
        for(tick=0;tick<600 && !os_signal_value(&m2_cancelled);++tick) {
            if(tick==120 && cancelTest) {error=os_cancel_self();if(error)goto done;}
            error=os_sleep(1); if(error) goto done;
        }
    } else {
        error=ui_line("Keeping verified colors. Use 'wmview revert /device/undo' to revert.");
        if(!error && !os_signal_value(&m2_cancelled)) commit=1;
    }
done:
    signal=os_signal_value(&m2_cancelled);
    if(changed && (!commit || error || signal)) {
        restoreError=window_restore_pair(1,&original.colors);
        if(!restoreError) {
            Byte outputError=ui_line("REVERTED (verified foreground/background; border untouched)");
            if(!error) error=outputError;
        } else ui_result("RESTORE FAILED",restoreError);
    }
    if(error==ERR_ARGUMENT) ui_line("Invalid arguments/profile: v1 syntax, version, colors, name or mode.");
    if(error==218) ui_line("File exists: refusing overwrite; choose a NEW destination.");
    if(error) ui_result("OS-9/profile error",error);
    if(signal) ui_result("Cancelled by signal",signal);
    if(restoreError) return restoreError;
    return error ? error : signal;
}
