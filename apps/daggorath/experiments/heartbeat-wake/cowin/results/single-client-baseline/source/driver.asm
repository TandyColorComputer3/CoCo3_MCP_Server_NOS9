* DHeartbeat: driver-owned, video-tick single-bit heartbeat.
* Source: daggorath-reference a94326f COMMON:CLK30, PLOOK:INIVUX.
* Lifecycle: verified probes/virq/counter.asm; EOU Technical Reference
* ch2 pp28-30/App F; IOMan FIRQ/IRQPoll and SCF final-path SS.Close.
* PIA: Tandy Technical Reference III, PIA/Sound Table 4; native_beat.c probe.
* Private ABI: claim $90, release $91, rate $92 (X=0..255),
* resume $93, freeze $94. Query $90: A=active, X=rate:remaining,
* Y=enabled:fault, B=latched phase (0 small, 1 large). Zero rate/count
* byte means 256 decrements. COMMON:CLK30 changes HEARTS on this edge.
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
diagmagic rmb 6
diagvirq rmb 2
diagedges rmb 2
diagpb1before rmb 1
diagpb1after rmb 1
diagphasebefore rmb 1
diagphaseafter rmb 1
diagwaitbefore rmb 1
diagwaitafter rmb 1
diagowner rmb 1
diagpid rmb 1
diagarmu rmb 2
diagticku rmb 2
diagsendreached rmb 1
diagsendcc rmb 1
diagsendb rmb 1
diagsendsuccess rmb 2
diagbuild rmb 1
diagarmcount rmb 2
diagarmpending rmb 2
diagobserved rmb 2
diagsleepreturns rmb 2
diagmaxdelta rmb 1
diagduplicates rmb 2
diagmissed rmb 2
diagphaseerrors rmb 2
diaglastobserved rmb 2
diaglastphase rmb 1
diagedgetick rmb 2
diagmaxlatency rmb 1
diagerror rmb 1
diagdone rmb 1
requestpending rmb 1
diagkickcalls rmb 2
diagkickaccepted rmb 2
diagkickwakeattempts rmb 2
diagkickwakesuccess rmb 2
diagkicknosignal rmb 2
diagclaims rmb 2
diagclaimempty rmb 2
diagkickcollision rmb 2
diagkickerrors rmb 2
diaglastkicktick rmb 2
diagsametick rmb 2
diaglastkickerror rmb 1
memsize equ .
 mod endmod,name,Drivr+Objct,ReEnt,entry,memsize
 fcb UPDAT.
name fcs /DArm63/
 fcb 1
entry lbra init
 lbra read
 lbra write
 lbra getstat
 lbra setstat
 lbra term
poll fcb 0,1,10
init clrb
 lda #$d6
 sta diagmagic,u
 lda #$b7
 sta diagmagic+1,u
 lda #$05
 sta diagmagic+2,u
 lda #$64
 sta diagmagic+3,u
 lda #$a5
 sta diagmagic+4,u
 lda #$5a
 sta diagmagic+5,u
 lda #$64
 sta diagbuild,u
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
snapshot ldy PD.RGS,y
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
getticks pshs y
 ldy D.Proc
 ldb P$ID,y
 puls y
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
 pshs y
 ldy D.Proc
 ldb P$ID,y
 puls y
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
 lbeq finish
 cmpa #$9b
 lbeq claimrequest
 cmpa #$9c
 lbeq sleepreturned
 lbra bad
freeze
 pshs cc
 orcc #IntMasks
 clr enabled,u
 puls cc
 clrb
 rts
armwait pshs cc,d,x,y
 orcc #IntMasks
 stu diagarmu,u
 ldd diagarmcount,u
 addd #1
 std diagarmcount,u
 ldy PD.RGS,y
 ldd edgegen,u
 bne armpending
 ldd edgegen+2,u
 cmpd R$X,y
 bne armpending
 tst requestpending,u
 bne armpendingrequest
 lda owner,u
 sta wakepid,u
 sta diagowner,u
 sta diagpid,u
 lda #1
 sta waitarmed,u
 ldd #0
 std R$X,y
 puls cc,d,x,y
 clrb
 rts
armpendingrequest ldd diagarmpending,u
 addd #1
 std diagarmpending,u
 clr waitarmed,u
 ldd #2
 std R$X,y
 puls cc,d,x,y
 clrb
 rts
armpending ldd diagarmpending,u
 addd #1
 std diagarmpending,u
 clr waitarmed,u
 ldd #1
 std R$X,y
 puls cc,d,x,y
 clrb
 rts
claimrequest pshs cc,d,x,y
 orcc #IntMasks
 ldy PD.RGS,y
 tst requestpending,u
 beq claimempty
 clr requestpending,u
 ldd diagclaims,u
 addd #1
 std diagclaims,u
 ldd #1
 std R$X,y
 puls cc,d,x,y
 clrb
 rts
claimempty ldd diagclaimempty,u
 addd #1
 std diagclaimempty,u
 ldd #0
 std R$X,y
 puls cc,d,x,y
 clrb
 rts
sleepreturned ldd diagsleepreturns,u
 addd #1
 std diagsleepreturns,u
 clrb
 rts
