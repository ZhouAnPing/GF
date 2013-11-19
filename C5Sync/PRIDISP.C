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

	if (PRS_VFI_GetToken("EVENT_ID", _SIZ_EVENTID, tokenVal) != _RET_SUCCESS)
		return (TRC_SetAlarm("MH_Event_Report_Handler", _RET_FLDMISSING,
				"EVENT_ID"));

	if (strcmp(tokenVal, "CARRIER_ENTERED") == 0) {
		/********************************************************************
		 ***  Start mods for differentiating lots at fab3 or smp. Retval
		 ***  contains either _RET_FAB3LOCATION, _RET_SMPLOCATION or error
		 ***/
		retval = PRIParseCarrierEntered(Carrierid, Amhsequipid, Category,
				Number);
		switch (retval) {
		case _RET_FAB3LOCATION:
			FAB3MethodCarrierEntered(Carrierid, Amhsequipid, Category, Number,
					&ptpstatus, ErrorMsg);
			break;

		case _RET_SMPLOCATION:
			PRIMethodCarrierEntered(Carrierid, Amhsequipid, Category, Number,
					&ptpstatus, ErrorMsg);
			break;
		default:
			TRC_Send(LOG_ALIAS, _TRC_LVL_WARN,
					"%s: Lot %s has blank promis location.\n", TRC_GetTime(),
					Carrierid);
			return (TRC_ChkAlarm());
			break;
		}
		/***
		 ***  End mods for differentiating carrierids at fab3 or smp
		 ********************************************************************/
		return (TRC_ChkAlarm()); /* This is still required to indicate result of program
		 * execution other than the default case
		 */
	}

	if (strcmp(tokenVal, "CARRIER_REMOVED") == 0) {
		/********************************************************************
		 ***  Start mods for differentiating lots at fab3 or smp. Retval
		 ***  contains either _RET_FAB3LOCATION, _RET_SMPLOCATION or error
		 ***/

		retval = PRIParseCarrierRemoved(Carrierid, Amhsequipid);
		switch (retval) {
		case _RET_FAB3LOCATION:
			FAB3MethodCarrierRemoved(Carrierid, Amhsequipid, &ptpstatus,
					ErrorMsg);
			break;

		case _RET_SMPLOCATION:
			PRIMethodCarrierRemoved(Carrierid, Amhsequipid, &ptpstatus,
					ErrorMsg);
			break;
		default:
			TRC_Send(LOG_ALIAS, _TRC_LVL_WARN,
					"%s: Lot %s has blank promis location.\n", TRC_GetTime(),
					Carrierid);
			return (TRC_ChkAlarm());
			break;
		}
		/***
		 ***  End mods for differentiating lots at fab3 or smp
		 ********************************************************************/
		return (TRC_ChkAlarm()); /* This is still required to indicate result of program
		 * execution other than the default case      */
	}

	if (strcmp(tokenVal, "LOC_CHANGED") == 0) {
		/********************************************************************
		 ***  Start mods for differentiating lots at fab3 or smp. Retval
		 ***  contains either _RET_FAB3LOCATION, _RET_SMPLOCATION or error
		 ***/
		retval = PRIParseCarrierEntered(Carrierid, Amhsequipid, Category,
				Number);
		switch (retval) {
		case _RET_FAB3LOCATION:
			FAB3MethodLocChanged(Carrierid, Amhsequipid, Category, Number,
					&ptpstatus, ErrorMsg);
			break;
		case _RET_SMPLOCATION:
			PRIMethodLocChanged(Carrierid, Amhsequipid, Category, Number,
					&ptpstatus, ErrorMsg);
			break;
		default:
			TRC_Send(LOG_ALIAS, _TRC_LVL_WARN,
					"%s: Lot %s has blank promis location.\n", TRC_GetTime(),
					Carrierid);
			return (TRC_ChkAlarm());
			break;
		}
		/***
		 ***  End mods for differentiating carrierids at fab3 or smp
		 ********************************************************************/
		return (TRC_ChkAlarm()); /* This is still required to indicate result of program
		 * execution other than the default case
		 */
	}
	if (strcmp(tokenVal, "F3GETALLLOTLOC") == 0) {
		FAB3ParseExecF3GetAllLotLoc();
		return (TRC_ChkAlarm());
	}
	if (strcmp(tokenVal, "TOSHMCS_LOC_CHANGE") == 0) {
		/*... Check for error message from Toshiba MCS ...*/
		PRS_VFI_GetToken("ECD", _PRS_UINT, &retval);
		/*... if error, return. logging already done when msg came thru ...*/
		if (retval)
			return 0;
		PRIParseToshMcsLocChange(&count, tCarrierId, tLocationId);
		/*... if no lots, return ...*/
		if (!count)
			return 0;
		PRIMethodToshMcsLocChange(count, tCarrierId, tLocationId);
		return (TRC_ChkAlarm());
	}
	/*
	 |
	 |   functionality moved to TICKSRV
	 |
	 |   if (strcmp(tokenVal,"TOSHMCS_QRY_TIMEOUT")==0)
	 {
	 if(PRS_VFI_GetToken(TOK_TICKID, _SIZ_GENFIELD, tickId)!=_RET_SUCCESS)
	 return (TRC_SetAlarm("PRIDispatch_EVENT_REPORT", _RET_FLDMISSING, TOK_TICKID));

	 /*... if tick is upto 60 secs old use it ...* /
	 myTime = time(NULL) - 60 ;
	 tickTime = (time_t) atoi(tickId) ;
	 if(tickTime>=myTime)
	 (void)PRIMethodToshLotLoc();
	 else
	 {
	 sprintf(ErrorMsg
	 ,"Recieved expired message from TICKSRV\n%d ->PRISRVtime\n%d ->Ticktime"
	 , myTime, tickTime);
	 TRC_SetAlarm("PRIDispatch_EVENT_REPORT",_RET_GENERIC, ErrorMsg);
	 }
	 return (TRC_ChkAlarm());
	 }
	 |
	 */
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
	/*******************************************************************************
	 ***  Begin Modification 001211 Ivan
	 ***  as an addition due to mismatch in cmds from fab3 to smp
	 ***/
	if (strcmp(tokenVal, "MOVE") == 0) {
		/*-------------------------------------------------------------------
		 Make move in SMP, send update to FAB3 promis
		 Manage in PRIDispatch_all_others()
		 -------------------------------------------------------------------*/
		return (PRIDispatch_ALL_OTHERS("RETRIEVEFAB3LOT"));
	}
	/***
	 ***  End Modification 001211 Ivan
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
	if (strcmp(Cmd, "SETLOTLOCATION") == 0) {
		/* Perform Get Dcvar List */
		PRIParseSetLotLocation(Mid, Carrierid, Amhsequipid, Category);
		strcpy(Number, "");

		if (TRC_ChkAlarm())
			PRIMethodCarrierEntered(Carrierid, Amhsequipid, Category, Number,
					&ptpstatus, ErrorMsg);
		strcpy(message.host, rHostname); /*... put back hostname ...*/
		PRIReplyCmdack(Mid);
		return (TRC_ChkAlarm());
	}

	if (strcmp(Cmd, "CLEARLOTLOCATION") == 0) {
		/* Perform Get Chart List */
		PRIParseClearLotLocation(Mid, Carrierid, Amhsequipid);
		if (TRC_ChkAlarm())
			PRIMethodCarrierRemoved(Carrierid, Amhsequipid, &ptpstatus,
					ErrorMsg);
		strcpy(message.host, rHostname); /*... put back hostname ...*/
		PRIReplyCmdack(Mid);
		return (TRC_ChkAlarm());
	}

	if (strcmp(Cmd, "RETRIEVELOT") == 0) {
		/* Using Category for preferred output port*/
		PRIParseRetrieveLot(Mid, Carrierid, Amhsequipid, UserId, Category);
		if (strlen(Amhsequipid) == 0) //manage test lots' null locations Ivan 287364
			return -1;

		strcpy(Number, "");
		//strcpy(Category,"OPORT"); //Ivan adding port from command recieved
		/*
		 Location or Category contains
		 1. "OUTPUT" - is a retrieve command
		 2. "PORT-A" - is a retrieve command
		 3. ""       - is a store into stocker command
		 4. "NONE"   - is a store into stocker command
		 */
		if (strlen(Category) > 0) {
			if (strcmp(Category, "OUTPUT") == 0)
				strcpy(Category, "PORT"); // set to default port(else parsed value)
			else if (strcmp(Category, "NONE") == 0)
				memset(Category, 0, sizeof(Category));
		}

		if (TRC_ChkAlarm()) {
			if (!Fab3Stocker(Amhsequipid))  //PROMISLOC in tag data
					{
				/*...Added UserId...*//*...and outputport (location)...*/
				/*...send to smp (_RET_SMPLOCATION)...*/
				PRIMethodMove(Carrierid, Amhsequipid, UserId, Category,
						ErrorMsg);
				//if( TRC_ChkAlarm())
				// PRIMethodCarrierEntered( Carrierid, Amhsequipid, Category, Number,&ptpstatus,ErrorMsg);
			} else {
				/*...send to FAB3 (_RET_FAB3LOCATION)...*/
				FAB3MethodMove(Carrierid, Amhsequipid, Category, UserId,
						&retval, ErrorMsg);

				/*... The move above was for smp lot in fab3stocker. If success,
				 ... need to clear the location in SMP promis ...*/
				if (TRC_ChkAlarm())
					PRIMethodCarrierRemoved(Carrierid, Amhsequipid, &retval,
							ErrorMsg);
			}
		}
		strcpy(message.host, rHostname); /*... put back hostname ...*/
		PRIReplyCmdack(Mid);
		return (TRC_ChkAlarm());
	}

	if (strcmp(Cmd, "RETRIEVEFAB3LOT") == 0) {
		/*--------------------------------------------------------------
		 RETRIEVEFAB3LOT
		 Added new function to manage move for FAB3 lots
		 --------------------------------------------------------------*/
		/* Using Category for preferred output port*/
		PRIParseRetrieveLot(Mid, Carrierid, Amhsequipid, UserId, Category);
		strcpy(Number, "");
		//strcpy(Category,"OPORT");   //Ivan adding port from command recieved
		/*
		 Location or Category contains
		 1. "OUTPUT" - is a retrieve command
		 2. "PORT-A" - is a retrieve command
		 3. ""       - is a store into stocker command
		 4. "NONE"   - is a store into stocker command
		 */
		if (strlen(Category) > 0) {
			if (strcmp(Category, "OUTPUT") == 0)
				strcpy(Category, "PORT"); // set to default port(else parsed value)
			else if (strcmp(Category, "NONE") == 0)
				memset(Category, 0, sizeof(Category));
		}

		if (TRC_ChkAlarm()) {
			//  if(strlen(Amhsequipid)==0) //manage test lots' null locations Ivan 287364
			//     return -1;
			if (!Fab3Stocker(Amhsequipid))  //PROMISLOC in tag data
				/*...Added UserId...*//*...and outputport (location)...*/
				/*...send to smp (_RET_SMPLOCATION)...*/
				PRIMethodMove(Carrierid, Amhsequipid, UserId, Category,
						ErrorMsg);
			else {
				/*...send to FAB3 (_RET_FAB3LOCATION)...*/
				FAB3MethodMove(Carrierid, Amhsequipid, Category, UserId,
						&retval, ErrorMsg);

				/*... The move above was for smp lot in fab3stocker. If success,
				 ... need to clear the location in SMP promis ...*/
				if (TRC_ChkAlarm())
					PRIMethodCarrierRemoved(Carrierid, Amhsequipid, &retval,
							ErrorMsg);
			}
		}
		strcpy(message.host, rHostname);
		PRIReplyCmdack(Mid);
		return (TRC_ChkAlarm());
	}
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
	char Carrierid[_SIZ_CARRIERID];
	char Amhsequipid[_SIZ_AMHSEQUIPID];
	char Category[_SIZ_CATEGORY];
	char Number[_SIZ_NUMBER];
	char Location[_SIZ_CATEGORY];
	char Priority[_SIZ_MCSPRIORITY];
	char Eqptid[_SIZ_EQPTID];
	char UserID[_SIZ_USERID]; /* New */
	char ErrorMsg[_SIZ_GENFIELD];
	char rHostname[_SIZ_MID] = { 0 };
	int ptpstatus;
	int pristatus;
	extern int transnet_down;

	TRC_ClrAlarm();

	if (transnet_down) {
		strcpy(ErrorMsg, "Transnet is down ");
		PRIReplyMCSFailure(ErrorMsg);
		return (0);
	}
	/*... Save host information ...*/
	strcpy(rHostname, message.host);
	if (strcmp(Cmd, "MCS_SEND_LOT_LOC") == 0) {
		/* Perform Get Dcvar List */
		PRIParseSetLotLocation_NVFEI(Carrierid, Amhsequipid, Category);
		strcpy(Number, "");
		ptpstatus = 1;
		if (TRC_ChkAlarm())
			PRIMethodCarrierEntered(Carrierid, Amhsequipid, Category, Number,
					&ptpstatus, ErrorMsg);
		strcpy(message.host, rHostname); /*... put back hostname ...*/
		if (ptpstatus)
			PRIReplyCmdack_NVFEI();
		if (!ptpstatus)
			PRIReplyPromisFailed(ErrorMsg);
	} else if (strcmp(Cmd, "MCS_CLR_LOT_LOC") == 0) {
		/* Perform Get Chart List */
		PRIParseCarrierid(Carrierid);
		ptpstatus = 1;
		if (TRC_ChkAlarm())
			PRIMethodGetLotLoc(Carrierid, Amhsequipid, Location, &ptpstatus,
					ErrorMsg);
		strcpy(message.host, rHostname); /*... put back hostname ...*/
		if (!ptpstatus)
			PRIReplyPromisFailed(ErrorMsg);
		else {
			if (TRC_ChkAlarm())
				PRIMethodCarrierRemoved(Carrierid, Amhsequipid, &ptpstatus,
						ErrorMsg);
			strcpy(message.host, rHostname); /*... put back hostname ...*/
			if (TRC_ChkAlarm())
				PRIReplyCmdack_NVFEI();
			else
				PRIReplyPromisFailed(ErrorMsg);
		}
	} else if (strcmp(Cmd, "MCS_GET_LOT_LOC") == 0) {
		/* Perform Get Chart List */
		PRIParseCarrierid(Carrierid);
		ptpstatus = 1;
		if (TRC_ChkAlarm())
			PRIMethodGetLotLoc(Carrierid, Amhsequipid, Location, &ptpstatus,
					ErrorMsg);
		strcpy(message.host, rHostname); /*... put back hostname ...*/
		if (ptpstatus)
			PRIReplyGetLotLoc(Amhsequipid, Location);
		if (!ptpstatus)
			PRIReplyPromisFailed(ErrorMsg);
	} else if (strcmp(Cmd, "MCS_MOVE_LOT_LOC") == 0) {
		/* Perform Get Chart List */
		PRIParseMoveLotLoc(UserID, Carrierid, Eqptid, Location, Priority); /* add UserID */
		if (strlen(Eqptid) == 0) //manage test lots' null locations Ivan 287364
			return -1;
		/* Location or category contains
		 1. |OUTPUT| - is a retrieve command
		 2. |PORT-A| - is a retrieve command
		 3. ||       - is a store (into stocker) command
		 4. |NONE|   - is a store (into stocker) command
		 */
		if (strlen(Location) > 0) {
			if (strcmp(Location, "OUTPUT") == 0)
				strcpy(Location, "PORT"); // set to default port(else parsed value)
			else if (strcmp(Location, "NONE") == 0)
				memset(Location, 0, sizeof(Location));
		}

		/**********************************************************************************************
		 ***  Changed the original pristatus=0 to -1 as doing that will send control to the false
		 ***  section of the if !pristatus. This is required as trc_chkalarm might be false in which
		 ***  case the PRIMethodMoveLotLoc is not called, and therefore there should not be a call to
		 ***  PRIReplyCmdack
		 ***
		 //       pristatus=0; Changed back again 010305
		 ***/

		pristatus = 0;

		if (TRC_ChkAlarm()) {
			if (!Fab3Stocker(Eqptid))  //PROMISLOC in tag data
				/*...Added UserId...*//*...and outputport (location)...*/
				/*...send to smp (_RET_SMPLOCATION)...*/
				PRIMethodMoveLotLoc(Carrierid, Eqptid, Location, UserID,
						Priority, &pristatus, ErrorMsg);
			else {
				/*...send to FAB3 (_RET_FAB3LOCATION)...*/
				FAB3MethodMove(Carrierid, Eqptid, Location, UserID, &pristatus,
						ErrorMsg);
				/*... The move above was for smp lot in fab3stocker. If success,
				 ... need to clear the location in SMP promis ...*/
				if (TRC_ChkAlarm())
					PRIMethodCarrierRemoved(Carrierid, Eqptid, &ptpstatus,
							ErrorMsg);
			}
		}
		strcpy(message.host, rHostname); /*... put back hostname ...*/
		if (TRC_ChkAlarm())
			PRIReplyCmdack_NVFEI();
		else {
			strcpy(ErrorMsg, TRC_ErrorMsg());
			if ((pristatus == 1) || (pristatus == 2))
				PRIReplyPromisFailed(ErrorMsg);
			else
				PRIReplyMCSFailure(ErrorMsg);
		}
	} else if (strcmp(Cmd, "MCS_SRVSHUT") == 0) {
		/* Perform Get Chart List */
		if (TRC_ChkAlarm())
			PRIMethodShutDown(ErrorMsg);
		strcpy(message.host, rHostname); /*... put back hostname ...*/
		if (strcmp(ErrorMsg, "") == 0)
			PRIReplyCmdack_NVFEI();
		else
			PRIReplyMCSFailure(ErrorMsg);
	} else {
		PRIParseMid(Mid);
		TRC_SetAlarm("PRIDispatch()", _RET_CMDINVALID, Cmd);
		strcpy(message.host, rHostname); /*... put back hostname ...*/
		PRIReplyCmdack(Mid);
		return (TRC_SetAlarm("PRIsrv received ", _RET_FAILURE, "UNKNOWN CMD"));
	}
	return (TRC_ChkAlarm());
}
