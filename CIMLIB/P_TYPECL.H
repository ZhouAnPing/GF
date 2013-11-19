/** -------------------------------------------- */
/** Include file name: PAMS_C_TYPE_CLASS		*/
/** File name: P_TYPECL.H			*/
/** Usage: #include pams_c_type_class		*/
/** 						*/
/**  Function:                                                 **/
/**    This include file defines all the message definitions   **/
/**    used within the MessageQ system.                        **/
/**                                                            **/
/**  NOTE:                                                     **/
/**    The following are reserved to DIGITAL:                  **/
/**                                                            **/
/**        Msg Classes: 28, 29, 62 & 30,000 thru 32,767        **/
/**        Msg Types:   -1 thru -5,000                         **/
/**------------------------------------------------------------**/
 
/****************************************************************/
/**                                                            **/
/**        MRS Class and Type codes (800-899)                  **/
/**                                                            **/
/****************************************************************/
 
#define MSG_CLAS_MRS 28           /* * define MRS class **/
 
/** define MRS type codes **/
#define MSG_TYPE_MRS_DQF_TRANSFER       -700
#define MSG_TYPE_MRS_DQF_TRANSFER_ACK   -701
#define MSG_TYPE_MRS_DQF_TRANSFER_REP   -702
#define MSG_TYPE_MRS_DQF_SET            -704
#define MSG_TYPE_MRS_DQF_SET_REP        -705
#define MSG_TYPE_MRS_SAF_SET            -706
#define MSG_TYPE_MRS_SAF_SET_REP        -707
#define MSG_TYPE_MRS_ACK                -801
#define MSG_TYPE_MRS_SAF_TRANSFER       -805
#define MSG_TYPE_MRS_SAF_TRANSFER_ACK   -806
#define MSG_TYPE_MRS_SAF_TRANSFER_REP   -807
#define MSG_TYPE_MRS_CNF                -840
#define MSG_TYPE_MRS_CNF_ACK            -841
#define MSG_TYPE_MRS_CNF_NAK            -842
#define MSG_TYPE_MRS_DEBUG_ON           -850
#define MSG_TYPE_MRS_DEBUG_OFF          -851
#define MSG_TYPE_MRS_JRN_DISABLE 	-871
#define MSG_TYPE_MRS_JRN_DISABLE_REP 	-872
#define MSG_TYPE_MRS_JRN_ENABLE 	-873
#define MSG_TYPE_MRS_JRN_ENABLE_REP 	-874
#define MSG_TYPE_MRS_SET_PCJ            -880
#define MSG_TYPE_MRS_SET_PCJ_REP        -881
#define MSG_TYPE_MRS_SET_DLJ            -882
#define MSG_TYPE_MRS_SET_DLJ_REP        -883
 
/****************************************************************/
/**                                                            **/
/**        PAMS   Class and Type codes (900-999)               **/
/**                                                            **/
/****************************************************************/
 
#define MSG_CLAS_PAMS 29           /* * define PAMS class **/
 
/** define PAMS type codes **/
#define MSG_TYPE_TIMER_EXPIRED           -900
#define MSG_TYPE_LIST_ALL_Q_REQ          -960
#define MSG_TYPE_LIST_ALL_Q_RESP         -961
#define MSG_TYPE_ENABLE_Q_NOTIFY_REQ     -962
#define MSG_TYPE_ENABLE_Q_NOTIFY_RESP    -963
#define MSG_TYPE_DISABLE_Q_NOTIFY_REQ    -964
#define MSG_TYPE_DISABLE_Q_NOTIFY_RESP   -965
#define MSG_TYPE_Q_UPDATE                -966
#define MSG_TYPE_LOCATE_Q_REP            -972
#define MSG_TYPE_LINKMGT_REQ             -975
#define MSG_TYPE_LINKMGT_RESP            -976
#define MSG_TYPE_DECLARE_SQ              -980
#define MSG_TYPE_UNDECLARE_SQ            -981
#define MSG_TYPE_ALLOCATED_SQ            -982
#define MSG_TYPE_MSG_STATUS              -983
#define MSG_TYPE_DMQ_SERVER_NAK          -989
#define MSG_TYPE_ENABLE_NOTIFY           -990
#define MSG_TYPE_DISABLE_NOTIFY          -991  
#define MSG_TYPE_PROCESS_DCL             -992  
#define MSG_TYPE_PROCESS_EXIT            -993  
#define MSG_TYPE_LIST_DCLS               -994  
#define MSG_TYPE_LIST_ALL_ENTRYS         -995
#define MSG_TYPE_LIST_ALL_ENTRIES        -995
#define MSG_TYPE_LIST_ALL_CONNECTIONS    -996
#define MSG_TYPE_LIST_ALL_GROUPS         -997
#define MSG_TYPE_LINK_LOST               -998
#define MSG_TYPE_LINK_COMPLETE           -999
 
