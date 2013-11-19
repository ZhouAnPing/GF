/*****************************************************************************

   Tokens.h:                                 Date:    March 1998
   ~~~~~~~~~                                 Author:  Bala

   Header file for Tokens used for Parsing Functions

******************************************************************************/

#ifndef _CIMTOKEN_H
#define _CIMTOKEN_H

#define _TOK_MID                                "MID"
#define _TOK_EQPTID                             "EQPTID"
#define _TOK_GRAPHID                    "GRAPHID"
#define _TOK_RECPID                             "RECPID"
#define _TOK_APPLID                             "APPLID"
#define _TOK_LOTID                              "LOTID"
#define _TOK_BATCHID                    "BATCHID"
#define _TOK_USERID                             "USERID"
#define _TOK_OPERATORID                 "USERID"         /* New */
#define _TOK_LOCNAME                    "PROMISLOC"              /* New */
#define _TOK_PASSWORD           "PASSWORD"
#define _TOK_ETX                                "ETX"
#define _TOK_ECD                                "ECD"
#define _TOK_EQPTTYPE                   "EQPTTYPE"
#define _TOK_SECSMODEL                  "SECSMODEL"
#define _TOK_PROMLOCATION               "PROMLOCATION"
#define _TOK_LASTBATCH                  "LASTBATCH"
#define _TOK_AUTOCONFIGFLAG             "AUTOCONFIGFLAG"
#define _TOK_BATCHINGMODE               "BATCHINGMODE"
#define _TOK_AUTOEQPTFLAG               "AUTOEQPTFLAG"
#define _TOK_MAXBATCH                   "MAXBATCH"
#define _TOK_MINLOTINBATCH              "MINLOTINBATCH"
#define _TOK_MAXLOTINBATCH              "MAXLOTINBATCH"
#define _TOK_AUTODCACCEPTFLAG   "AUTODCACCEPTFLAG"
#define _TOK_RECPACTIVATEFLAG   "RECPACTIVATEFLAG"
#define _TOK_NUMLOT                             "NUMLOTS"
#define _TOK_BATCHSTATE         "BATCHSTATE"
#define _TOK_LOTTYPE                "LOTTYPE"
#define _TOK_PROCID             "PROCID"
#define _TOK_PARTID             "PARTID"
#define _TOK_INSTNUM            "INSTNUM"
#define _TOK_TRACEID            "TRACEID"
#define _TOK_TRACEDESC          "TRACEDESC"
#define _TOK_TRIGGEREVENT       "TRIGGEREVENT"
#define _TOK_NUMSAMPLE          "NUMSAMPLE"
#define _TOK_SAMPLEPERIOD       "SAMPLEPERIOD"
#define _TOK_STARTTIME          "STARTTIME"
#define _TOK_ENDTIME            "ENDTIME"
#define _TOK_LOTCOUNT           "LOTCOUNT"
#define _TOK_PARAMNAME                  "PARAMNAME"
#define _TOK_PARAMVAL           "PARAMVAL"
#define _TOK_PARAMCOUNT         "PARAMCOUNT"
#define _TOK_PPID               "PPID"
#define _TOK_DCVARTYPE          "DCVARTYPE"
#define _TOK_REJECTFLAG                 "REJECTFLAG"
#define _TOK_ORIGINATOR                 "ORIGINATOR"
#define _TOK_ADCSOURCE                  "SOURCEKEY"
#define _TOK_RUNTYPE                    "RUNTYPE"
#define _TOK_OWNEREQPTID                "OWNEREQPTID"
#define _TOK_OWNERBATCHID               "OWNERBATCHID"
#define _TOK_COLLTYPE                   "COLLTYPE"
#define _TOK_ARMID                              "ARMID"
#define _TOK_FORCEFLAG                  "FORCEFLAG"
#define _TOK_REMEASUREFLAG      "REMEASUREFLAG"
#define _TOK_OWNERRECPID                "OWNERRECPID"
#define _TOK_COLLFLAG           "COLLFLAG"
#define _TOK_NUMDCVAR                   "NUMDCVAR"
#define _TOK_DCVAR                              "DCVAR"
#define _TOK_DCVARCATEG                 "DCVARCATEG"
#define _TOK_WAFERSAMPLEPLAN    "WAFERSAMPLE"
#define _TOK_SITESAMPLEPLAN     "SITESAMPLE"
#define _TOK_SOURCERVID         "RVID"
#define _TOK_COLLEVENTID                "COLLEVENT"
#define _TOK_COLLEVENTNAME      "COLLEVENTNAME"
#define _TOK_PORTID             "PORTID"
#define _TOK_AUTOACCEPTFLAG     "AUTOACCEPT"
#define _TOK_REMEASUREBATCH     "REMEASUREBATCH"
#define _TOK_EQPTCATG               "EQPTCATG"
#define _TOK_NUMVIDELEM         "NUMVIDELEM"

