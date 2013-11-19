$! GF - Start of CIM System custom modifications
$!----------------------------------------------------------------------------
$! RunMCSSYNC.Com - Run the GF MCS Client
$!
$  set message log:messages
$  @ttc:attach.com
$  on error then exit
$  RETURN_TO_VAX := "EXIT"
$  BYE := "EXIT"
$  RESTART := "GOTO LOOP"
$  loop:
$  C3SYNC :== $PROMPRI1:[FAB3PRI.SSP.F235MCS.C3SYNC]MCSSYNC.exe
$  C3SYNC -delay 60 -qname MCSSIM -qdelay 200 -verbose 1 -retry 3
$!
$!----------------------------------------------------------------------------
$! GF - End of CIM System custom modifications
