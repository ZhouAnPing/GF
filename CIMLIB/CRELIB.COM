$! CSM - Start of CIM System custom modifications
$!--------------------------------------------------------------
$
$ purge *.*
$ CC/deb/noopt  ordarray.c  + DMQ$USER:DMQ/LIB
$ CC/deb/noopt  trcutil.c + DMQ$USER:DMQ/LIB
$ CC/deb/noopt  talkdmq.c + DMQ$USER:DMQ/LIB
$ cc/deb/noopt  parsptp.c  + DMQ$USER:DMQ/LIB
$ cc/deb/noopt  parsvfei.c  + DMQ$USER:DMQ/LIB
$ cc/deb/noopt  parsdel.c  + DMQ$USER:DMQ/LIB
$ cc/deb/noopt/stand=vaxc talkdmq2.c + dmq$user:dmq/lib
$ del/nolog/nocon cimlib.olb;*
$ WRITE SYS$OUTPUT "OLD .olb DELETED"
$ library /create cimlib  ordarray.OBJ,talkdmq.OBJ,TRCUTIL.OBJ,-
parsvfei.obj,parsptp.obj,parsdel.obj,talkdmq2.OBJ
$ WRITE SYS$OUTPUT ".olb CREATED"
$ del *.obj;*/nolog/noconfirm
$
$!--------------------------------------------------------------
$! CSM - End of CIM System custom modifications