/*********** Tokens specific to Alarm server ******************************/

#define _TOK_NUMALMLIST         "ALMCNT"
#define _TOK_ALMID                      "ALARMID"
#define _TOK_ALMTEXT            "ALARMTEXT"
#define _TOK_ALMDESC        "ALARMDESC"
#define _TOK_ALMSTATE       "ALARMSTATE"
#define _TOK_SEVERITY       "SEVERITY"
#define _TOK_SEVERITYDESC   "SEVERITYDESC"
#define _TOK_ALMTIME            "ALARMTIME"
#define _TOK_ALMCOUNTER     "ALARMCOUNTER"
#define _TOK_ALMTRGTIM          "TRIGGERTIME"
#define _TOK_ALMCLRTIM          "CLEARTIME"

#define _ALM_GETLIST        "GETLIST"           /* Get alarm list        */
#define _ALM_ALMRPT         "ALARMREPORT"       /* Get alarm report      */
#define _ALM_ALMCLR         "ALARMCLEAR"        /* Get alarm clear       */
#define _ALM_ALMDET                     "ALARMDETAIL"           /* Get alarm detail      */
#define _ALM_REFRESH            "REFRESHALARM"      /* Refresh display info. */
#define _ALM_REPLY_GETLIST  "ALMLIST"           /* Reply for GETLIST     */
#define _ALM_REPLY_ALMCLR       "CMDACK"                    /* Reply for ALARMCLEAR  */
#define _ALM_PERFORMOCAP    "PERFORMOCAP"       /* Send to OCAP server   */

/********** End of tokens specific to Alarm server *************************/

/********** Tokens specific to OCAP server *********************************/

