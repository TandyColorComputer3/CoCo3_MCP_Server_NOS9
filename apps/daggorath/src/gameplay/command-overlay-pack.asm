        nam     dodcmd
edition set     1
        mod     endmod,name,$21,$80,entry,0
name    fcs     /dodcmd/
        fcb     edition
entry   includebin command-overlay-code.bin
        emod
endmod  equ     *
        end
