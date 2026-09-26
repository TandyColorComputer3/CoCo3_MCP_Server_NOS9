* Minimal read-only staging probe. No file writes or hardware access.
* Calling pattern: nitros9-reference level1/cmds/echo.asm (start/Exit),
* f470fa52eb172b59b22c1b722074998cb42de9b1; constants from defs/os9.d.
        use os9.d
        mod endmod,name,Prgrm+Objct,ReEnt+1,start,256
name    fcs /stgprobe/
        fcb 1
start   leax message,pcr
        ldy #messageEnd-message
        lda #1
        os9 I$WritLn
        bcs exit
        clrb
exit    os9 F$Exit
message fcc /MCP STAGING OK/
        fcb 13
messageEnd equ *
        emod
endmod  equ *
        end