#define _TOK_SOURCE                     "OCAPSOURCE"                    /* ALARM or SPC            */
#define _TOK_SOURCEKEY          "OCAPSOURCEKEY"                 /* Graph id or Eqpt id     */
#define _TOK_SETUPKEY       "OCAPSETUPSOURCEKEY"    /* secstype; only for alarm*/
#define _TOK_SUBKEY                     "OCAPSUBKEY"                    /* OOS/OOC or Alarm id     */
#define _TOK_RUNKEY                     "OCAPRUNKEY"                    /* Batchid or Alarm counter*/
#define _TOK_USERDATA       "USERDATA"          /* User data               */
#define _TOK_USERINST       "USERINST"          /* User instruction desc   */
#define _TOK_USERGROUP      "USERGROUP"         /* User group              */
#define _TOK_SERNUM         "SERNUM"            /* Serial number           */
#define _TOK_COMMENT        "COMMENT"           /* Comment entered by user */
#define _TOK_CHKLSTCNT      "CHKLSTCNT"
#define _TOK_ACTIONCNT      "ACTIONCNT"
#define _TOK_ACTIONNAME     "ACTIONNAME"
#define _TOK_ACTIONVAL      "ACTIONVAL"
#define _TOK_DATE           "DATE"
#define _TOK_DSPCHKLIST     "DSPCHKLIST"                /* Action to display check list */
#define _TOK_HOLDLOT        "HOLDLOT"           /* Action to hold lot           */
#define _TOK_CHNGEQPSTATE   "CHNGEQPSTATE"      /* Action to change eqpt state  */
#define _TOK_SENDNOTIFY     "SENDNOTIFICATION"  /* Action to send notification  */
#define _TOK_DISABLERECP    "DISABLERECP"       /* Action to disable recipe     */
#define _TOK_EXESYSSCRIPT   "EXESYSSCRIPT"      /* Action to execute script file*/
#define _TOK_FORCECOMMENT   "FORCECOMMENT"      /* Action to force a comment    */
#define _TOK_LOGACTION      "LOGACTION"         /* Log Action information to History*/
#define _TOK_EQPALARM       "EQPALARM"          /* String to be sent to Notn. server */
#define _TOK_ALARM          "ALARM"             /* String to be sent to Notfn. server*/
#define _TOK_SPC            "SPC"               /* String for SPC */
#define _TOK_SRCCATG        "SOURCECATG"        /* String for source category */
#define _TOK_SRCSUBCATG     "SOURCESUBCATG"     /* String for source sub catg */
#define _TOK_OOCCOUNT       "OOCCOUNT"
#define _TOK_OOSCOUNT       "OOSCOUNT"

#define _OCAP_GETCHKLIST    "GETCHKLIST"       /* Get check list        */
#define _OCAP_UPDCHKLIST    "UPDCHKLIST"       /* Update check list     */
#define _OCAP_GETCOMMENT    "GETCOMMENT"       /* Get comment           */
#define _OCAP_UPDCOMMENT    "UPDCOMMENT"       /* Update comment        */
#define _OCAP_PERFORMOCAP   "PERFORMOCAP"      /* Perform OCAP actions  */
#define _OCAP_NOTIFY        "NOTIFY"           /* Command to Notfn. server */
#define _OCAP_REPLY_GETCHKLIST  "CHKLIST"          /* Reply to GETCHKLIST cmd */
#define _OCAP_REPLY_GETCOMMENT  "COMMENTDATA"  /* Reply to GETCOMMENT cmd */
#define _OCAP_REPLY_CMDACK      "CMDACK"       /* Command ack reply       */
  
/********** End of tokens specific to OCAP server **************************/

#define _PRS_PTP_LOTINFO     "LOTQUERY_LOTINFO"
#define _PRS_PTP_UPDLOTPARM  "LOTPARM_UPDATE"
#define _PRM_BATCHID         "BATCHID"
#define _PRM_DISPEQPTTYPE    "DISPEQPTTYPE"




/**** Command Identifiers related to BCXsrv*****/
#define _BCX_CBATCH             "CBATCH"            /* Create Batch            */
#define _BCX_UPDBATCHSTATE      "UPDBATCHSTATE"     /* Update BatchState       */
#define _BCX_ADDLOT                             "ADDLOTTOBATCH"     /* Add Lot to Batch        */
#define _BCX_REMLOT                             "REMLOTFROMBATCH"   /* Remove Lot From Batch   */
#define _BCX_DELBATCH                   "DELETEBATCH"       /* Delete Batch            */
#define _BCX_GETEQPTINFO                "GETEQPTINFO"       /* Get Eqpt Info           */
#define _BCX_GETBATCHLIST               "GETBATCHLIST"      /* Get Batch List          */
#define _BCX_GETBATCHPARAM              "GETBATCHPARAM"
#define _BCX_UPDBATCHPARAM              "UPDBATCHPARAM"
#define _BCX_EXIT               "EXIT"              /* Exit Command to Stop    */

/****  Reply Identifiers reaylated to BCXsrv *****/

