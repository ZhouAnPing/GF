/*************************************************************************

 PRIDispatch:                                      Date:  March 1998
 ~~~~~~~~~~~~~                                     Author: Ben

 This function is the router for processing the PRI server's input
 commands.It determines which function should be called based on the
 contents of the inbound message's header.

 Arguments:
 ~~~~~~~~~~
 None.

 Returns:
 ~~~~~~~~
 Nothing.

 Modifications:
 ~~~~~~~~~~~~~~
 Author    Date    Description...
 ------    ------  ------------------------------------------------------
 Ivan          001004  redo code structure

 **************************************************************************/

/*...Standard include files...*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/*...CIMLIB include files...*/
#ifdef WIN32
#include "talkutl.h"
#include "parsutl.h"
#else
#include "ssp_cimlib:talkutl.h"
#include "ssp_cimlib:parsutl.h"
#endif

/*...MHServ include files...*/
#include "prisrv.h"

int PRIDispatch_EVENT_REPORT();
int PRIDispatch_MACH_CMD();
int PRIDispatch_ALL_OTHERS(char *);

int PRIDispatch(char * Cmd) {
	/*--------------------------------------------------------------
	 Dispatch VFEI format commands
	 --------------------------------------------------------------*/
	TRC_ClrAlarm();
	if (strcmp(Cmd, "EVENT_REPORT") == 0)
		return (PRIDispatch_EVENT_REPORT());

	if (strcmp(Cmd, "MACH_CMD") == 0)
		return (PRIDispatch_MACH_CMD());

	if (strcmp(Cmd, "ALARM_REPORT") == 0)
		return (1);

	return (PRIDispatch_ALL_OTHERS(Cmd));
}

int PRIDispatch_EVENT_REPORT() {
	char tokenVal[_SIZ_MSGBUFF] = { 0 };
	char Carrierid[_SIZ_CARRIERID] = { 0 };
	char Amhsequipid[_SIZ_AMHSEQUIPID] = { 0 };
	char Category[_SIZ_CATEGORY] = { 0 };
	char Number[_SIZ_NUMBER] = { 0 };
	char ErrorMsg[_SIZ_GENFIELD] = { 0 };
	int retval = 0;
	int ptpstatus;
	extern int transnet_down;
	//char smpLocation[_SIZ_GENFIELD]={0};

	int count;
	char tCarrierId[_MAX_CARRIER][_SIZ_CARRIERID];
	char tLocationId[_MAX_CARRIER][_SIZ_LOCATIONID];

	if (PRS_VFI_GetToken("EVENT_ID", _SIZ_EVENTID, tokenVal) != _RET_SUCCESS) {
		return (TRC_SetAlarm("MH_Event_Report_Handler", _RET_FLDMISSING,
				"EVENT_ID"));
	}

	if (strcmp(tokenVal, "CARRIER_ENTERED") == 0) {

		PRIParseCarrierEntered(Carrierid, Amhsequipid, Category, Number);

		PRIMethodCarrierEntered(Carrierid, Amhsequipid, Category, Number,
				&ptpstatus, ErrorMsg);

		return (TRC_ChkAlarm());
	}

	if (strcmp(tokenVal, "CARRIER_REMOVED") == 0) {

		PRIParseCarrierRemoved(Carrierid, Amhsequipid);

		PRIMethodCarrierRemoved(Carrierid, Amhsequipid, &ptpstatus, ErrorMsg);

		return (TRC_ChkAlarm());
	}

	if (strcmp(tokenVal, "LOC_CHANGED") == 0) {

		PRIParseCarrierEntered(Carrierid, Amhsequipid, Category, Number);

		PRIMethodLocChanged(Carrierid, Amhsequipid, Category, Number,
				&ptpstatus, ErrorMsg);

		return (TRC_ChkAlarm());
	}
	return (TRC_SetAlarm("MH_Event_Report_Handler", _RET_FAILURE,
			"UNKNOWN EVENTID"));
}