/****************************************************************/
/**                                                            **/
/**       SBS_SERVER - class: PAMS                             **/
/**                    type:  codes (1150-1179)                **/
/**                                                            **/
/****************************************************************/
 
#define MSG_TYPE_SBS_REG              -1150    /* * registration message long form*/
#define MSG_TYPE_SBS_REG_EZ           -1173    /* * registration record short form (new v2 format)*/
#define MSG_TYPE_SBS_REG_REPLY        -1152    /* * registration reply*/
#define MSG_TYPE_SBS_REG_EZ_REPLY     -1153    /* * registration reply*/
#define MSG_TYPE_SBS_DEREG_BY_ID      -1154    /* * deregister request (old v1 format)*/
#define MSG_TYPE_MOT_DEREG            -1154    /* * deregister request (old v1 format)*/
#define MSG_TYPE_SBS_DEREG            -1174    /* * deregister request (new v2 format)*/
#define MSG_TYPE_SBS_DEREG_ACK        -1155    /* * deregister acknowledgement*/
#define MSG_TYPE_SBS_PURGE            -1157    /* * purge registration table*/
#define MSG_TYPE_SBS_PURGE_ACK        -1158    /* * purge registration acknowledgement*/
#define MSG_TYPE_SBS_DUMP_TABLES      -1162    /* * process, reg, and group tables to log*/
#define MSG_TYPE_SBS_BS_SEQGAP        -1166    /* * declaration message missing*/
 
/****************************************************************/
/**                                                            **/
/**       AVAIL_SERVER - class: PAMS                           **/
/**                    type:  codes (1180-1189)                **/
/**                                                            **/
/****************************************************************/
 
#define MSG_TYPE_AVAIL_REG            -1180    /* * available registration */
#define MSG_TYPE_AVAIL_DEREG          -1181    /* * available deregistration */
#define MSG_TYPE_AVAIL_REG_REPLY      -1182    /* * registration reply*/
#define MSG_TYPE_AVAIL                -1183    /* * process is now available */
#define MSG_TYPE_UNAVAIL              -1184    /* * process is now unavailable */
 
/****************************************************************/
/**                                                            **/
/**       ETHERNET   Class and Type codes (1000-1010)          **/
/**                                                            **/
/****************************************************************/
 
#define MSG_CLAS_ETHERNET  100      /* * define Ethernet class */
 
/** define Ethernet type codes*/
#define MSG_TYPE_E_CONNECT            -1000    /* * Initiate connection         */
#define MSG_TYPE_E_DISCONNECT         -1001    /* * shutdown circuit               */
#define MSG_TYPE_E_INITIALIZE         -1002    /* * set Ethernet characteristics */
#define MSG_TYPE_E_CONNECT_COMPLETE   -1003    /* * circuit established            */
#define MSG_TYPE_E_CONNECT_REJECTED   -1004    /* * partner rejected circuit       */
#define MSG_TYPE_E_PARTNER_DISC       -1005    /* * partner disconnected circuit   */
#define MSG_TYPE_E_IO_ERROR           -1006    /* * bad return status from QIO$ */
#define MSG_TYPE_E_PROTOCOL_ERROR     -1007    /* * unrecognized msg from partner*/
#define MSG_TYPE_E_LISTEN_TIMEOUT     -1008    /* * partner's heartbeat stopped */
#define MSG_TYPE_E_MESSAGES_LOST      -1009    /* * detected msg loss           */
#define MSG_TYPE_E_RUNTIME_ERROR      -1010    /* * detected runtime error in AST*/
 
/***************************************************/
/**       Generic UCB Types and Classes           **/
/***************************************************/
 
#define MSG_CLAS_UCB    102
 
#define MSG_TYPE_UCB_CONNECT          -1200    /* * Initiate connection         */
#define MSG_TYPE_UCB_DISCONNECT       -1201    /* * shutdown circuit            */
#define MSG_TYPE_UCB_INITIALIZE       -1202    /* * set UCB characteristics   */
#define MSG_TYPE_UCB_CONNECT_COMPLETE -1203    /* * circuit established         */
#define MSG_TYPE_UCB_PARTNER_DISC     -1204    /* * partner disconnected circuit*/
#define MSG_TYPE_UCB_MESSAGES_LOST    -1205    /* * detected msg loss           */
#define MSG_TYPE_UCB_IO_ERROR         -1206    /* * bad return status from QIO$ */
#define MSG_TYPE_UCB_RCV_DATA         -1207    /* * data message received by UCB*/
 
 
/****************************************************************/
/**                                                            **/
/**        DEFINE DEMO type codes for DEMO_IO_SERVER           **/
/**                                                            **/
/****************************************************************/
 