#define _BCX_REPLY_CBATCH                       "NEWBATCH"                               /* Reply for CBATCH        */
#define _CMD_ACK                                        "CMD_ACK"                                /* Standard acknowledgment */
#define _BCX_REPLY_EQPTINFO                     "EQPTDATA"                               /* Reply for CBATCH        */
#define _BCX_REPLY_BATCHINFO            "BATCHDATA"                      /* Reply for BATCHINFO     */
#define _BCX_REPLY_GETBATCHPARAM        "BATCHPARAMDATA"

/**** Command Identifiers related to ADCSQLsrv*****/

#define _ADCSQL_GETBATCHDTL    "GETBATCHDETAIL"       /* Get Batch Detail */
#define _ADCSQL_GETPREDATA     "GETPREDATA"           /* Get Pre Data */
#define _ADCSQL_GETRVIDLIST    "GETRVIDLIST"          /* Get Rvid List */
#define _ADCSQL_INSMEASDATA    "INSMEASDATA" 
#define _ADCSQL_DELMEASDATA    "DELMEASDATA"
#define _ADCSQL_INSRVIDDATA    "INSRVIDDATA"
#define _ADCSQL_DELRVIDDATA    "DELRVIDDATA"
#define _ADCSQL_GETDCINFO      "GETDCINFO"
#define _ADCSQL_EXIT               "EXIT"              /* Exit Command to Stop    */


/**** Command Identifiers related to ADCSQLsrv*****/

#define _ADCSQL_REPLY_GETBATCHDTL     "BATCHDETAIL"         /* Reply for Batchdetail    */
#define _ADCSQL_REPLY_EQPTINFO        "EQPTDATA"
#define _ADCSQL_REPLY_BATCHINFO       "BATCHDATA"
#define _ADCSQL_REPLY_GETPREDATA      "PREDATA"
#define _ADCSQL_REPLY_GETRVIDLIST     "RVIDLIST"
#define _ADCSQL_REPLY_GETDCINFO       "DCINFO"

/**** Command Identifiers related to SPCSQLsrv*****/
#define _SPCSQL_REPLY_DCVARLIST        "DCVARLIST"
#define _SPCSQL_REPLY_CHARTDET        "CHARTDET"
#define _SPCSQL_REPLY_GRAPHDET        "GRAPHDET"
#define _SPCSQL_REPLY_POINTDET        "POINTDET"
#define _SPCSQL_REPLY_EQPTLIST        "EQPTLIST"
#define _SPCSQL_LOCKGRAPH  "LOCKGRAPH"
#define _SPCSQL_UNLOCKGRAPH  "UNLOCKGRAPH"
#define _SPCSQL_GETDCVARLIST  "GETDCVARLIST"
#define _SPCSQL_GETCHARTDET  "GETCHARTDET"
#define _SPCSQL_GETGRAPHDET  "GETGRAPHDET"
#define _SPCSQL_GETEQPTLIST  "GETEQPTLIST"
#define _SPCSQL_GETPOINTDET  "GETPOINTDET"
#define _SPCSQL_EXIT  "EXIT"


#define _BATCHGUI                          "BGUI"
#define _BATCHVGUI                         "VGUI"
#define _BATCHADHOC                        "ADHOC"
#define _BATCHADC                           "ADC"
#define _BATCHEM                            "EM"

#define _BATCHGUISOURCE            "P"
#define _BATCHVGUIADHOCSOURCE  "Q"
#define _BATCHADCSOURCE            "S"
#define _BATCHEMSOURCE             "D"


/***** ADCSrv Details ****/

#define _TLK_ADC_QNAME         "ADCSRV"  /* To Be Moved to TLKUTL.h later */
#define _TLK_TGTADC_TRACE_LIST  151      /* -do-                          */
#define _ADC_TRACEINFO         "GETRACEINFO" /* Get Trace Information */
#define _ADC_TRACEDETAIL       "GETRACEDETAIL" /*Get Trace Vid Details */       
#define _ADC_REPLY_TRACEINFO   "TRACEINFODATA"
#define _ADC_REPLY_TRACEDETAIL "TRACEDETAILDATA"
#define _ADC_SETUPDC           "SETUPDC"
#define _ADC_REPLY_SETUPDC     "SETUPDCDATA"
#define _ADC_EXIT                      "EXIT"
#define _ADC_DCCOMPLETE        "DCCOMPLETE"
#define _ADC_DCRESET           "DCRESET"
#define _ADC_DCCOM             "DCCOMPLETE"
#define _ADC_DCVGUISTART       "VGUISTART"
#define _ADC_DCVALIDATE        "DCVALIDATE"

