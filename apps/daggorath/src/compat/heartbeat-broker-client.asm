        section code
        export  _dag_broker_start,_dag_broker_rate,_dag_broker_snapshot
        export  _dag_broker_stats,_dag_broker_trace
        export  _dag_broker_scene,_dag_broker_close,_dag_broker_cancel
        import  _stkcheck

* DagHeartbeatBroker offsets (CMOC packs this byte/long/word structure).
b_cmd   equ     0
b_reply equ     1
b_pid   equ     2
b_open  equ     3
b_seq   equ     4
b_phase equ     5
b_fault equ     6
b_gen   equ     7
b_heart equ     11
b_scene equ     13
b_stale equ     15
b_maxwake equ   17
b_maxheart equ  18
b_maxscene equ  19
b_maxlat equ    20
b_deadline equ  21
b_collapsed equ 22
b_duplicate equ 23
b_tracegen equ  24
b_tracetick equ 26
b_tracepb1 equ  28
b_tracephase equ 29
b_tracewake equ 30
b_traceheart equ 31
b_tracelat equ  32
b_tracescene equ 33
b_tracepass equ 34
b_traceempty equ 35
b_kick  equ     36
b_size  equ     37

magic   equ     $b6
version equ     1
f_link  equ     $03
f_wait  equ     $04
f_send  equ     $08
f_sprior equ    $0d
i_open  equ     $84
i_dup   equ     $82
i_read  equ     $89
i_write equ     $8a
i_setstt equ    $8e
i_close equ     $8f
err_arg equ     187
err_write equ   245

* A=path, X=buffer. Transfer one complete fixed broker frame. PipeMan may
* return a short count; zero progress or a count beyond the remainder is an
* invalid frame transfer. Caller Y and U are CMOC resident context.
write16 pshs    y,u
        pshs    a
        tfr     x,u
        ldy     #16
w16loop pshs    y
        lda     2,s
        tfr     u,x
        ldy     ,s
        os9     i_write
        bcs     ioerr
        tfr     y,d
        beq     ioshort
        cmpd    ,s
        bhi     ioshort
        leau    d,u
        pshs    y
        ldd     2,s
        subd    ,s
        leas    4,s
        tfr     d,y
        bne     w16loop
        clrb
        bra     ioout

read16  pshs    y,u
        pshs    a
        tfr     x,u
        ldy     #16
r16loop pshs    y
        lda     2,s
        tfr     u,x
        ldy     ,s
        os9     i_read
        bcs     ioerr
        tfr     y,d
        beq     ioshort
        cmpd    ,s
        bhi     ioshort
        leau    d,u
        pshs    y
        ldd     2,s
        subd    ,s
        leas    4,s
        tfr     d,y
        bne     r16loop
        clrb
        bra     ioout

ioerr   leas    2,s
        bra     ioout
ioshort leas    2,s
        ldb     #err_write
ioout   puls    a
        puls    u,y,pc

* X=broker, A=operation, B=argument, Y=flags. Returns A=reply status,
* B=OS/application error. One outstanding request makes the named mapped
* scene buffer immutable until this routine receives its acknowledgment.
exchange
        pshs    u,y,x,d
        leas    -32,s
        leau    ,s
        clr     ,u
        ldd     #0
        std     1,u
        std     3,u
        std     5,u
        std     7,u
        std     9,u
        std     11,u
        std     13,u
        clr     15,u
        ldx     34,s                    * saved broker
        tst     b_open,x
        lbeq    exarg
        ldd     #magic*256+version
        std     ,u
        ldd     32,s                    * saved op/argument
        std     2,u
        inc     b_seq,x
        ldb     b_seq,x
        stb     3,u
        ldb     33,s                    * argument has its own field; byte 3 is sequence
        stb     5,u
        ldd     36,s                    * saved flags
        stb     4,u
        ldb     b_gen,x
        stb     6,u
        ldb     b_gen+1,x
        stb     7,u
        ldb     b_gen+2,x
        stb     8,u
        ldb     b_gen+3,x
        stb     9,u
        ldb     b_phase,x
        stb     10,u
        lda     b_cmd,x
        leax    ,u
        lbsr    write16
        tstb
        lbne    exdone
        ldx     34,s
        lda     b_kick,x
        cmpa    #$ff
        lbeq    exarg
        ldb     #$9a
        ldx     #0
        ldy     #0
        os9     i_setstt
        lbcs    exdone
        ldx     34,s
        lda     b_reply,x
        leax    16,u
        lbsr    read16
        tstb
        lbne    exdone
        ldx     34,s
        ldd     16,u
        cmpd    #magic*256+version
        lbne    exarg
        ldb     18,u
        cmpb    32,s                    * operation
        lbne    exarg
        ldb     19,u
        cmpb    b_seq,x
        lbne    exarg
        ldb     20,u
        stb     b_fault,x
        ldb     32,s                    * operation
        cmpb    #9
        beq     extrace
        ldd     22,u
        std     b_gen,x
        ldd     24,u
        std     b_gen+2,x
        ldb     26,u
        stb     b_phase,x
        ldb     32,s
        cmpb    #8
        beq     exstats
        ldd     27,u
        std     b_heart,x
        ldd     29,u
        std     b_scene,x
        clra
        ldb     31,u
        std     b_stale,x
        bra     exparsed
