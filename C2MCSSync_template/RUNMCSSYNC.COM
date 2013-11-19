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
$  MCSSYNC :== $F2MCSSYNC:MCSSYNC
$  MCSSYNC -delay 60 -qname MCS -qdelay 200 -verbose 1 -retry 3
$!
$!----------------------------------------------------------------------------
$! GF - End of CIM System custom modifications
