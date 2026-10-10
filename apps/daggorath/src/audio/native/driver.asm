* DHeartbeat: driver-owned, video-tick single-bit heartbeat.
* Source: daggorath-reference a94326f COMMON:CLK30, PLOOK:INIVUX.
* Lifecycle: verified probes/virq/counter.asm; EOU Technical Reference
* ch2 pp28-30/App F; IOMan FIRQ/IRQPoll and SCF final-path SS.Close.
* PIA: Tandy Technical Reference III, PIA/Sound Table 4; native_beat.c probe.
* Private ABI: claim $90, release $91, rate $92 (X=0..255),
* resume $93, freeze $94, ARM $97 (expected generation Y:X),
* observe $98 (generation Y:X), DISARM $99, KICK $9A, claim request $9B.
* Query $90: A=active, X=rate:remaining, Y=enabled:fault,
* B=latched phase (0 small, 1 large). Zero rate/count byte means 256
* decrements. COMMON:CLK30 changes HEARTS on this edge. KICK is the only
* non-owner state mutation; generation and request state remain authoritative.
 use defsfile
 org V.SCF
packet rmb 5
active rmb 1
owner rmb 1
pinowned rmb 1
savedbit rmb 1
savedddr rmb 1
rate rmb 1
remaining rmb 1
enabled rmb 1
fault rmb 1
phase rmb 1
videoticks rmb 2
edgegen rmb 4
waitarmed rmb 1
wakepid rmb 1
requestpending rmb 1
memsize equ .
 mod endmod,name,Drivr+Objct,ReEnt,entry,memsize
 fcb UPDAT.
name fcs /DHeartbeat/
 fcb 1
entry lbra init
 lbra read
 lbra write
 lbra getstat
 lbra setstat
 lbra term
poll fcb 0,1,10
init clrb
 rts
read comb
 ldb #E$EOF
 rts
write clrb
 rts
getstat cmpa #$90
 beq query
 cmpa #$95
 lbeq getticks
 cmpa #$96
 lbeq snapshot
 lbra bad
query
 ldy PD.RGS,y
 pshs cc
 orcc #IntMasks
 lda active,u
 sta R$A,y
 lda phase,u
 sta R$B,y
 lda rate,u
 ldb remaining,u
 std R$X,y
 lda enabled,u
 ldb fault,u
 std R$Y,y
 puls cc
 clrb
 rts
* Atomic observation of the existing VIRQ-owned phase and edge counter.
* X=low edge generation, Y=high edge generation, B=latched phase, A=fault.
snapshot lbsr currentpid
 cmpb owner,u
 lbne busy
 ldy PD.RGS,y
 pshs cc
 orcc #IntMasks
 lda fault,u
 sta R$A,y
 lda phase,u
 sta R$B,y
 ldd edgegen,u
 std R$Y,y
 ldd edgegen+2,u
 std R$X,y
 puls cc
 clrb
 rts
* M6: atomically drain elapsed 60 Hz callbacks for simulation scheduling.
* This is a private service operation on the existing VIRQ, not a second
* interrupt hook. Counter bytes are appended after the M2 ABI fields.
getticks lbsr currentpid
 cmpb owner,u
 lbne busy
 ldy PD.RGS,y
 pshs cc
 orcc #IntMasks
 ldd videoticks,u
 std R$X,y
 clra
 clrb
 std videoticks,u
 puls cc
 clrb
 rts
setstat cmpa #SS.Close
 lbeq closing
 cmpa #SS.Open
 lbeq write
 cmpa #SS.ComSt
 lbeq write
 cmpa #$90
 lbeq start
 cmpa #$9a
 lbeq kick
 lbsr currentpid
 cmpb owner,u
 lbne busy
 tst active,u
 lbeq bad
 cmpa #$91
 lbeq term
 cmpa #$92
 lbeq updaterate
 cmpa #$93
 lbeq resume
 cmpa #$94
 beq freeze
 cmpa #$97
 lbeq armwait
 cmpa #$98
 lbeq observe
 cmpa #$99
 lbeq disarm
 cmpa #$9b
 lbeq claimrequest
 lbra bad
freeze
 pshs cc
 orcc #IntMasks
 clr enabled,u
 puls cc
 clrb
 rts
