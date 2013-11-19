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
$  C2SYNC :== $PROMPRI1:[FAB3PRI.SSP.F235MCS.C2SYNC]MCSSYNC.exe
$!  C2SYNC -delay 60 -qname C5SYNC -qdelay 200 -verbose 1 -retry 3
$  C2SYNC -port 8001 -delay 60 -retry 3
$!
$!----------------------------------------------------------------------------
$! GF - End of CIM System custom modifications