#define MSG_TYPE_DEMO_ADD             -1200    /* * add record function   **/
#define MSG_TYPE_DEMO_CHG             -1216    /* * chg record function   **/
#define MSG_TYPE_DEMO_DEL             -1232    /* * del record function   **/
#define MSG_TYPE_DEMO_INQ             -1248    /* * inq record function   **/
#define MSG_TYPE_DEMO_LST             -1280    /* * list record function  **/
#define MSG_TYPE_DEMO_LST_NEXT        -1281    /* * list next record function **/
#define MSG_TYPE_DEMO_CLIENT_UP       -1282    /* * client up             **/
#define MSG_TYPE_DEMO_CLIENT_DOWN     -1283    /* * client down           **/
 
/****************************************************************/
/**                                                            **/
/**        DEFINE LU62 type codes for LU6.2 UCB		     **/
/**                                                            **/
/****************************************************************/
 
#define MSG_CLAS_APPC 62	     /* * define LU6.2 class */
 
#define MSG_TYPE_LU62_INIT  	-6200	     /* * init the interface*/
#define MSG_TYPE_LU62_DEFINE_LU         -6201        /* * define an LU*/
#define MSG_TYPE_LU62_ALLOCATE 	 	-6202	     /* * allocate a conversation*/
#define MSG_TYPE_LU62_SEND_DATA 	-6203	     /* * send a data block*/
#define MSG_TYPE_LU62_REQ_CONFIRM 	-6204	     /* * ask the remote to CONFIRM*/
#define MSG_TYPE_LU62_SEND_CONFIRM 	-6205	     /* * reply to a CONFIRM REQ*/
#define MSG_TYPE_LU62_CONFIRM_RECV 	-6206	     /* * Req CONFIRM, go to recv when obtained*/
#define MSG_TYPE_LU62_DEALLOCATE 	-6207        /* * deallocate a conversation*/
#define MSG_TYPE_LU62_SEND_ERROR 	-6208	     /* * Signal an error*/
#define MSG_TYPE_LU62_DELETE_LU 	-6209	     /* * Delete an LU*/
 
#define MSG_TYPE_LU62_RECV_DATA  	-6210	     /* * data received from remote partner*/
#define MSG_TYPE_LU62_CONFIRM_REQ 	-6211	     /* * remote wants a CONFIRM*/
#define MSG_TYPE_LU62_CONFIRM_SEND 	-6212	     /* * remote has CONFIRMED, gone to recv*/
#define MSG_TYPE_LU62_CONFIRMED  	-6213	     /* * remote has CONFIRMED*/
#define MSG_TYPE_LU62_ERROR  	-6214	     /* * Error has been signalled*/
#define MSG_TYPE_LU62_DEALLOCATED 	-6215	     /* * Conversation has been deallocated*/
 
/****************************************************************/
/**                                                            **/
/** The following message types are for 2.1 functionality      **/
/**                                                            **/
/****************************************************************/
 
#define MSG_TYPE_LU62_REQ_SEND          -6220        /* * Issue REQUEST_TO_SEND */
#define MSG_TYPE_LU62_CONNECTED 	-6221	     /* * Remote has allocated a conversation*/
#define MSG_TYPE_LU62_DEFINE_TP 	-6222        /* * Define TPN for remote to use*/
#define MSG_TYPE_LU62_ACTIVATE 	-6223        /* * Wait for inbound connect*/
#define MSG_TYPE_LU62_OK_SEND           -6224        /* * Remote is in RECEIVE State */
#define MSG_TYPE_LU62_FLUSH             -6225        /* * Force transmission of data */
 
/****************************************************************/
/**                                                            **/
/** The following message types are for the Generic Port Server**/
/**                                                            **/
/****************************************************************/
 
#define MSG_TYPE_REGISTER_TARGET -6280	     /* * register a VAX TP*/
#define MSG_TYPE_CONNECT_REQUEST -6281	     /* * ask for a connection to IBM*/
#define MSG_TYPE_CONNECT_ACCEPT         -6282        /* * Connection established*/
#define MSG_TYPE_CONNECT_REJECT 	-6283	     /* * Connection request rejected*/
#define MSG_TYPE_CONNECTION_TERMINATED  -6284        /* * Connection broken*/
#define MSG_TYPE_CHANGE_DIRECTION -6285        /* * IBM issued RECEIVE*/
#define MSG_TYPE_DATA_MESSAGE 	-6286	     /* * Application data / control*/
#define MSG_TYPE_LOG_EVENT 	-6287	     /* * Event Log Message*/
#define MSG_TYPE_LU62_ADD_LU 	-6288	     /* * dynamic LU definition*/
#define MSG_TYPE_LU62_ADD_TARGET -6289        /* * dynamic Target definition*/
#define MSG_TYPE_LU62_REPORT_STATUS     -6290        /* * report LU status to event stream*/
#define MSG_TYPE_LU62_SHUTDOWN 	-6299	     /* * DIVE DIVE DIVE*/
#define MSG_CLAS_PROMIS                  377
/************** End of PAMS_xxx_TYPE_CLASS ***************/
