$! CSM - Start of CIM System custom modifications
$!-------------------------------------------------------------
$
$ cc:==cc/deb/noopt/DEFINE=(DEBUG)
$ cc mcsmain.c  + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled mcsmain.c"
$ cc mcsdisp.c  + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled mcsdisp.c"
$ cc mcsparse.c + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled mcsparse.c"
$ cc mcsmethod.c + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled mcsmethod.c"
$ cc mcsloc.c + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled mcsloc.c"
$ cc mcsreply.c + DMQ$USER:DMQ/LIB
$ write sys$output "successfully compiled mcsreply.c"
$! Build Fortran objects...
$ @bld:compile  mcsint.for
$ write sys$output "successfully compiled mcsint.for"
$ @build_hsmslib.com
$ write sys$output "successfully build hsmslib"
$ cxxlink/exe=mcsintsrv.exe mcsint.obj,mcsmain.obj,mcsdisp.obj,mcsparse.obj,-
   mcsmethod.obj,mcsloc.obj,mcsreply.obj,hsmslib/lib,-
    ssp_cimlib:cimlib/lib,prom:promis/lib,dmq$lib:dmq/opt,-
    tim:timwrtshr/opt,wct:wctwrtshr/opt,tps:server/opt,BLD:IMAGE/OPTIONAL
$ write sys$output "done with linking"
$ del *.obj;*
$ purge *.*
$ copy MCSINTSRV.EXE; PROMPRI1:[FAB3PRI.SSP.F235MCS.MCSINT]MCSINTSRV.EXE;/log
$!--------------------------------------------------------------
$! CSM - End of CIM System custom modifications
