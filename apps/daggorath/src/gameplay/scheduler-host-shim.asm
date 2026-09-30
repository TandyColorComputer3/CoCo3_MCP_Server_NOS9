        section code
        export  _scheduler_preload
        export  _scheduler_release
        export  _scheduler_link
        export  _scheduler_call
        export  _scheduler_unlink
_scheduler_preload
        pshs    y,u
        leax    dodsched_path,pcr
        os9     $22
        bcs     preload_done
        clrb
preload_done
        clra
        puls    y,u,pc
_scheduler_release
        pshs    y,u
        leax    dodsched_name,pcr
        lda     #$21
        os9     $1d
        bcs     release_done
        clrb
release_done
        clra
        puls    y,u,pc
_scheduler_link
        pshs    y,u
        leax    dodsched_name,pcr
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
_scheduler_call
        pshs    y,u
        ldu     6,s
        ldy     2,u
        ldx     8,s
        lda     #1
        ldb     2,x
        jsr     ,y
        clra
        puls    y,u,pc
_scheduler_unlink
        pshs    y,u
        ldx     6,s
        ldu     ,x
        os9     $02
        bcs     unlink_done
        clrb
unlink_done
        clra
        puls    y,u,pc
dodsched_name
        fcs     /dodsched/
dodsched_path
        fcs     "/d1/dodsched"
        endsection
