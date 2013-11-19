! GF - Start of CIM System custom modifications
!--------------------------------------------------------------
! HSMS Adapter Test Program build scripts
!
! Build C++ objects...
$ set verify
$ cxx /DEFINE=(_STP_VMS_ITANIUM,_STP_NOT_WINDOWS,_STP_MESSAGEPUMP_USEUDP) HSMS.CPP
$ cxx /DEFINE=(_STP_VMS_ITANIUM,_STP_NOT_WINDOWS,_STP_MESSAGEPUMP_USEUDP) STLHelper.CPP
$ cxx /DEFINE=(_STP_VMS_ITANIUM,_STP_NOT_WINDOWS,_STP_MESSAGEPUMP_USEUDP) CThreadPool.CPP
$ cxx /DEFINE=(_STP_VMS_ITANIUM,_STP_NOT_WINDOWS,_STP_MESSAGEPUMP_USEUDP) misc.CPP
$ cxx /DEFINE=(_STP_VMS_ITANIUM,_STP_NOT_WINDOWS,_STP_MESSAGEPUMP_USEUDP) CWRAPPER.CPP
$ delete hsmslib.olb;*/log
$ library /create hsmslib HSMS.OBJ,STLHelper.OBJ,CThreadPool.OBJ,misc.OBJ,CWRAPPER.OBJ
$! Build C objects...
$ cc /noopt  testclient.c
$ cxxlink testclient,hsmslib/lib
$! cxxlink testclient,hsms,misc,stlhelper,cthreadpool,cwrapper
$ purge
$ delete *.obj;*/log
$ set noverify
!--------------------------------------------------------------
! GF - End of CIM System custom modifications