/** Tokens ***/
#define _TOK_NUMTRACE          "NUMTRACE"
#define _TOK_NUMVID                             "NUMVID"
#define _TOK_TRACEID           "TRACEID"
#define _TOK_TRACEDESC         "TRACEDESC"
#define _TOK_TRIGGEREVENT      "TRIGGEREVENT"
#define _TOK_NUMSAMPLE         "NUMSAMPLE"  
#define _TOK_SAMPLEPERIOD      "SAMPLEPERIOD"
#define _TOK_VIDS                       "VIDS"
#define _TOK_VIDNAMES          "VIDNAMES" 
#define _TOK_VIDELEM           "NUMVIDELEM"
#define _TOK_VIDFORMAT         "VIDFORMAT"
#define _TOK_GAINFACTOR        "GAINFACTOR" 
#define _TOK_OFFSETFACTOR      "OFFSETFACTOR"
#define _TOK_VARNAME           "DCVAR"
#define _TOK_WAFERCNT          "WAFERCOUNT"
#define _TOK_VARCNT            "VARCOUNT"
#define _TOK_WAFERNO           "WAFERNUMBER"
#define _TOK_WAFERID           "WAFERID"
#define _TOK_MEASDATA          "MEASDATA"
#define _TOK_RECIPE            "RECIPE"
#define _TOK_DCVARCOUNT        "DCVARCOUNT"
#define _TOK_EQPTTYPE          "EQPTTYPE"
#define _TOK_WSAMPPLAN         "WAFERSAMPLINGPLAN"
#define _TOK_SSAMPPLAN         "SITESAMPLINGPLAN"
#define _TOK_RVIDNAME           "RVIDNAME"
#define _TOK_RVID               "RVID"
#define _TOK_SOURCEEQPTMODEL    "SOURCEEQPTSECSMODEL"
#define _TOK_RVIDCOUNT          "RVIDCOUNT"
#define _TOK_SPCFLAG            "SPCFLAG"
#define _TOK_MEASTYPE           "MEASTYPE"
#define _TOK_MEASTEST           "MEASTEST"
#define _TOK_VIDINDEX           "VIDINDEX"
#define _TOK_DESCRIPT        "DESCRIPTION"
#define _TOK_FORMULA         "FORMULA"
#define _TOK_FACTOR          "FACTOR"
#define _TOK_DCVARCATG       "DCVARCATG"
#define _TOK_WAFERPLAN       "WAFERPLAN"
#define _TOK_SITEPLAN       "SITEPLAN"
#define _TOK_LSL                        "LSL"
#define _TOK_USL                        "USL"
#define _TOK_SPCTARGET       "SPCTARGET"
#define _TOK_DATATYPE        "DATATYPE"
#define _TOK_DATATYPECOUNT        "DATATYPECOUNT"
#define _TOK_LVL        "LVL"
#define _TOK_UVL        "UVL"
#define _TOK_PURPOSE            "PURPOSE"
#define _TOK_WFRINCREMENTEVENT "WAFERINCREMENTEVENT"
#define _TOK_SOURCECOUNT                "SOURCECOUNT"
#define _TOK_DCVAROOCCOUNT              "DCVAROOCCOUNT"
#define _TOK_DCVAROOSCOUNT              "DCVAROOSCOUNT"
#define _TOK_CHARTCOUNT         "CHARTCOUNT"
#define _TOK_CHARTTYPE          "CHARTTYPE"
#define _TOK_SAMPLELEVEL                "SAMPLELEVEL"
#define _TOK_SAMPLELEVELNUMBER          "SAMPLELEVELNUMBER"
#define _TOK_OOC                        "OOC"
#define _TOK_OOS                        "OOS"
#define _TOK_PRECALC                    "PRECALC"
#define _TOK_GRAPHCOUNT                 "GRAPHCOUNT"
#define _TOK_CHARTOOCCOUNT              "CHARTOOCCOUNT"
#define _TOK_CHARTOOSCOUNT        "CHARTOOSCOUNT"
#define _TOK_SPECTYPEFLAG        "SPECTYPEFLAG"
#define _TOK_TARGET              "TARGET"
#define _TOK_GRAPHTYPE        "GRAPHTYPE"
#define _TOK_EQPTCOUNT        "EQPTCOUNT"
#define _TOK_OPERID           "OPERID"
#define _TOK_RULECOUNT        "RULECOUNT"
#define _TOK_RULEDESC         "RULEDESC"
#define _TOK_RULENAME         "RULENAME"
#define _TOK_WAFERCOUNT        "WAFERCOUNT"



