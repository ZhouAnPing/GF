$! GF - Start of CIM System custom modifications
$!-------------------------------------------------------------
$ cc:==cc/deb/noopt/DEFINE=(DEBUG)
$ cc testsend.c  + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled testsend.c" 
$ link/exe=testsend.exe testsend.obj,-
ssp_cimlib:cimlib/lib,dmq$lib:dmq/opt
$ write sys$output "done with linking"
$ del *.obj;*
$ purge *.*
$copy testsend.EXE; PROMPRI1:[FAB3PRI.SSP.F235MCS.MCSSIM]testsend.EXE;/log
$!--------------------------------------------------------------
$! GF - End of CIM System custom modifications
