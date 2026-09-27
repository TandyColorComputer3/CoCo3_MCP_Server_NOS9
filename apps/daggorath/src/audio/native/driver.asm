* DHeartbeat: driver-owned, video-tick single-bit heartbeat.
* Source: daggorath-reference a94326f COMMON:CLK30, PLOOK:INIVUX.
* Lifecycle: verified probes/virq/counter.asm; EOU Technical Reference
* ch2 pp28-30/App F; IOMan FIRQ/IRQPoll and SCF final-path SS.Close.
* PIA: Tandy Technical Reference III, PIA/Sound Table 4; native_beat.c probe.
* Private ABI: claim $90, release $91, rate $92 (X=0..255),
* resume $93, freeze $94. Query $90: A=active, X=rate:remaining,
* Y=enabled:fault. Zero rate/count byte means 256 decrements.
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
 lbne bad
 ldy PD.RGS,y
 pshs cc
 orcc #IntMasks
 lda active,u
 sta R$A,y
 lda rate,u
 ldb remaining,u
 std R$X,y
 lda enabled,u
 ldb fault,u
 std R$Y,y
 puls cc
 clrb
 rts
setstat cmpa #SS.Close
 beq closing
 cmpa #SS.Open
 lbeq write
 cmpa #SS.ComSt
 lbeq write
 cmpa #$90
 lbeq start
 ldb PD.PD,y
 cmpb owner,u
 bne busy
 tst active,u
 beq bad
 cmpa #$91
 lbeq term
 cmpa #$92
 beq updaterate
 cmpa #$93
 beq resume
 cmpa #$94
 lbne bad
 pshs cc
 orcc #IntMasks
 clr enabled,u
 puls cc
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
 ldb PD.PD,y
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
start tst active,u
 bne busy
 ldb PD.PD,y
 stb owner,u
 lbsr acquire
 bcs startfailed
 clr enabled,u
 clr fault,u
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
* Callback: fixed system static U, no OS calls, pointers or process state.
* Mask only this bounded register/countdown operation; preserve incoming CC.
* Acknowledge VIRQ before returning carry clear to the polling chain.
tick pshs cc,d
 orcc #IntMasks
 lda packet+Vi.Stat,u
 anda #$fe
 sta packet+Vi.Stat,u
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
 lda rate,u
 sta remaining,u
tickdone puls cc,d
 andcc #$fe
 rts
 emod
endmod equ *
 end