exstats ldb     27,u
        stb     b_maxwake,x
        ldb     28,u
        stb     b_maxheart,x
        ldb     29,u
        stb     b_maxscene,x
        ldb     30,u
        stb     b_maxlat,x
        ldb     31,u
        stb     b_deadline,x
        ldb     21,u
        stb     b_collapsed,x           * nonzero means a sequence fault
        clr     b_duplicate,x
        bra     exparsed
extrace ldd     22,u
        std     b_tracegen,x
        ldd     24,u
        std     b_tracetick,x
        ldb     26,u
        andb    #1
        stb     b_tracepb1,x
        ldb     26,u
        lsrb
        andb    #1
        stb     b_tracephase,x
        ldb     27,u
        stb     b_tracewake,x
        ldb     28,u
        stb     b_traceheart,x
        ldb     29,u
        stb     b_tracelat,x
        ldb     30,u
        stb     b_tracescene,x
        ldb     26,u
        lsrb
        lsrb
        andb    #1
        stb     b_tracepass,x
        ldb     21,u
        stb     b_traceempty,x
exparsed
        lda     21,u                    * status
        ldb     b_fault,x
        bra     exout
exshort ldb     #err_write
        bra     exout
exarg   ldb     #err_arg
        bra     exout
exdone  nop
exout   leas    32,s
        leas    6,s                     * saved op/X/flags; retain saved U
        puls    u,pc

_dag_broker_rate
        pshs    y,u
        lbsr    _stkcheck
        fdb     -74
        leau    ,s
        ldx     6,u
        lda     #2
        ldb     9,u
        ldy     #0
        lbsr    exchange
        puls    y,u,pc

_dag_broker_snapshot
        pshs    y,u
        lbsr    _stkcheck
        fdb     -74
        leau    ,s
        ldx     6,u
        ldd     #$0600
        ldy     #0
        lbsr    exchange
        puls    y,u,pc

_dag_broker_stats
        pshs    y,u
        lbsr    _stkcheck
        fdb     -74
        leau    ,s
        ldx     6,u
        lda     #8
        clrb
        ldy     #0
        lbsr    exchange
        puls    y,u,pc

_dag_broker_trace
        pshs    y,u
        lbsr    _stkcheck
        fdb     -74
        leau    ,s
        ldx     6,u
        lda     #9
        clrb
        ldy     #0
        lbsr    exchange
        puls    y,u,pc

_dag_broker_scene
        pshs    y,u
        lbsr    _stkcheck
        fdb     -76
        leau    ,s
        ldx     6,u
        lda     #5
        ldb     9,u
        ldy     #0
        tst     11,u
        beq     scgo
        leay    1,y
scgo    lbsr    exchange
        pshs    b
        ldx     12,u
        beq     scnostore
        ldb     #0
        cmpa    #1
        bne     scstore
        incb
scstore stb     ,x
scnostore
        puls    b
        puls    y,u,pc

* Close any path number in A. Returns B=0 or the OS-9 error.
closea  os9     i_close
        bcs     clret
        clrb
clret   rts

* Restore standard path B from saved path A. The saved path is closed.
restore pshs    a,b
        tfr     b,a
        lbsr    closea
        bcs     rsbad
        lda     ,s
        os9     i_dup
        bcs     rsbad
        cmpa    1,s
        bne     rsarg
        lda     ,s
        lbsr    closea
        bra     rsout
rsarg   ldb     #err_arg
        bra     rsout
rsbad   nop
rsout   leas    2,s
        rts

* Install source A onto standard path B, saving standard in byte at X.
install pshs    u
        tfr     x,u
        pshs    a,b
        lda     1,s
        os9     i_dup
        bcs     insout
        sta     ,u
        lda     1,s
        lbsr    closea
        bcs     insout
        lda     ,s
        os9     i_dup
        bcs     insout
        cmpa    1,s
        beq     insok
        ldb     #err_arg
        bra     insout
insok   clrb
insout  leas    2,s
        puls    u,pc

clearb  ldb     #b_size
cloop   clr     ,x+
        decb
        bne     cloop
        rts

