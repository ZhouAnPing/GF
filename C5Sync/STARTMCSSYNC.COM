$! GF - Start of CIM System custom modifications
$!----------------------------------------------------------------------------
$! STARTC5SYNC.COM - Start the GF C5SYNC client
$! 
$! Exit with error if NOT on PRIMARY
$  where  := 'f$trnlnm("PROMIS_WHERE","LNM$GROUP")
$  if (where .nes. "PRIMARY")
$  then
$    write sys$output " ** on ''where', not PRIMARY, exiting"
$    exit
$  endif
$!
$  on control_y then exit %X10000614
$  on error then exit &$status .or. %X10000000
$!
$  write sys$output "    Starting GF MCS Integration"
$!
$! Get needed privileges
$  srvprivs = "TMPMBX,SYSPRV"
$  @cfc:setpriv "DETACH,ALTPRI,NOACNT,PSWAPM,''srvprivs'"
$!
$! Establish defaults for production system
$  prio = 6
$  wslim = 1024
$  wsquota = 1806
$  wsext = 3072
$  swap_value = "noswap"
$!  
$!  define/nolog/group C5MCSSYNC PROMPRI1:[PROMF3.SSP.F235MCS.C5MCSSYNC]
$!  sho log MCSINT
$  define/user sys$output nl:
$  run sys$system:loginout -
		/Process_name	= C5MCSSYNC -
		/Input		= runMCSSYNC -
		/Output		= C5mcssync.log  -
		/Uic		= 'f$user()' -
		/'swap_value' -
		/NoAuthorize -
		/Buffer_limit	= 40000 -	
		/Io_direct	= 100 -
		/Enqueue_Limit	= 1000 -
		/File_Limit	= 150 -		
		/Page_File	= 80000 -
		/Privilege 	= ('srvprivs') -
		/Priority	= 'prio' -
		/Working	= 'wslim' -		! \
		/Maximum_Working= 'wsquota' -		!  * Working set settings
		/Extent		= 'wsext'		! /
$!
$  set process/priv=('cfc_oldprivs')
$!
$!----------------------------------------------------------------------------
$! GF - End of CIM System custom modifications
