        section code
        export _scheduler_callback_init
        export _scheduler_task_gateway
        export _scheduler_present_gateway
        export _scheduler_progress_gateway
_scheduler_task_resident import
_scheduler_present_resident import
_scheduler_progress_resident import

* Save CMOC's resident data base (Y) with the caller-owned service context.
_scheduler_callback_init
        ldx     2,s
        tfr     y,d
        std     ,x
        ldd     4,s
        std     2,x
        rts

* The mapped module's JSR leaves its return address and C arguments intact.
* Replacing Y then tail-jumping lets the stack-checked resident C routine see
* its normal frame and return directly to dodsched. scheduler.c has no Y data
* references after a service call; its state is reached through context X/U.
_scheduler_task_gateway
        ldx     2,s
        ldy     ,x
* The resident executable can be mapped at any Level II logical base.  An
* absolute JMP to an imported symbol is only its link-relative value (and
* previously landed in process data at $06EB when doddemo was at $A000).
* PC-relative LEAX preserves the inter-symbol displacement through module
* relocation, then the indirect jump enters the actual resident mapping.
        leax    _scheduler_task_resident,pcr
        jmp     ,x

_scheduler_present_gateway
        ldx     2,s
        ldy     ,x
        ldd     2,x
        std     2,s
        leax    _scheduler_present_resident,pcr
        jmp     ,x

_scheduler_progress_gateway
        ldx     2,s
        ldy     ,x
        ldd     2,x
        std     2,s
        leax    _scheduler_progress_resident,pcr
        jmp     ,x
        endsection