/* TOken for EMR */

#define _DATA_VFI_EMGMID            "EMG"               
#define _DATA_VFI_DATAID        "DATA_ID"

#define _DATA_VFI_BUTTON_NAME   "BUTTON_NAME"
#define _DATA_VFI_MODE_NAME     "MODE_NAME"
#define _DATA_VFI_OPTION_FLAG   "OPTION_FLAG"
#define _DATA_VFI_OPER_OPTION   "OPERATOR_OPTION"
#define _DATA_VFI_SPSR_STATUS   "SPSR_STATUS"
#define _DATA_VFI_PROMRECIPE    "PROMIS_RECIPE"
#define _DATA_VFI_EQPTRECIPE    "EQPT_RECIPE" 
#define _DATA_VFI_SPSRRECIPE    "SPSR_RECIPE" 
#define _DATA_VFI_JOBFILE       "JOB_FILE"  
#define _DATA_VFI_LAYERID       "LAYERID" 
#define _DATA_VFI_EXPOSURE      "EXPOSURE"  
#define _DATA_VFI_FOCUS         "FOCUS"  
#define _DATA_VFI_PROGID        "PROGRAM_ID" 
#define _DATA_VFI_PROCID        "PROCESS_ID" 
#define _DATA_VFI_RETICLEID     "RETICLE_ID" 
#define _DATA_SPSR_MODE         "SPSR_MODE" 
#define _DATA_SPSR_DATA         "%s" 
#define _DATA_VFI_USERID        "USERID" 
#define _DATA_VFI_PASSWORD      "PASSWORD"
#define _DATA_VFI_COLL_COUNTER  "COLLECTION_COUNTER" 
#define _DATA_VFI_TRACEID       "TRACE_ID" 
#define _DATA_VFI_PARAMCOUNT    "PARAMCOUNT" 
#define _DATA_VFI_SPSRNO        "SPSR_NO"
#define _DATA_VFI_PARAMNAME     "PARAMNAME"
#define _DATA_VFI_PARAMVALUE    "PARAMVALUE"
#define _DATA_VFI_EQPID         "EQPT_ID"

#define _DATAID_BUTTON_CLICK    "BUTTON_CLICK"
#define _DATAID_CHNGPROC_MODE "CHANGE_PROC_MODE"
#define _DATAID_CHNGRUN_MODE "CHANGE_RUN_MODE"
#define _DATAID_INITFAIL_REPLY "INIT_FAILURE_REPLY"
#define _DATAID_SPSRDATA "SPSR_DATA"
#define _DATAID_GETTRACE "GET_TRACE_DATA"
/*  From Trans EMG H file */

