! GF - Start of CIM System custom modifications
!--------------------------------------------------------------
! MCS.BLD
!
$ set verify
$ purge *.*
$! Build C++ objects...
$ cxx /DEFINE=(_STP_VMS_ALPHA,_STP_NOT_WINDOWS,_STP_MESSAGEPUMP_USEUDP) [-.HSMSADAPTER]HSMS.CPP
$ cxx /DEFINE=(_STP_VMS_ALPHA,_STP_NOT_WINDOWS,_STP_MESSAGEPUMP_USEUDP) [-.HSMSADAPTER]STLHelper.CPP
$ cxx /DEFINE=(_STP_VMS_ALPHA,_STP_NOT_WINDOWS,_STP_MESSAGEPUMP_USEUDP) [-.HSMSADAPTER]CThreadPool.CPP
$ cxx /DEFINE=(_STP_VMS_ALPHA,_STP_NOT_WINDOWS,_STP_MESSAGEPUMP_USEUDP) [-.HSMSADAPTER]misc.CPP
$ cxx /DEFINE=(_STP_VMS_ALPHA,_STP_NOT_WINDOWS,_STP_MESSAGEPUMP_USEUDP) [-.HSMSADAPTER]CWRAPPER.CPP
$ del hsmslib.olb;*
$ library /create hsmslib HSMS.OBJ,STLHelper.OBJ,CThreadPool.OBJ,misc.OBJ,CWRAPPER.OBJ
$ set noverify
!--------------------------------------------------------------
! GF - End of CIM System custom modifications