int PRIDispatch_MACH_CMD() {
	char Mid[_SIZ_MID];
	char tokenVal[_SIZ_MSGBUFF];
	int Tid;
	int Status, retval = 0;
	extern int transnet_down;

	PRS_VFI_ChkInpMsg();
	if (PRS_VFI_GetToken("CMD_TYPE", _SIZ_CMDTYPE, tokenVal) != _RET_SUCCESS)
		return (TRC_SetAlarm("MH_Mach_Cmd_Handler", _RET_FLDMISSING, "CMD_TYPE"));

	// TRC_Send(LOG_ALIAS, _TRC_LVL_DEBUG,
	//    "%s: MHsrv %s %s.\n",
	//    TRC_GetTime(),message.buffer,tokenVal);

	if (strcmp(tokenVal, "STARTUP") == 0) {
		TransNet_Startup = 1;

		if (PRS_VFI_GetToken("TID", _PRS_UINT, &Tid) != _RET_SUCCESS)
			return (TRC_SetAlarm("MH_Mach_Cmd_Handler", _RET_FLDMISSING, "TID"));

		/*  added release of set locations on (re)start of transnet.  */
		if (FAILLOTstart)
			FreeFailedSetLocationsMem();
		/*  end add ***************************************************/

		Status = MH_TransNet_Init(Tid);
		if (Status != _RET_SUCCESS) {
			TRC_Send(LOG_ALIAS, _TRC_LVL_WARN,
					"%s: MHsrv unable to finish \"STARTUP\" sequence with PRI TransNet.\n",
					TRC_GetTime());
			return (TRC_ChkAlarm());
		}

		TRC_Send(LOG_ALIAS, _TRC_LVL_INFO,
				"%s: MHsrv established \"STARTUP\" sequence with PRI TransNet successfully.\n",
				TRC_GetTime());
		TransNet_Startup = 0;
		transnet_down = 0;
		return (TRC_SetAlarm("MH_Mach_Cmd_Handler", _RET_SUCCESS));
	}

	/*******************************************************************************
	 ***  Begin Modification 000926 Ivan
	 ***  Added CMD_ACK reply to the HEARTBEAT command from TRANSNET
	 ***
	 ***/
	if (strcmp(tokenVal, "HELLO") == 0) {
		strcpy(Mid, "TRANSNET");
		PRIReplyCmdack(Mid);
		transnet_down = 0;
		return (TRC_ChkAlarm());
	}
	/***
	 ***  End Modification 000926 Ivan
	 ******************************************************************************/

	if (strcmp(tokenVal, "SHUTDOWN") == 0) {
		strcpy(Mid, "TRANSNET");
		PRIReplyCmdack(Mid);
		transnet_down = 1;
		return (TRC_ChkAlarm());
	}
	/*******************************************************************************
	 *** Added reply to whoever with error text for unknown command
	 ***/
	PRIParseMid(Mid); /* Also clears alarm */
	TRC_SetAlarm("MH_Mach_Cmd_Handler", _RET_CMDINVALID, tokenVal);
	PRIReplyCmdack(Mid);
	return TRC_GetAlarm();
}

int PRIDispatch_ALL_OTHERS(char *Cmd) {
	char Mid[_SIZ_MID] = { 0 };
	char Carrierid[_SIZ_CARRIERID] = { 0 };
	char Amhsequipid[_SIZ_AMHSEQUIPID] = { 0 };
	char Category[_SIZ_CATEGORY] = { 0 };
	char UserId[_SIZ_USERID] = { 0 };
	char Number[_SIZ_NUMBER] = { 0 };
	char ErrorMsg[_SIZ_GENFIELD] = { 0 };
	char rHostname[_SIZ_MID] = { 0 };
	int retval = 0;
	int ptpstatus = 0;
	extern int transnet_down;

	/*... save hostname ...*/
	strcpy(rHostname, message.host);
	/*--------------------------------------------------------------*/
	/* Invalid or unkown command */
	PRIParseMid(Mid);
	TRC_SetAlarm("PRIDispatch", _RET_CMDINVALID, Cmd);
	PRIReplyCmdack(Mid);
	return (TRC_ChkAlarm());
}

int PRIDispatch_NVFEI(char *Cmd) {
	/*--------------------------------------------------------------
	 Dispatch NON VFEI format commands
	 --------------------------------------------------------------*/
	char Mid[_SIZ_MID];

	char ErrorMsg[_SIZ_GENFIELD];
	char rHostname[_SIZ_MID] = { 0 };

	extern int transnet_down;

	TRC_ClrAlarm();

	if (transnet_down) {
		strcpy(ErrorMsg, "Transnet is down ");
		PRIReplyMCSFailure(ErrorMsg);
		return (0);
	}
	/*... Save host information ...*/
	strcpy(rHostname, message.host);

	PRIParseMid(Mid);
	TRC_SetAlarm("PRIDispatch()", _RET_CMDINVALID, Cmd);
	strcpy(message.host, rHostname); /*... put back hostname ...*/
	PRIReplyCmdack(Mid);
	return (TRC_SetAlarm("PRIsrv received ", _RET_FAILURE, "UNKNOWN CMD"));

	return (TRC_ChkAlarm());
}
