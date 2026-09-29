        section code
        export  _overlay_preload
        export  _overlay_release
        export  _overlay_link
        export  _overlay_call
        export  _overlay_unlink

* F$NMLoad ($22) is deliberately non-mapping.  It retains the physical module
* without occupying dodgame's free DAT slot; command calls use F$Link ($00).
_overlay_preload
        pshs    y,u
* LoadMod, reached through F$NMLoad, opens a file and therefore needs an
* executable pathlist. F$Link/F$UnLoad search the module directory and need
* only the module name.
        leax    dodcmd_path,pcr
        os9     $22
        bcs     preload_done
        clrb
preload_done
        clra
        puls    y,u,pc

* Level II F$UnLoad ($1d), by name, releases the retained non-mapping link.
_overlay_release
        pshs    y,u
        leax    dodcmd_name,pcr
        lda     #$21
        os9     $1d
        bcs     release_done
        clrb
release_done
        clra
        puls    y,u,pc

* OverlayLink: header word at 0, entry word at 2.
_overlay_link
        pshs    y,u
        leax    dodcmd_name,pcr
        lda     #$21
        os9     $00
        bcs     link_done
        ldx     6,s
        stu     ,x
        sty     2,x
        clrb
link_done
        clra
        puls    y,u,pc

_overlay_call
        pshs    y,u
        ldu     6,s
        ldy     2,u
        ldx     8,s
        lda     #1
        ldb     16,x
        jsr     ,y
        clra
        puls    y,u,pc

_overlay_unlink
        pshs    y,u
        ldx     6,s
        ldu     ,x
        os9     $02
        bcs     unlink_done
        clrb
unlink_done
        clra
        puls    y,u,pc

dodcmd_name
        fcs     /dodcmd/
dodcmd_path
* Production artifacts are staged on the disposable application floppy, which
* the verified game lifecycle mounts as /d1 before dodgame starts.
        fcs     "/d1/dodcmd"
        endsection
