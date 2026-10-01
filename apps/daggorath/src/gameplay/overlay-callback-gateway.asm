        section code
        export _overlay_callback_init
        export _overlay_health_gateway
        export _overlay_object_name_gateway
        export _overlay_render_status_gateway
_overlay_health_resident import
_overlay_object_name_resident import
_overlay_render_status_resident import

* Save the resident CMOC data base before F$Link maps dodcmd and its Y value.
_overlay_callback_init
        ldx     2,s
        tfr     y,d
        std     ,x
        rts

* The first service argument is the caller-owned context.  Restore its
* resident Y, then use PC-relative target construction: module-relative
* imported addresses are not CPU addresses under Level II mapping.
_overlay_health_gateway
        ldx     2,s
        ldy     ,x
        leax    _overlay_health_resident,pcr
        jmp     ,x

_overlay_object_name_gateway
        ldx     2,s
        ldy     ,x
        leax    _overlay_object_name_resident,pcr
        jmp     ,x

_overlay_render_status_gateway
        ldx     2,s
        ldy     ,x
        leax    _overlay_render_status_resident,pcr
        jmp     ,x
        endsection