* KICK is allowed from a non-owner path. Event state is authoritative; S$Wake
* is emitted only by the operation that atomically clears waitarmed.
kick pshs cc,d,x,y
 orcc #IntMasks
 tst active,u
 lbeq kickbad
 ldd diagkickcalls,u
 addd #1
 std diagkickcalls,u
 tst requestpending,u
 bne kickcollision
 lda #1
 sta requestpending,u
 ldd diagkickaccepted,u
 addd #1
 std diagkickaccepted,u
 ldd videoticks,u
 std diaglastkicktick,u
 cmpd diagedgetick,u
 bne kickarmed
 ldd diagsametick,u
 addd #1
 std diagsametick,u
kickarmed tst waitarmed,u
 beq kicknosignal
 clr waitarmed,u
 ldd diagkickwakeattempts,u
 addd #1
 std diagkickwakeattempts,u
 lda owner,u
 ldb #S$Wake
 os9 F$Send
 bcs kicksenderror
 ldd diagkickwakesuccess,u
 addd #1
 std diagkickwakesuccess,u
 puls cc,d,x,y
 clrb
 rts
kicknosignal ldd diagkicknosignal,u
 addd #1
 std diagkicknosignal,u
 puls cc,d,x,y
 clrb
 rts
kicksenderror stb diaglastkickerror,u
 ldd diagkickerrors,u
 addd #1
 std diagkickerrors,u
 puls cc,d,x,y
 comb
 ldb diaglastkickerror,u
 rts
kickcollision ldd diagkickcollision,u
 addd #1
 std diagkickcollision,u
kickbad puls cc,d,x,y
 comb
 ldb #188
 rts
observe pshs cc,d,x,y
 orcc #IntMasks
 ldy PD.RGS,y
 ldd diagobserved,u
 addd #1
 std diagobserved,u
 ldd R$X,y
 pshs d
 subd diaglastobserved,u
 beq observedduplicate
 cmpd #1
 bhi observedmissed
 cmpb diagmaxdelta,u
 bls observedphase
 stb diagmaxdelta,u
 bra observedphase
observedduplicate ldd diagduplicates,u
 addd #1
 std diagduplicates,u
 lda #1
 sta diagerror,u
 bra observedphase
observedmissed pshs d
 ldd diagmissed,u
 addd #1
 std diagmissed,u
 puls d
 cmpb diagmaxdelta,u
 bls missedstored
 stb diagmaxdelta,u
missedstored lda #2
 sta diagerror,u
observedphase lda phase,u
 cmpa diaglastphase,u
 bne phaseok
 ldd diagphaseerrors,u
 addd #1
 std diagphaseerrors,u
 lda #3
 sta diagerror,u
phaseok sta diaglastphase,u
 puls d
 std diaglastobserved,u
 ldd videoticks,u
 subd diagedgetick,u
 cmpb diagmaxlatency,u
 bls observedout
 stb diagmaxlatency,u
observedout puls cc,d,x,y
 clrb
 rts
finish pshs cc
 orcc #IntMasks
 clr enabled,u
 lda #1
 sta diagdone,u
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
 pshs y
 ldy D.Proc
 ldb P$ID,y
 puls y
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
 pshs y
 ldy D.Proc
 ldb P$ID,y
 puls y
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
 clr waitarmed,u
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
tick pshs cc,d,x,y
 orcc #IntMasks
 ldd diagvirq,u
 addd #1
 std diagvirq,u
 stu diagticku,u
 lda packet+Vi.Stat,u
 anda #$fe
 sta packet+Vi.Stat,u
 ldd videoticks,u
 addd #1
 std videoticks,u
 tst enabled,u
 lbeq tickdone
 dec remaining,u
 lbne tickdone
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
 lbra tickdone
edge ldb $ff22
 stb diagpb1before,u
 lda phase,u
 sta diagphasebefore,u
 eorb #2
 stb $ff22
 stb diagpb1after,u
 lda phase,u
 eora #1
 sta phase,u
 sta diagphaseafter,u
 ldd edgegen+2,u
 addd #1
 std edgegen+2,u
 bne edgecountdone
 ldd edgegen,u
 addd #1
 std edgegen,u
edgecountdone
 ldd videoticks,u
 std diagedgetick,u
 cmpd diaglastkicktick,u
 bne edgedifferenttick
 ldd diagsametick,u
 addd #1
 std diagsametick,u
edgedifferenttick
 ldd diagedges,u
 addd #1
 std diagedges,u
 lda waitarmed,u
 sta diagwaitbefore,u
 beq nowake
 clr waitarmed,u
 inc diagsendreached,u
 lda wakepid,u
 sta diagpid,u
 ldb #S$Wake
 os9 F$Send
 pshs cc
 puls a
 sta diagsendcc,u
 stb diagsendb,u
 bcs nowake
 ldd diagsendsuccess,u
 addd #1
 std diagsendsuccess,u
nowake lda waitarmed,u
 sta diagwaitafter,u
 lda rate,u
 sta remaining,u
tickdone puls cc,d,x,y
 andcc #$fe
 rts
 emod
endmod equ *
 end
