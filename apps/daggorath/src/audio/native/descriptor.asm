* Native heartbeat pseudo-device; descriptor layout: upstream Nil/VRN.
 use defsfile
 mod endmod,name,Devic+Objct,ReEnt,manager,driver
 fcb UPDAT.
 fcb HW.Page
 fdb 0
 fcb 1
 fcb DT.SCF
manager fcs /SCF/
driver fcs /DHeartbeat/
name fcs /dhb/
 fcb 1
 emod
endmod equ *
 end
