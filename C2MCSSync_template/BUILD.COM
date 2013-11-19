! GF - Start of CIM System custom modifications
!--------------------------------------------------------------
! MCS.BLD
!
! Build C objects...
$ set verify
$ purge *.*
$ cc /noopt dmqutil.c + DMQ$USER:DMQ/LIB
$ cc /noopt talkstd.c + DMQ$USER:DMQ/LIB
$ cc /noopt Init_decmsg_Q + DMQ$USER:DMQ/LIB
$ cc /noopt get_mcs_info + DMQ$USER:DMQ/LIB
$ cc PARSVFEI.c  + DMQ$USER:DMQ/LIB
$ cc /noopt trcutil.c
$ cc /noopt hsms_event.c + DMQ$USER:DMQ/LIB
$ del mcslib.olb;*
$ library /create mcslib DMQUTIL.OBJ,GET_MCS_INFO.OBJ,PARSVFEI.OBJ,-
                  HSMS_EVENT.OBJ,INIT_DECMSG_Q.OBJ,TALKSTD.OBJ,TRCUTIL.OBJ
$! Build C++ objects...
$! cxx /DEFINE=(_STP_VMS_ALPHA,_STP_NOT_WINDOWS,_STP_MESSAGEPUMP_USEUDP) [-.HSMSADAPTER]HSMS.CPP
$! cxx /DEFINE=(_STP_VMS_ALPHA,_STP_NOT_WINDOWS,_STP_MESSAGEPUMP_USEUDP) [-.HSMSADAPTER]STLHelper.CPP
$! cxx /DEFINE=(_STP_VMS_ALPHA,_STP_NOT_WINDOWS,_STP_MESSAGEPUMP_USEUDP) [-.HSMSADAPTER]CThreadPool.CPP
$! cxx /DEFINE=(_STP_VMS_ALPHA,_STP_NOT_WINDOWS,_STP_MESSAGEPUMP_USEUDP) [-.HSMSADAPTER]misc.CPP
$! cxx /DEFINE=(_STP_VMS_ALPHA,_STP_NOT_WINDOWS,_STP_MESSAGEPUMP_USEUDP) [-.HSMSADAPTER]CWRAPPER.CPP
$! del hsmslib.olb;*
$! library /create hsmslib HSMS.OBJ,STLHelper.OBJ,CThreadPool.OBJ,misc.OBJ,CWRAPPER.OBJ
$! Build Fortran objects...
@bld:compile  mcssync.for
cxxlink mcssync,mcslib/lib,hsmslib/lib,prom:promis/lib,dmq$lib:dmq/opt,tim:timwrtshr/opt,-
	wct:wctwrtshr/opt,tps:server/opt,BLD:IMAGE/OPTIONAL
$ delete *.lis;*/log
$ delete *.obj;*/log
$ purge *.*
$ !copy *.*; PROMPRI1:[PROMF3.SSP.F235MCS.C2MCSSYNC] /log
$ copy *.exe; PROMPRI1:[PROMF3.SSP.F235MCS.C2MCSSYNC] /log
$ !copy *MCSSYNC.COM; PROMPRI1:[PROMF3.SSP.F235MCS.C2MCSSYNC] /log
$ set noverify
!--------------------------------------------------------------
! GF - End of CIM System custom modifications