#define _DATA_VFI_MID                   "EMR"
#define _DATA_VFI_EMRMSG        "EMR_MSG"
#define _DATA_VFI_EMGMSG        "EMG_MSG"
#define _DATA_VFI_DATAID        "DATA_ID"
#define _DATA_VFI_MSG_CATEG     "MSG_CATEG"
#define _DATA_VFI_MSG_STRING    "MSG_STRING"
#define _DATA_VFI_OPERID        "USER_ID" 
#define _DATA_VFI_PASSWORD      "PASSWORD" 
#define _DATA_VFI_LOTID         "LOT_ID" 
#define _DATA_VFI_ARMID         "ARM_ID"
#define _DATA_VFI_PROMRECIPE    "PROMIS_RECIPE" 
#define _DATA_VFI_EQPTRECIPE    "EQPT_RECIPE" 
#define _DATA_VFI_RETICLEID     "RETICLE_ID" 
#define _DATA_VFI_TRACECOUNT    "TRACE_COUNT"
#define _DATA_VFI_TRACEID               "TRACE_ID"
#define _DATA_VFI_NUMSAMPLES    "NUM_SAMPLES"
#define _DATA_VFI_PERIOD                "SAMPLE_PERIOD"
#define _DATA_VFI_TRACEDESC             "TRACE_DESC"
#define _DATA_VFI_TRIGEVENT             "TRIGGER_EVENT"
#define _DATA_VFI_PENDINGTRACE  "PENDING_TRACE"
#define _DATA_VFI_SPSR_NO       "SPSR_NO"
#define _DATA_VFI_BUTTON_NAME   "BUTTON_NAME"
#define _DATA_VFI_ENABLE_FLAG   "ENABLE_FLAG"
#define _DATA_VFI_ERROR_FLAG    "ERROR_FLAG"
#define _DATA_VFI_BATCHID               "BATCH_ID"
#define _DATA_VFI_WFRQTY                "WFR_QTY"
#define _DATA_VFI_RECPID                "RECIPE_ID"
#define _DATA_VFI_DELFLAG               "DELETE_FLAG"
#define _DATA_VFI_LOTTYPE               "LOT_TYPE"
#define _DATA_VFI_ARMSTS                "ARM_STATUS"
#define _DATA_VFI_FIELDNAME             "FIELD_NAME"
#define _DATA_VFI_FIELDVALUE    "FIELD_VALUE"
#define _DATA_VFI_COLOR                 "COLOR"
#define _DATA_VFI_POPUPNAME             "POPUP_NAME"
#define _DATA_VFI_ADHOC_TYPE    "ADHOC_TYPE"
#define _DATA_VFI_ENABLE_UNLOCK "ENABLE_UNLOCK"
#define _DATA_VFI_ENABLE_LOAD   "ENABLE_LOAD"
#define _DATA_VFI_ENABLE_UNLOAD "ENABLE_UNLOAD"
#define _DATA_VFI_GUI_TYPE      "GUI_TYPE"
#define _DATA_VFI_REWORK_FLAG   "REWORK_FLAG"
#define _DATA_VFI_PARTID        "PART_ID"


#define _DATAID_POPUPMSG                "POPUP_MSG"
#define _DATAID_POPUPUNDRAW     "POPUP_UNDRAW"
#define _DATAID_GETSPSR                 "GET_SPSR_DATA"
#define _DATAID_INITIALIZE              "INITIALIZE"
#define _DATAID_INITFAILURE             "INIT_FAILURE"
#define _DATAID_TRACEDATA               "TRACE_DATA"
#define _DATAID_UPDTRACE                "UPDATE_TRACE"
#define _DATAID_CONFIRMADC              "CONFIRM_ADC"
#define _DATAID_BTNSTATE                "BUTTON_STATE"
#define _DATAID_OPERMSG                 "OPERATOR_MSG"
#define _DATAID_OPERINST                "OPERATOR_INST"
#define _DATAID_LOTINBUFFER             "LOTS_IN_BUFFER"
#define _DATAID_LOTINPROCESS    "LOTS_IN_PROCESS"
#define _DATAID_UPDATEFIELD             "UPDATE_FIELD"
#define _DATAID_ARMINFO                 "ARM_INFO"
#define _DATAID_ARMSTS              "ARM_STATUS"

