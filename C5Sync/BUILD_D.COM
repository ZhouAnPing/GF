$! GF - Start of GF System custom modifications
$!-------------------------------------------------------------
$ cc:==cc/deb/noopt
$ cc primain.c  + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled primain.c"
$ cc pridisp.c  + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled pridisp.c"
$ cc priparse.c + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled priparse.c"
$ cc primethod.c + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled primethod.c"
$ cc pristartup.c + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled pristartup.c"
$ cc priloc.c + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled priloc.c"
$ cc prireply.c + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled prireply.c"
$ link/debug/exe=c5syncsrv.exe primain.obj,pridisp.obj,priparse.obj,-
primethod.obj,priloc.obj,pristartup.obj,prireply.obj,-
ssp_cimlib:cimlib/lib,dmq$lib:dmq/opt,BLD:IMAGE/OPTIONAL
$ write sys$output "done with linking"
$ copy c5syncsrv.exe; PROMPRI1:[FAB3PRI.SSP.F235MCS.C5SYNC]c5syncsrv.exe; /log
$ del *.obj;*
$ purge *.*
$!--------------------------------------------------------------
$! GF - End of GF System custom modifications
