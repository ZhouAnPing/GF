$! GF - Start of CIM System custom modifications
$!-------------------------------------------------------------
$ cc:==cc/deb/noopt/DEFINE=(DEBUG)
$ cc send_startup.c  + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled send_startup.c" 
$ link/exe=send_startup.exe send_startup.obj,-
ssp_cimlib:cimlib/lib,dmq$lib:dmq/opt
$ write sys$output "done with linking"
$ del *.obj;*
$ purge *.*
$
$!--------------------------------------------------------------
$! GF - End of CIM System custom modifications
