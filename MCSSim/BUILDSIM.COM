$! GF - Start of CIM System custom modifications
$!-------------------------------------------------------------
$ cc:==cc/deb/noopt/DEFINE=(DEBUG)
$ cc VMSSIM.c  + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled VMSSIM.c"
$ cc LOADLOTLIST.C + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled LOADLOTLIST.c" 
$ link/exe=VMSSIM.exe VMSSIM.obj,LOADLOTLIST.obj,-
ssp_cimlib:cimlib/lib,dmq$lib:dmq/opt
$ write sys$output "done with linking"
$ del *.obj;*
$ purge *.*
$copy VMSSIM.EXE; PROMPRI1:[FAB3PRI.SSP.F235MCS.MCSSIM]VMSSIM.EXE;/log
$!--------------------------------------------------------------
$! GF - End of CIM System custom modifications
