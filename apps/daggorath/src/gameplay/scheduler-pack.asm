        nam     dodsched
edition set     1
        mod     endmod,name,$21,$80,entry,0
name    fcs     /dodsched/
        fcb     edition
entry   includebin scheduler-code.bin
        emod
endmod  equ     *
        end