* ARM returns X=0 when armed, X=1 when generation is already pending, and
* X=2 when a committed request is already pending. Expected generation is
* the complete 32-bit Y:X value; notifications never replace that authority.
armwait pshs cc,d,x,y
 orcc #IntMasks
 ldy PD.RGS,y
 ldd edgegen,u
 cmpd R$Y,y
 bne armpending
 ldd edgegen+2,u
 cmpd R$X,y
 bne armpending
 tst requestpending,u
 bne armpendingrequest
 lda owner,u
 sta wakepid,u
 lda #1
 sta waitarmed,u
 ldd #0
 std R$X,y
 puls cc,d,x,y
 clrb
 rts
armpendingrequest
 clr waitarmed,u
 clr wakepid,u
 ldd #2
 std R$X,y
 puls cc,d,x,y
 clrb
 rts
armpending
 clr waitarmed,u
 clr wakepid,u
 ldd #1
 std R$X,y
 puls cc,d,x,y
 clrb
 rts
* Observation is an atomic equality check against the complete generation.
* The driver does not own shown-generation state; the broker remains its
* authority and advances it only after successful visible presentation.
observe pshs cc,d,x,y
 orcc #IntMasks
 ldy PD.RGS,y
 ldd edgegen,u
 cmpd R$Y,y
 bne observebad
 ldd edgegen+2,u
 cmpd R$X,y
 bne observebad
 puls cc,d,x,y
 clrb
 rts
observebad
 puls cc,d,x,y
 comb
 ldb #188
 rts
* DISARM retires notification state but never discards a committed request.
disarm pshs cc
 orcc #IntMasks
 clr waitarmed,u
 clr wakepid,u
 puls cc
 clrb
 rts
* KICK commits one request. A non-owner may call it through its own path, but
* it cannot alter rate, generation, phase, ARM, ownership, or lifecycle state.
kick pshs cc,d,x,y
 orcc #IntMasks
 tst active,u
 beq kickbad
 tst requestpending,u
 bne kickbad
 lda #1
 sta requestpending,u
 tst waitarmed,u
 beq kickdone
 clr waitarmed,u
 lda wakepid,u
 clr wakepid,u
 ldb #S$Wake
 os9 F$Send
 bcs kicksenderror
kickdone
 puls cc,d,x,y
 clrb
 rts
kicksenderror
 stb fault,u
 puls cc,d,x,y
 comb
 ldb fault,u
 rts
kickbad
 puls cc,d,x,y
 comb
 ldb #188
 rts
claimrequest pshs cc,d,x,y
 orcc #IntMasks
 ldy PD.RGS,y
 tst requestpending,u
 beq claimempty
 clr requestpending,u
 ldd #1
 bra claimdone
claimempty
 ldd #0
claimdone
 std R$X,y
 puls cc,d,x,y
 clrb
 rts
updaterate ldy PD.RGS,y
 tst R$X,y
 bne invalid
 ldb R$X+1,y
 pshs cc
 orcc #IntMasks
 stb rate,u
 puls cc
 clrb
 rts
resume tst fault,u
 bne invalid
 pshs cc
 orcc #IntMasks
 lda #1
 sta enabled,u
 puls cc
 clrb
 rts
closing tst active,u
 lbeq write
 lbsr currentpid
 cmpb owner,u
 lbeq term
 clrb
 rts
bad comb
 ldb #E$UnkSvc
 rts
busy comb
 ldb #E$DevBsy
 rts
invalid comb
 ldb #187
 rts
* Installed EOU Level II defs/os9.d D.Proc/P$ID and kernel/fsprior.asm use
* this current-process identity. PD.PD is a path-descriptor field, not a PID.
* All owner checks use the same D.Proc/P$ID convention.
currentpid pshs y
 ldy D.Proc
 ldb P$ID,y
 puls y,pc