/* Metrology EMR related strings */
#define _STR_GET_ADHOC          "GET_ADHOC"
#define _STR_TRACKOUT           "TRACKOUT"
#define _STR_REMEASURE_VERIFY   "REMEASURE_VERIFY"
#define _STR_SPEC_VERIFY        "SPEC_VERIFY"
#define _STR_INIT_FAILURE       "INIT_FAILURE"
#define _STR_LOT_INFO           "LOT_INFO"
#define _STR_INITIALIZE         "INITIALIZE"
#define _STR_POPUP_MSG          "POPUP_MSG"
#define _STR_EMR_MSG            "EMR_MSG"
#define _STR_FORCE_VERIFY       "FORCE_VERIFY"
#define _STR_CANCEL_GUI         "CANCEL_GUI"
#define _STR_POPUP_BANNER       "POPUP_BANNER"
#define _STR_CANCEL_BANNER      "CANCEL_BANNER"
#define _STR_PUTLOTINFO         "PUT_LOTINFO"


/* SPC related Color,marker string */

#define _STR_BACKCOLOUR         "BACKCOLOR"
#define _STR_CENTRECOLOUR       "CENTERCOLOR"
#define _STR_LIMITCOLOUR        "LIMITCOLOR"
#define _STR_LINECOLOUR         "LINECOLOR"
#define _STR_CAPTIONCOLOUR      "CAPTIONCOLOR"
#define _STR_OOCCOLOUR          "OOCCOLOR"
#define _STR_POINTCOLOUR        "POINTCOLOR"
#define _STR_TEXTCOLOUR         "TEXTCOLOR"
#define _STR_CAUSECOLOUR        "CAUSECOLOR"
#define _STR_NOCALCCOLOUR       "NOCALCCOLOR"
#define _STR_AXISCOLOUR         "AXISCOLOR"
#define _STR_OOCMARKER          "OOCMARKER"
#define _STR_POINTMARKER        "POINTMARKER"
#define _STR_OOSMARKER          "OUTOFSPECMARKER"
#define _STR_NOCALCMARKER       "NOCALCMARKER"

/*   PRI SRV   */

#define _TOK_AMHSEQUIPID   "AMHSEQUIPID"
#define _TOK_CARRIERID   "CARRIERID"
#define _TOK_VID            "VID"
#define _TOK_PAGENUMBER   "PAGENUMBER"
#define _TOK_PAGELENGTH   "PAGELENGTH"
#define _TOK_TOOLID   "TOOL_ID"
#define _TOK_PRIORITY   "PRIORITY"
#define _TOK_OUTPUTPORT   "OUTPUTPORT"
#define _TOK_SECTION   "SECTION"
#define _TOK_DISPLAYTEXT   "DISPLAYTEXT"
#define _TOK_KEYATTRIBUTE   "KEY_ATTRIBUTE"
#define _TOK_CMDTYP   "CMD_TYPE"
#define _TOK_HIM   "HIM"
#define _TOK_MACHCMD   "MACH_CMD"
/* TOKEN USED IN SPC CONTROL SERVER COMMAND AND REPLY */

#define _TOK_SPCSTART           "SPCSTART"      /* SPC START command to SPCCTLSRV */
#define _TOK_SOURCEEQPTID       "SOURCEEQPTID"  /* SOurce Equipment ID */
#define _TOK_OWNEREQPTID        "OWNEREQPTID"   /* Owner Equipment ID  */

#define _TOK_HELDLOT            "HELDLOT"       /* Lots held */
#define _TOK_DISABLEDRECIPE     "DISABLEDRECIPE" /* Disabled recipe */
#define _TOK_EQPTSTATE          "EQPTSTATE"     /* New Equipment State */

#endif
