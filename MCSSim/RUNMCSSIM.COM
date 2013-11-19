$! GF - Start of CIM System custom modifications
$!----------------------------------------------------------------------------
$! RunMCSSIM.Com - Run the GF MCS Simulator
$!
$  set message log:messages
$  @ttc:attach.com
$  on error then exit
$  RETURN_TO_VAX := "EXIT"
$  BYE := "EXIT"
$  RESTART := "GOTO LOOP"
$  define/nolog/group MCSSIM   PROMPRI1:[PROMF3.SSP.F235MCS.MCSSIM]
$  loop:  
$  MCSSIM :== $MCSSIM:VMSSIM
$  MCSSIM -fMCSSIM.cfg
$!
$!----------------------------------------------------------------------------
$! GF - End of CIM System custom modifications
                   
