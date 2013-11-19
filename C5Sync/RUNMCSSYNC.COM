$! GF - Start of CIM System custom modifications
$!----------------------------------------------------------------------------
$! RunC5SYNC.Com - Run the GF MCS Intergration
$!
$  set message log:messages
$  @ttc:attach.com
$  on error then exit
$  RETURN_TO_VAX := "EXIT"
$  BYE := "EXIT"
$  RESTART := "GOTO LOOP"
$  define/nolog/group C5MCSSYNC PROMPRI1:[FAB3PRI.SSP.F235MCS.C5SYNC]
$  loop:
$  C5SYNC :== $PROMPRI1:[FAB3PRI.SSP.F235MCS.C5SYNC]c5syncsrv
$  C5SYNC 9
$!
$!----------------------------------------------------------------------------
$! GF - End of CIM System custom modifications
