$! GF - Start of CIM System custom modifications
$!----------------------------------------------------------------------------
$! RunMCSSYNC.Com - Run the GF MCS Intergration
$!
$  set message log:messages
$  @ttc:attach.com
$  on error then exit
$  RETURN_TO_VAX := "EXIT"
$  BYE := "EXIT"
$  RESTART := "GOTO LOOP"
$  loop:
$!  MCSINTSRV :== $MCSINT:mcsintsrv
$  define/nolog/group MCSINT PROMPRI1:[FAB3PRI.SSP.F235MCS.MCSINT]
$  MCSINTSRV :== $PROMPRI1:[FAB3PRI.SSP.F235MCS.MCSINT]mcsintsrv.exe
$  MCSINTSRV 9 8002
$!
$!----------------------------------------------------------------------------
$! GF - End of CIM System custom modifications