start tst active,u
 bne busy
 lbsr currentpid
 stb owner,u
 lbsr acquire
 bcs startfailed
 clr enabled,u
 clr fault,u
 clr phase,u
 clr edgegen,u
 clr edgegen+1,u
 clr edgegen+2,u
 clr edgegen+3,u
 clr waitarmed,u
 clr wakepid,u
 clr requestpending,u
 clr rate,u
 lda #1
 sta remaining,u
 leax poll,pcr
 leay tick,pcr
 pshs u
 leau packet+Vi.Stat,u
 tfr u,d
 puls u
 os9 F$IRQ
 bcs rollback
 lda #$80
 sta packet+Vi.Stat,u
 ldd #1
 std packet+Vi.Rst,u
 leay packet,u
 ldx #1
 os9 F$VIRQ
 bcs rollback
 lda #1
 sta active,u
 clrb
 rts
* Both removal APIs are idempotent on this verified EOU target. Preserve
* original installation failure; never return with a callback still installed.
rollback pshs b,cc
 lbsr term
 puls cc,b,pc
startfailed pshs cc
 clr owner,u
 clr wakepid,u
 clr waitarmed,u
 clr requestpending,u
 puls cc,pc
* Acquisition begins low (COMINI); shutdown restores acquired PB1/DDR bit.
* Never restore an entire stale PIA/video byte or change the sound mux.
acquire pshs cc
 orcc #IntMasks
 lda $ff23
 anda #$3f
 cmpa #$34
 beq bankok
 cmpa #$3c
 bne bankbad
bankok ldb $ff22
 andb #2
 stb savedbit,u
 anda #$fb
 sta $ff23
 ldb $ff22
 andb #2
 stb savedddr,u
 ldb $ff22
 orb #2
 stb $ff22
 ora #4
 sta $ff23
 ldb $ff22
 andb #$fd
 stb $ff22
 lda #1
 sta pinowned,u
 puls cc
 clrb
 rts
bankbad puls cc
 lbra invalid
term pshs cc
 orcc #IntMasks
 clr enabled,u
 clr waitarmed,u
 clr wakepid,u
 clr requestpending,u
 puls cc
remove leay packet,u
 ldx #0
 os9 F$VIRQ
 bcs retain
 ldx #0
 os9 F$IRQ
 bcs retain
 tst pinowned,u
 beq released
 pshs cc
 orcc #IntMasks
 lda $ff23
 ora #4
 sta $ff23
 ldb $ff22
 andb #$fd
 orb savedbit,u
 stb $ff22
 anda #$fb
 sta $ff23
 ldb $ff22
 andb #$fd
 orb savedddr,u
 stb $ff22
 ora #4
 sta $ff23
 clr pinowned,u
 puls cc
released clr active,u
 clr owner,u
 clrb
 rts
* IOMan ignores Term carry before freeing statics. Retain storage and yield
* on a removal error, rather than returning a dangling callback. Process only.
retain pshs x
 ldx #60
 os9 F$Sleep
 puls x
 bra remove
* Callback: fixed system static U. Its only OS call is the accepted advisory
* F$Send after an armed edge; no graphics, IPC framing or dynamic allocation.
* Mask only this bounded register/countdown operation; preserve incoming CC.
* Acknowledge VIRQ before returning carry clear to the polling chain.
tick pshs cc,d,x,y
 orcc #IntMasks
 lda packet+Vi.Stat,u
 anda #$fe
 sta packet+Vi.Stat,u
 ldd videoticks,u
 addd #1
 std videoticks,u
 tst enabled,u
 beq tickdone
 dec remaining,u
 bne tickdone
 lda $ff23
 anda #$3f
 cmpa #$34
 beq edge
 cmpa #$3c
 beq edge
* Refuse a changed PIA bank/interrupt configuration; do not write DDR as data.
 lda #187
 sta fault,u
 clr enabled,u
 bra tickdone
edge ldb $ff22
 eorb #2
 stb $ff22
 lda phase,u
 eora #1
 sta phase,u
 ldd edgegen+2,u
 addd #1
 std edgegen+2,u
 bne edgecountdone
 ldd edgegen,u
 addd #1
 std edgegen,u
edgecountdone
* Accepted edge order: PB1, phase, generation, then advisory wake.
 tst waitarmed,u
 beq nowake
 clr waitarmed,u
 lda wakepid,u
 clr wakepid,u
 ldb #S$Wake
 os9 F$Send
 bcc nowake
 stb fault,u
 clr enabled,u
nowake
 lda rate,u
 sta remaining,u
tickdone puls cc,d,x,y
 andcc #$fe
 rts
 emod
endmod equ *
 end
