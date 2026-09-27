* Private counter pseudo-device; descriptor layout: upstream Nil/VRN.
 use defsfile
 mod endmod,name,Devic+Objct,ReEnt,manager,driver
 fcb UPDAT.
 fcb HW.Page
 fdb 0
 fcb 1
 fcb DT.SCF
manager fcs /SCF/
driver fcs /VCounter/
name fcs /vc/
 fcb 1
 emod
endmod equ *
 end
