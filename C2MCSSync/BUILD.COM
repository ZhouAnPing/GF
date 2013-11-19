! GF - Start of CIM System custom modifications
!--------------------------------------------------------------
! MCS.BLD
!
! Build C objects...
$ set verify
$ purge *.*
$ cc:==cc/deb/noopt/DEFINE=(DEBUG)
$ cc dmqutil.c + DMQ$USER:DMQ/LIB
$ cc talkstd.c + DMQ$USER:DMQ/LIB
$! cc Init_decmsg_Q + DMQ$USER:DMQ/LIB
$ cc get_mcs_info + DMQ$USER:DMQ/LIB
$ cc PARSVFEI.c  + DMQ$USER:DMQ/LIB
$ cc trcutil.c
$ cc hsms_event.c + DMQ$USER:DMQ/LIB
$ del mcslib.olb;*
$ library /create mcslib GET_MCS_INFO.OBJ,PARSVFEI.OBJ,-
                  DMQUTIL.OBJ,HSMS_EVENT.OBJ,TALKSTD.OBJ,TRCUTIL.OBJ
$ @build_HSMSLib
$! Build Fortran objects...
@bld:compile  mcssync.for
cxxlink mcssync,mcslib/lib,hsmslib/lib,prom:promis/lib,dmq$lib:dmq/opt,tim:timwrtshr/opt,-
	wct:wctwrtshr/opt,tps:server/opt,BLD:IMAGE/OPTIONAL
$ @buildstartupcmd
$ delete *.lis;*/log
$ delete *.obj;*/log
$ purge *.*
$ copy *.exe; PROMPRI1:[FAB3PRI.SSP.F235MCS.C2SYNC] /log
$ set noverify
!--------------------------------------------------------------
! GF - End of CIM System custom modifications