_dag_broker_start
        pshs    y,u
        lbsr    _stkcheck
        fdb     -114
        leau    ,s
        leas    -20,s                   * s0,s1,s2,pid,error + reply[16]
        ldx     6,u
        lbsr    clearb
        ldx     6,u
        ldd     #$ffff
        std     b_cmd,x
        stb     b_kick,x
        ldb     #$ff
        stb     -20,u
        stb     -19,u
        stb     -18,u
        leax    pipe,pcr
        lda     #3
        os9     i_open
        lbcs    stfail
        ldx     6,u
        sta     b_cmd,x
        leax    pipe,pcr
        lda     #3
        os9     i_open
        lbcs    stfail
        ldx     6,u
        sta     b_reply,x
        leax    heartbeat,pcr
        lda     #3
        os9     i_open
        lbcs    stfail
        ldx     6,u
        sta     b_kick,x
        lda     b_cmd,x
        ldb     #0
        leax    -20,u
        lbsr    install
        lbcs    strestore
        ldx     6,u
        lda     b_reply,x
        ldb     #1
        leax    -19,u
        lbsr    install
        bcs     strestore
        lda     9,u
        ldb     #2
        leax    -18,u
        lbsr    install
        bcs     strestore
        pshs    u
        ldx     10,u
        ldy     #0
        ldu     #0
        lda     #$11
        clrb
        os9     f_link
        puls    u
        bcs     strestore
        sta     -17,u
strestore
        pshs    b                       * preserve first error/carry B
        lda     -18,u
        cmpa    #$ff
        beq     str1
        ldb     #2
        lbsr    restore
str1    lda     -19,u
        cmpa    #$ff
        beq     str0
        ldb     #1
        lbsr    restore
str0    lda     -20,u
        cmpa    #$ff
        beq     strdone
        ldb     #0
        lbsr    restore
strdone puls    b
        tstb
        lbne    stfail
        ldx     6,u
        lda     -17,u
        sta     b_pid,x
        ldb     #192
        os9     f_sprior
        bcs     stchild
        lda     b_reply,x
        leax    -16,u
        lbsr    read16
        tstb
        bne     stchild
        ldx     6,u
        ldd     -16,u
        cmpd    #magic*256+version
        bne     starg
        ldb     -14,u
        cmpb    #1
        bne     starg
        ldb     -12,u
        stb     b_fault,x
        bne     stchild
        ldd     -10,u
        std     b_gen,x
        ldd     -8,u
        std     b_gen+2,x
        ldb     -6,u
        stb     b_phase,x
        inc     b_open,x
        clra
        ldb     13,u
        pshs    b,a
        ldd     6,u
        pshs    b,a
        lbsr    _dag_broker_rate
        leas    4,s
        tstb
        bne     stchild
        ldx     6,u
        lda     #3
        clrb
        ldy     #0
        lbsr    exchange
        bra     stout
stshort ldb     #err_write
        bra     stchild
starg   ldb     #err_arg
stchild pshs    b
        ldx     6,u
        lda     b_pid,x
        ldb     #3
        os9     f_send
        os9     f_wait
        puls    b
stfail  pshs    b
        ldx     6,u
        lda     b_kick,x
        cmpa    #$ff
        beq     stfr
        lbsr    closea
stfr
        ldx     6,u
        lda     b_reply,x
        cmpa    #$ff
        beq     stfc
        lbsr    closea
stfc    ldx     6,u
        lda     b_cmd,x
        cmpa    #$ff
        beq     stclr
        lbsr    closea
stclr   ldx     6,u
        lbsr    clearb
        ldx     6,u
        ldd     #$ffff
        std     b_cmd,x
        puls    b
stout   leas    20,s
        puls    y,u,pc

_dag_broker_close
        pshs    y,u
        lbsr    _stkcheck
        fdb     -78
        leau    ,s
        ldx     6,u
        tst     b_open,x
        beq     csuccess
        lda     #7
        clrb
        ldy     #0
        lbsr    exchange
        tstb
        bne     cout
        os9     f_wait
        bcs     cout
        tstb
        bne     cout
        ldx     6,u
        lda     b_reply,x
        lbsr    closea
        bcs     cout
        ldx     6,u
        lda     b_cmd,x
        lbsr    closea
        bcs     cout
        ldx     6,u
        lda     b_kick,x
        cmpa    #$ff
        beq     cclear
        lbsr    closea
        bcs     cout
cclear
        ldx     6,u
        lbsr    clearb
csuccess clrb
cout    puls    y,u,pc

_dag_broker_cancel
        pshs    y,u
        lbsr    _stkcheck
        fdb     -78
        leau    ,s
        ldx     6,u
        lda     b_pid,x
        beq     csuccess
        ldb     #3
        os9     f_send
        bcs     cout
        os9     f_wait
        bcs     cout
        ldx     6,u
        lda     b_reply,x
        cmpa    #$ff
        beq     cancmd
        lbsr    closea
        bcs     cout
cancmd  ldx     6,u
        lda     b_cmd,x
        cmpa    #$ff
        beq     cankick
        lbsr    closea
        bcs     cout
cankick ldx     6,u
        lda     b_kick,x
        cmpa    #$ff
        beq     canclr
        lbsr    closea
        bcs     cout
canclr  ldx     6,u
        lbsr    clearb
        clrb
        bra     cout

pipe    fcc     "/pipe"
        fcb     0
heartbeat fcc  "/dhb"
        fcb     0
        endsection
