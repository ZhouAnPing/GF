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
$ cc Init_decmsg_Q + DMQ$USER:DMQ/LIB
$ cc get_mcs_info + DMQ$USER:DMQ/LIB
$ cc trcutil.c
$ del mcslib.olb;*
$ library /create mcslib DMQUTIL.OBJ,GET_MCS_INFO.OBJ,-
                  INIT_DECMSG_Q.OBJ,TALKSTD.OBJ,TRCUTIL.OBJ
$! Build Fortran objects...
@bld:compile  mcssync.for
link mcssync,mcslib/lib,prom:promis/lib,dmq$lib:dmq/opt,tim:timwrtshr/opt,-
	wct:wctwrtshr/opt,tps:server/opt,BLD:IMAGE/OPTIONAL
$ delete *.lis;*/log
$ delete *.obj;*/log
$ purge *.*
$ copy mcssync.exe; PROMPRI1:[FAB3PRI.SSP.F235MCS.C3SYNC]mcssync.exe;/log
$ set noverify
!--------------------------------------------------------------
! GF - End of CIM System custom modifications
