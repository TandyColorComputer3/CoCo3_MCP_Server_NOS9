* Disposable counter-only VIRQ driver. No hardware I/O.
* EOU Technical Reference ch2 pp28-30, App F: IRQ before VIRQ;
* remove VIRQ before IRQ. EOU VRN VInit/IRQSvc, IOMan FIRQ/IRQPoll,
* SCF SS.Close are provenance for the API pattern, not copied driver code.
* User ABI (private probe): GetStat $90 => X=count,Y=static,A=active;
* SetStat $90=start, $91=stop, $92=one-shot stop failure, $93=Term delay.
 use defsfile
 org V.SCF
packet rmb 5
counter rmb 2
active rmb 1
failnext rmb 1
failterm rmb 1
owner rmb 1
memsize equ .
 mod endmod,name,Drivr+Objct,ReEnt,entry,memsize
 fcb UPDAT.
name fcs /VCounter/
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
 bne bad
 ldy PD.RGS,y
 pshs cc
 orcc #IntMasks
 ldd counter,u
 std R$X,y
 stu R$Y,y
 lda active,u
 sta R$A,y
 puls cc
 clrb
 rts
setstat cmpa #SS.Close
 beq closing
 cmpa #SS.Open
 beq write
 cmpa #SS.ComSt
 beq write
 cmpa #$90
 beq start
 tst active,u
 beq ownok
 ldb PD.PD,y
 cmpb owner,u
 bne busy
ownok cmpa #$91
 beq stop
 cmpa #$93
 beq injectterm
 cmpa #$92
 bne bad
 lda #1
 sta failnext,u
 clrb
 rts
injectterm lda #1
 sta failterm,u
 clrb
 rts
closing ldb PD.PD,y
 cmpb owner,u
 beq term
 clrb
 rts
bad comb
 ldb #E$UnkSvc
 rts
start tst active,u
 bne busy
 ldb PD.PD,y
 stb owner,u
 leax poll,pcr
 leay tick,pcr
 pshs u
 leau packet+Vi.Stat,u
 tfr u,d
 puls u
 os9 F$IRQ
 bcs done
 lda #$80
 sta packet+Vi.Stat,u
 ldd #1
 std packet+Vi.Rst,u
 leay packet,u
 ldx #1
 os9 F$VIRQ
 bcc started
 pshs b,cc
 ldx #0
 os9 F$IRQ
 puls cc,b,pc
started lda #1
 sta active,u
 clrb
done rts
busy comb
 ldb #E$DevBsy
 rts
stop tst failnext,u
 beq term
 clr failnext,u
 comb
 ldb #187
 rts
* Explicit-stop injection never bypasses Term. Its separate diagnostic delays
* one removal attempt, exercising the same retain/yield path as an OS error.
* Inspected EOU F.VIRQ and FIRQ removal are idempotent/no-error.
* No early return between deregistrations: IOMan does not honor Term failure.
term tst failterm,u
 beq remove
 clr failterm,u
 bra retain
remove leay packet,u
 ldx #0
 os9 F$VIRQ
 bcs retain
 ldx #0
 os9 F$IRQ
 bcs retain
 clr active,u
 clr owner,u
 clrb
 rts
* Never return Term with a failed removal: retain its call/storage, yield, retry.
* This is driver process context, NEVER the callback.
retain pshs x
 ldx #60
 os9 F$Sleep
 puls x
 bra term
* IOMan loads U=system static and calls through Q$SERV.
* Preserve scratch D; U/DP/mappings untouched, acknowledge before returning.
tick pshs d
 lda packet+Vi.Stat,u
 anda #$fe
 sta packet+Vi.Stat,u
 ldd counter,u
 addd #1
 std counter,u
 puls d
 andcc #$fe
 rts
 emod
endmod equ *
 end
