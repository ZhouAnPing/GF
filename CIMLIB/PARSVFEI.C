#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define WINNTAPI 


/* End of standard includes.                                                  */
/******************************************************************************/


/******************************************************************************/
/* The following are include statements for local files.                      */

#include "cimdefs.h"
#include "trcutil.h"
#include "talkutl.h"
#include "parsutl.h"


/* End of include statements for local files.                                 */
/******************************************************************************/

/*****VERSION*********************************************************/

static char parsVFEIcid[] = "$Id: parsVFEI.c Ver 01.0 Rel 01.0 $";

/*********************************************************************/

/*******************************************************************************

   PRS_VFI_InitCommand:                               Date:  April 1996
   ~~~~~~~~~~~~~~~~~~~~                             Author:  Sebastien Spicer

   This function initializes the message buffer for a VFEI style command.

      Arguments:
      ~~~~~~~~~~
         - command (type is *char - Read only):
              The referenced string contains the command.

         - machine (type is *char - Read only):
              The referenced string contains the machine identifier.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int PRS_VFI_InitCommand (char *command,char *machine) 
{

  static unsigned int tid = 1;
  message.msgType = _TLK_TYPVFEI;
  if (tid > _PRS_VFI_MAXTID)
    tid = 1;
  strcpy (message.command, command);
  message.buffer[0] = 0;
  message.msgPtr = 0;
  message.msgLen = 1;
  message.msgSeq = tid;

  PRS_VFI_PutToken (_PRS_VFI_CMDID, _SIZ_GENFIELD, command);
  if (TRC_ChkAlarm()) PRS_VFI_PutToken (_PRS_VFI_MID, _SIZ_MID, machine);
  if (TRC_ChkAlarm()) PRS_VFI_PutToken (_PRS_VFI_TID, _PRS_UINT, &tid);
  if (TRC_ChkAlarm()) PRS_VFI_PutToken (_PRS_VFI_MTY, _SIZ_GENFIELD, "C");

  if (TRC_ChkAlarm()) tid++;
  return TRC_GetAlarm();
}
  


/******************** End of function PRS_VFI_InitCommand *********************/


/*******************************************************************************

   PRS_VFI_InitEvent:                               Date:    Oct 1997
   ~~~~~~~~~~~~~~~~~~                               Author:  Bala

   This function initializes the message buffer for a VFEI style command.

      Arguments:
      ~~~~~~~~~~
         - Event(type is *char - Read only):
              The referenced string contains the Event Name.

         - machine (type is *char - Read only):
              The referenced string contains the machine identifier.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int PRS_VFI_InitEvent (char *Event,char *machine) 
{

  static unsigned int tid = 1;
  message.msgType = _TLK_TYPVFEI;
  if (tid > _PRS_VFI_MAXTID)
    tid = 1;
  strcpy (message.command, Event);
  message.buffer[0] = 0;
  message.msgPtr = 0;
  message.msgLen = 1;
  message.msgSeq = tid;

  PRS_VFI_PutToken (_PRS_VFI_CMDID, _SIZ_GENFIELD, _EVENT_REPORT);
  PRS_VFI_PutToken (_PRS_VFI_EVENTID, _SIZ_GENFIELD, Event);
  if (TRC_ChkAlarm()) PRS_VFI_PutToken (_PRS_VFI_MID, _SIZ_MID, machine);
  if (TRC_ChkAlarm()) PRS_VFI_PutToken (_PRS_VFI_TID, _PRS_UINT, &tid);
  if (TRC_ChkAlarm()) PRS_VFI_PutToken (_PRS_VFI_MTY, _SIZ_GENFIELD, "E");
  if (TRC_ChkAlarm()) tid++;
  return TRC_GetAlarm();
}

/*******************************************************************************

   PRS_VFI_InitReply:                               Date:    Oct 1997
   ~~~~~~~~~~~~~~~~~~                               Author:  Bala

   This function initializes the message buffer for a VFEI style command.

      Arguments:
      ~~~~~~~~~~
         - Event(type is *char - Read only):
              The referenced string contains the Event Name.

         - machine (type is *char - Read only):
              The referenced string contains the machine identifier.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int PRS_VFI_InitReply (char *Event,char *machine) 
{

  int TempTid;
  message.msgType = _TLK_TYPVFEI;
  TempTid = message.seqToken;
  strcpy (message.command, Event);
  message.buffer[0] = 0;
  message.msgPtr = 0;
  message.msgLen = 1;

  PRS_VFI_PutToken (_PRS_VFI_CMDID, _SIZ_GENFIELD, Event);
  if (TRC_ChkAlarm()) PRS_VFI_PutToken (_PRS_VFI_MID, _SIZ_MID, machine);
  if (TRC_ChkAlarm()) PRS_VFI_PutToken (_PRS_VFI_TID, _PRS_UINT, &TempTid);
  if (TRC_ChkAlarm()) PRS_VFI_PutToken (_PRS_VFI_MTY, _SIZ_GENFIELD, "R");
  
  return TRC_GetAlarm();
}
  


/******************** End of function PRS_VFI_InitReply *********************/


/*******************************************************************************

   PRS_VFI_PutToken:                                  Date:  April 1996
   ~~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function will add a token to the VFEI message buffer.  The parameters
   received by the function are similar to printf.

      Arguments:
      ~~~~~~~~~~
         - format (type is *char - Read only):
              A pointer to a string which contains the format specifier for
              the message.

         Subsequent arguments are variable and follow the same style as those
         for printf.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  PRS_VFI_PutToken (char *vid,int type, void *data) 

{

  int msgLen;
  char value[_SIZ_GENFIELD];
  char tmpValue[_SIZ_GENFIELD];
  int status;

  tmpValue[0]='\0';
  if (type >= 0)
    strcpy(tmpValue,((char *) data));
  else
    switch (type) {
    case _PRS_UINT:
        case _PRS_UINT1:
        case _PRS_UINT2:
    case _PRS_BIN:
      sprintf(tmpValue,"%d",(*((unsigned *) data)) );
      break;
    case _PRS_INT:
      sprintf(tmpValue,"%d",(*((int *) data)));
      break;
        case _PRS_FLOAT:
      sprintf(tmpValue,"%e",(*((float *) data)));
      break;
    default:
      return (TRC_SetAlarm ("PRS_VFI_PutToken", _RET_PARSEERR, data));
   break;
    }


  /*......................................................................*/
  /* Check for potential errors...                                         */

  msgLen=strlen(tmpValue);
  if ((msgLen + message.msgLen + 2) > _SIZ_MSGBUFF) {
    status = TRC_SetAlarm ("PRS_VFI_PutToken", _RET_FLDTOOLONG,
                           "message buffer", _SIZ_MSGBUFF);
  }

 if (type >= 0)
    sprintf(value,"%s/A=\"%s\"",vid,tmpValue);
  else
    switch (type) {
    case _PRS_UINT:
      sprintf(value,"%s/U4=%s",vid,tmpValue);
      break;
        case _PRS_UINT1:
      sprintf(value,"%s/U1=%s",vid,tmpValue);
      break;
        case _PRS_UINT2:
      sprintf(value,"%s/U2=%s",vid,tmpValue);
      break;
    case _PRS_BIN:
      sprintf(value,"%s/B1=%s",vid,tmpValue);
      break;
    case _PRS_INT:
      sprintf(value,"%s/I4=%s",vid,tmpValue);
      break;
    case _PRS_FLOAT:
      sprintf(value,"%s/F4=%s",vid,tmpValue);
      break;

    default:
      return (TRC_SetAlarm ("PRS_VFI_PutToken", _RET_PARSEERR, data));
      break;
    }


 /*.............................................................*/
    /* All is clear, append to buffer...                            */

  if (message.msgPtr != 0)
      message.buffer[message.msgPtr++] = _PRS_VFI_DELIMIT;
      strcpy (&message.buffer[message.msgPtr], value);
      msgLen=strlen(value);
      message.msgPtr += msgLen;
      message.buffer[message.msgPtr] = 0;
      message.msgLen = message.msgPtr + 1;
      status = _RET_SUCCESS;
  return (status);
}


/********************** End of function PRS_VFI_PutToken **********************/


/*******************************************************************************

   PRS_VFI_ChkStatus:                                 Date:  April 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function will return the status code of a reply.  It is assumed
   that 0 is success.  If the reply is not successful, the alarm will be
   set with the returned message.  This command also verifies if the
   reply is of the proper sequence number.

      Arguments:
      ~~~~~~~~~~
         None.
 
      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  PRS_VFI_ChkStatus (void) {
  
  char errorTxt[_SIZ_GENFIELD];
  int errorCde, status;

  if (message.buffer[0] == 0)
    return (_RET_SUCCESS);

  status = PRS_VFI_GetToken ("ECD", _PRS_UINT, &errorCde);
  if (status != _RET_SUCCESS)
    return (status);

  if (errorCde != 0) {
    status = PRS_VFI_GetToken ("ETX", _SIZ_GENFIELD, errorTxt);
    if (status != _RET_SUCCESS)
      return (status);
    return (TRC_SetAlarm ("PRS_VFI_ChkStatus", _RET_HOSTERROR, errorTxt));
  }

  return (_RET_SUCCESS);
}

/********************** End of function PRS_VFI_ChkStatyus *********************/


/*******************************************************************************

   PRS_VFI_ChkMsgSeq:                                 Date:  April 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function will return based on parsing the message for sequence
   number.  One of the error codes may be that the function is out of 
   sequence.

      Arguments:
      ~~~~~~~~~~
         None.
 
      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  PRS_VFI_ChkMsgSeq (void) {
  
  
  int msgTID, status;

  status = PRS_VFI_GetToken ("TID", _PRS_UINT, &msgTID);
  if (status != _RET_SUCCESS)
    return (status);

  if (msgTID != message.msgSeq)
    return (TRC_SetAlarm ("PRS_VFI_ChkMsgSeq", _RET_PARSESEQ,
                          msgTID, message.msgSeq));
  
  return (_RET_SUCCESS);
}

/********************** End of function PRS_VFI_ChkMsgSeq *********************/
/*******************************************************************************

   PRS_VFI_ChkInpMsg:                                 Date:  April 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer


*******************************************************************************/

int  PRS_VFI_ChkInpMsg (void) {
  
  
  int msgTID, status;

 
  status = PRS_VFI_GetToken (_PRS_VFI_TID, _PRS_UINT ,&msgTID );
  if (status != _RET_SUCCESS)
    return (status);

  message.seqToken=msgTID;

 
  return (_RET_SUCCESS);
}

/********************** End of function PRS_VFI_ChkMsgSeq *********************/


/*******************************************************************************

   PRS_VFI_GetToken:                                  Date:  April 1996
   ~~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function will retrieve the data portion of the field associated with
   the given VID.

      Arguments:
      ~~~~~~~~~~
         - vid (type is *char - Read only):
              The referenced string represents the VID to be retrieved.

         - type (type is int - Read only):
              The coded integer represents the type of data.  If the value
              of type is greater than 0, the type is a string and the value
              of type represents the size of the string.

         - data (type is *void - Read/Write):
              The referenced data element is of the specified type.  Its
              contents will be loaded with data from the next field.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...
           
*******************************************************************************/

int  PRS_VFI_GetToken (char *vid,
                      int  type,
                      void *data) {

  char token[_SIZ_MSGBUFF], *pattern, *value;

  message.msgPtr = 0;
  pattern = PRS_VFI_SetPattern (vid, type);
  if (pattern[0] == 0)
    return (TRC_SetAlarm ("PRS_VFI_GetToken", _RET_RECNOTFND, vid));


  while (PRS_VFI_GetNext (token, _SIZ_MSGBUFF) == _RET_SUCCESS) {
    if (strncmp (token, pattern, strlen (pattern)) == 0) {
      if ((value = strchr (token, '=')) == NULL)
        return (TRC_SetAlarm ("PRS_VFI_GetToken", _RET_RECNOTFND, vid));
      return (PRS_VFI_SetToken (&value[1], type, data));
    }
  }
  return (TRC_SetAlarm ("PRS_VFI_GetToken", _RET_RECNOTFND, vid));
}

/********************* End of function PRS_VFI_GetToken ***********************/

/*******************************************************************************

   PRS_VFI_GetArray:                                  Date:  April 1996
   ~~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function will retrieve the data portion of the array associated with
   the given VID.

      Arguments:
      ~~~~~~~~~~
         - vid (type is *char - Read only):
              The referenced string represents the VID to be retrieved.

         - maxElem (type is int - Read only):
              The numeric value represents the maximum number of element that
              are defined in the array.

         - type (type is int - Read only):
              The coded integer represents the type of data in the array.  If
              the value of type is greater than 0, the type is a string and
              the value of type represents the maximum size of each string
              element in the array.

         - numElem (type is *int - Read/Write):
              The referenced integer will be loaded with the number of elements
              retrieved from the VFEI message.

         - data (type is *void - Read/Write):
              The referenced data element is of the specified type.  Its
              contents will be loaded with data from the next field.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...
           
*******************************************************************************/

int PRS_VFI_GetArray (char *vid,
                      int  maxElem,
                      int  type,
                      int  *numElem,
                      void *data) {

  char array[_SIZ_MSGBUFF], *strPtr;
  int parse, length;
  int status;
  
  /*..........................................................................*/
  /* Get the array of data from the message...                                */
  
  if ((status = PRS_VFI_GetToken (vid, _SIZ_MSGBUFF, array)) != _RET_SUCCESS)
    return (status);
  length = strlen (array);
  
 /*..........................................................................*/
  /* Process the entire array...                                              */
  
  parse = 0;
  *numElem = 0;
  while (parse < length) {

    /*........................................................................*/
    /* Get rid of leading blanks and assign start of token...                 */
  
    while (isspace (array[parse]) != 0)
      if (++parse >= length)
        return (_RET_SUCCESS);
    strPtr = &array[parse];


    /*........................................................................*/
    /* We got the start of a token - find the end...                          */

    if (*numElem >= maxElem)
      return (TRC_SetAlarm ("PRS_VFI_GetArray", _RET_PARSEERR, strPtr));
    while (1 == 1) {
      if (isspace (array[parse]) != 0)
        break;
      else if (array[parse] == '"')
        while ((++parse < length) && (array[parse] != '"'));
      if (++parse >= length) 
        break;
    }

    /*........................................................................*/
    /* Assign the token and NULL terminate it...                              */

    array[parse] = 0;
    if (type > 0)
      status = PRS_VFI_SetToken (strPtr, type,
                                 &((char *) data)[(*numElem) * type]);
    else
      switch (type) {
      case _PRS_UINT:
      case _PRS_BIN:
        status = PRS_VFI_SetToken (strPtr, type,
                                   &((unsigned *) data)[(*numElem)]);
        break;
      case _PRS_INT:
        status = PRS_VFI_SetToken (strPtr, type,
                                   &((int *) data)[(*numElem)]);
        break;
      case _PRS_FLOAT:
        status = PRS_VFI_SetToken (strPtr, type,
                                   &((float *) data)[(*numElem)]);
        break;
      default:
        return (TRC_SetAlarm ("PRS_VFI_GetArray", _RET_PARSEERR, array));
        break;
      }
    if (status != _RET_SUCCESS)
      return (status);
    (*numElem)++;
    parse++;
  }

  return (_RET_SUCCESS);
}

/********************* End of function PRS_VFI_GetArray ***********************/


/*******************************************************************************

   PRS_VFI_PutArray:                                  Date:  Sep1997
   ~~~~~~~~~~~~~~~~~                                  Author:  Bala

   This function will retrieve the data portion of the array associated with
   the given VID.

      Arguments:
      ~~~~~~~~~~
         - vid (type is *char - Read only):
              The referenced string represents the VID to be retrieved.

         - type (type is int - Read only):
              The coded integer represents the type of data in the array.  If
              the value of type is greater than 0, the type is a string and
              the value of type represents the maximum size of each string
              element in the array.

         - numElem (type is int - Read):
              The referenced integer will be loaded with the number of elements
              retrieved from the VFEI message.

         - data (type is *void - Read):
              The referenced data element is of the specified type.  Its
              contents will be loaded with data from the next field.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*****************************************************************************/
int  PRS_VFI_PutArray (char *vid,
                      int  type,
                      int  numElem,
                      void *data) {

  char data_str[_SIZ_MSGBUFF];
  char tmpstr[_SIZ_MSGBUFF];
  int tmpvar;
  int i;
  char dataval[_SIZ_GENFIELD];
  int msgLen;
        
        
  /*........................................................................*/
  /* Assign the token and NULL terminate it...                              */

  data_str[0] = '\0';
  if (type > 0)
    {
      sprintf(data_str,"%s/A[%d]=",vid,numElem);
      strcat(data_str,"[");
      for (i=0; i<numElem; i++)
        {
          if (i==0)
            strcat(data_str,"\"");
           else
            strcat(data_str," \"");

          /**** Get the Data Value ***/
          strncpy ( dataval,
                   &((char *) data)[i * type], type);
          strcat(data_str,dataval);
          strcat(data_str,"\"");
          /* printf (" Data %s\n", (char *) data ); */

        }
      strcat(data_str,"] ");
          
     /* printf (" DataStr %s\n",data_str); */
    }
  else
    {
      switch (type) {
      case _PRS_UINT:
        sprintf(data_str,"%s/U4[%d]=",vid,numElem);
        strcat(data_str,"[");
        tmpstr[0]='\0';
        tmpvar=0;
        for (i=0; i<numElem; i++)
          {
            sprintf(&tmpstr[tmpvar]," %d",
                    ((unsigned *) data)[i]);
            tmpvar=strlen(tmpstr);
          }
        strcat(data_str,tmpstr);
        strcat(data_str,"] ");
                break;

    case _PRS_FLOAT:
        sprintf(data_str,"%s/F4[%d]=",vid,numElem);
        strcat(data_str,"[");
        tmpstr[0]='\0';
        tmpvar=0;
        for (i=0; i<numElem; i++)
          {

            sprintf(&tmpstr[tmpvar]," %e",
                    ((float *) data)[i]);
            tmpvar=strlen(tmpstr);
          }
        strcat(data_str,tmpstr);
        strcat(data_str,"] ");
        break;

      case _PRS_BIN:
        sprintf(data_str,"%s/B4[%d]=",vid,numElem);
        strcat(data_str,"[");
        tmpstr[0]='\0';
        tmpvar=0;
        for (i=0; i<numElem; i++)
          {
            sprintf(&tmpstr[tmpvar]," %d",
                    ((unsigned *) data)[i]);
            tmpvar=strlen(tmpstr);
          }
        strcat(data_str,tmpstr);
        strcat(data_str,"] ");
        break;

      case _PRS_INT:
        sprintf(data_str,"%s/I4[%d]=",vid,numElem);
        strcat(data_str,"[");
        tmpstr[0]='\0';
        tmpvar=0;
        for (i=0; i<numElem; i++)
          {

            sprintf(&tmpstr[tmpvar]," %d",
                    ((int *) data)[i]);
            tmpvar=strlen(tmpstr);
          }
        strcat(data_str,tmpstr);
        strcat(data_str,"] ");
        break;
      default:
        return (TRC_SetAlarm ("PRS_VFI_PutArray", _RET_PARSEERR, data));
        break;
                
      }
    }


    /*.............................................................*/
    /* All is clear, append to buffer...                            */

  if (message.msgPtr != 0)
      message.buffer[message.msgPtr++] = _PRS_VFI_DELIMIT;
  strcpy (&message.buffer[message.msgPtr], data_str);
  msgLen=strlen(data_str);
  message.msgPtr += msgLen;
  message.buffer[message.msgPtr] = 0;    
  message.msgLen = message.msgPtr + 1;
  return (TRC_GetAlarm());
}

/********************* End of function PRS_VFI_GetArray *********************

/*******************************************************************************

   PRS_VFI_SetPattern:                                Date:  April 1996
   ~~~~~~~~~~~~~~~~~~~                              Author:  Sebastien Spicer

   This function will establish the search pattern as a function of the VID
   name and the VID type.

      Arguments:
      ~~~~~~~~~~
         - vid (type is *char - Read only):
              The referenced string represents the VID to be retrieved.

         - type (type is int - Read only):
              The coded integer represents the type of data.  If the value
              of type is greater than 0, the type is a string and the value
              of type represents the size of the string.

      Returns:
      ~~~~~~~~
         Type is *char - pointer to string containing the search pattern.  The
         string is a static variable which will be modified with subsequent
         calls.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...
           
*******************************************************************************/

char *PRS_VFI_SetPattern (char *vid,
                          int  type) {

  static char pattern[_SIZ_GENFIELD];

  if (type > 0)
    sprintf (pattern, "%.*s/", _SIZ_GENFIELD - 2, vid);
  else
    switch (type) {
    case _PRS_UINT:
      sprintf (pattern, "%.*s/U", _SIZ_GENFIELD - 3, vid);
      break;
    case _PRS_BIN:
      sprintf (pattern, "%.*s/B", _SIZ_GENFIELD - 3, vid);
      break;
    case _PRS_INT:
      sprintf (pattern, "%.*s/I", _SIZ_GENFIELD - 3, vid);
      break;
    case _PRS_FLOAT:
      sprintf (pattern, "%.*s/F", _SIZ_GENFIELD - 3, vid);
      break;
    default:
      pattern[0] = 0;
      break;
    }

  return (pattern);
}

/******************** End of function PRS_VFI_SetPattern **********************/


/*******************************************************************************

   PRS_VFI_SetToken:                                  Date:  April 1996
   ~~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function will convert a string to the specified type.  It is assumed
   that the string comes from VFEI.  If a string to string conversion is
   required, the enclosing quotes ("") or brackets ([]) will be removed.

      Arguments:
      ~~~~~~~~~~
         - source (type is *char - Read only):
              The referenced string contains the data to be converted.

         - type (type is int - Read only):
              The coded integer represents the type of data.  If the value
              of type is greater than 0, the type is a string and the value
              of type represents the size of the string.

         - data (type is *void - Read/Write):
              The referenced data element is of the specified type.  Its
              contents will be loaded with data from the next field.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...
           
*******************************************************************************/

int  PRS_VFI_SetToken (char *source,
                      int  type,
                      void *data) {

  int length;

  length = strlen (source);
  if (length == 0)
    return (TRC_SetAlarm ("PRS_VFI_SetToken", _RET_PARSEERR, source));

  if (type > 0) {
    if ((length > 1) &&
        (((source[0] == '[') && (source[length - 1] == ']')) ||
         ((source[0] == '"') && (source[length - 1] == '"')))) {
      if ((length - 2) >= type)
        return (TRC_SetAlarm ("PRS_VFI_SetToken", _RET_PARSEERR, source));
      strncpy ((char *) data, &source[1], length - 2);
      ((char *) data)[length - 2] = '\0';
    } else {
      if (length >= type)
        return (TRC_SetAlarm ("PRS_VFI_SetToken", _RET_PARSEERR, source));
      strcpy ((char *) data, source);
    }
  } else
    switch (type) {
    case _PRS_UINT:
    case _PRS_BIN:
      *((unsigned *) data) = (unsigned) atol (source);
      break;
    case _PRS_INT:
      *((int *) data) = (int) atol (source);
      break;
    case _PRS_FLOAT:
      sscanf(source,"%e",data);

 /*     *(data) = (float) atof (source);  */
      break;

    default:
      return (TRC_SetAlarm ("PRS_VFI_SetToken", _RET_PARSEERR, source));
      break;
    }
  
  return (_RET_SUCCESS);
}

/********************* End of function PRS_VFI_SetToken ***********************/


/*******************************************************************************

   PRS_VFI_GetNext:                                   Date:  April 1996
   ~~~~~~~~~~~~~~~~                                 Author:  Sebastien Spicer

   This function will retrieve the next VFEI field in the message buffer.
   It is assumed that the message pointer has been initialized.

      Arguments:
      ~~~~~~~~~~
         - data (type is *char - Read/Write):
              The referenced data element will be loaded with the next token.

         - size (type is int - Read only):
              The integer represents the maximum number of bytes that data
              can support.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int PRS_VFI_GetNext (char *data,
                     int  size) {

  int parse, tokSize;

  /*..........................................................................*/
  /* Check if given size is appropriate...                                    */
  if (size <= 0)
    return (_RET_PARSEERR);
  if (message.msgPtr >= message.msgLen)
    return (_RET_PARSEERR);
  /*..........................................................................*/
  /* Get rid of leading blanks...                                             */
  
  while (isspace (message.buffer[message.msgPtr]) != 0)
    if (++message.msgPtr >= message.msgLen)
      return (_RET_PARSEERR);
  
  /*..........................................................................*/
  /* We got the start of a token - find the end...                            */

  parse = message.msgPtr;
  while (1 == 1) {
    if (isspace (message.buffer[parse]) != 0)
      break;
    else if (message.buffer[parse] == '[')
      while ((++parse < message.msgLen) && (message.buffer[parse] != ']'));
    else if (message.buffer[parse] == '"')
      while ((++parse < message.msgLen) && (message.buffer[parse] != '"'));
    if (++parse >= message.msgLen) 
      break;
  }

  /*..........................................................................*/
  /* Assign the token and NULL terminate it...                                */

  tokSize = parse - message.msgPtr;
  if (tokSize >= size)
    return (_RET_PARSEERR);
  strncpy (data, &message.buffer[message.msgPtr], tokSize);
  data[tokSize] = 0;
  message.msgPtr = parse;
  return (_RET_SUCCESS);
}

/********************** End of function PRS_VFI_GetNext ***********************/


/*******************************************************************************

   PRS_VFI_LogOutMsg:                                 Date:  April 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function log an outgoing message to PROMIS TP.

      Arguments:
      ~~~~~~~~~~
         - logFile (type is *char - Read only):
              The referenced string contains the alias to be used for logging.

         - verbose (type is int - Read only):
              The integer value represents the verbose level to be used.

      Returns:
      ~~~~~~~~
         Nothing.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...
           
*******************************************************************************/

void  PRS_VFI_LogOutMsg (char *logFile,
                        int  verbose) {

  char field[_SIZ_LOGBLOCK + 1];
  int parse;

  if (TRC_GetVerb (logFile) < verbose) return;
  TRC_Send (logFile, verbose, _TRC_OUTSTR, TRC_GetTime (),
            message.msgLen, message.host);

  if (TRC_GetVerb (logFile) < (verbose + 1)) return;
  parse = 0;
  while (parse < message.msgLen) {
    strncpy (field, &message.buffer[parse], _SIZ_LOGBLOCK);
    field[_SIZ_LOGBLOCK] = 0;
    TRC_Send (logFile, verbose, "  >%-74s<\n", field);
    parse += _SIZ_LOGBLOCK;
  }
  
  return;
}

/********************* End of function PRS_VFI_LogOutMsg **********************/


/*******************************************************************************

   PRS_VFI_LogInpMsg:                                 Date:  April 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function log an incomming message to PROMIS TP.

      Arguments:
      ~~~~~~~~~~
         - logFile (type is *char - Read only):
              The referenced string contains the alias to be used for logging.

         - verbose (type is int - Read only):
              The integer value represents the verbose level to be used.

      Returns:
      ~~~~~~~~
         Nothing.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...
           
*******************************************************************************/

void PRS_VFI_LogInpMsg (char *logFile,
                        int  verbose) {

  char field[_SIZ_LOGBLOCK + 1];
  int parse;

  if (TRC_GetVerb (logFile) < verbose) return;
  TRC_Send (logFile, verbose, _TRC_INPSTR, TRC_GetTime (),
            message.msgLen, message.host);

  if (TRC_GetVerb (logFile) < (verbose + 1)) return;
  parse = 0;
  while (parse < message.msgLen) {
    strncpy (field, &message.buffer[parse], _SIZ_LOGBLOCK);
    field[_SIZ_LOGBLOCK] = 0;
    TRC_Send (logFile, verbose, "  >%-74s<\n", field);
    parse += _SIZ_LOGBLOCK;
  }
  
  return;
}



/************************************************************************************/
/*   The following portion has VB related TRC routines used for DLL                 */
/************************************************************************************/
/*******************************************************************************

   PRS_VFI_VB_InitCommand                                   Date:  Mar'98
   ~~~~~~~~~~~~                                    Author:  Jaya
  
      Arguments:
      
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/
#ifdef WINNT

int WINNTAPI PRS_VFI_VB_InitCommand (BSTR command,BSTR machine) 
{
        char locCommand[_SIZ_GENFIELD],
                 locMachine[_SIZ_GENFIELD];

        
        if ( ( command == NULL ) || ( machine ==NULL ) )
        {
                return ( TRC_SetAlarm ("PRS_VFI_InitCommand",_RET_GENERIC,
                        "NULL Arguments for command or machine "));
        }

        strcpy ( locCommand,(char *)command);
        strcpy ( locMachine,(char *)machine);

    return( PRS_VFI_InitCommand (locCommand, locMachine) );
}
/*******************************************************************************

   PRS_VFI_VB_InitEvent                                   Date:  Mar'98
   ~~~~~~~~~~~~                                    Author:  Jaya
  
      Arguments:
      
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/
int WINNTAPI PRS_VFI_VB_InitEvent (BSTR command,BSTR machine) 
{
        char locCommand[_SIZ_GENFIELD],
                 locMachine[_SIZ_GENFIELD];

        
        if ( ( command == NULL ) || ( machine ==NULL ) )
        {
                return ( TRC_SetAlarm ("PRS_VFI_InitEvent",_RET_GENERIC,
                        "NULL Arguments for command or machine "));
        }

        strcpy ( locCommand,(char *)command);
        strcpy ( locMachine,(char *)machine);

    return( PRS_VFI_InitEvent (locCommand, locMachine) );
}
/*******************************************************************************

   TRC_VB_InitReply                                  Date:  Mar'98
   ~~~~~~~~~~~~                                    Author:  Jaya
  
      Arguments:
      
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/
int WINNTAPI PRS_VFI_VB_InitReply (BSTR command,BSTR machine) 
{
        char locCommand[_SIZ_GENFIELD],
                 locMachine[_SIZ_GENFIELD];

        
        if ( ( command == NULL ) || ( machine ==NULL ) )
        {
                return ( TRC_SetAlarm ("PRS_VFI_InitReply",_RET_GENERIC,
                        "NULL Arguments for command or machine "));
        }

        strcpy ( locCommand,(char *)command);
        strcpy ( locMachine,(char *)machine);

    return( PRS_VFI_InitReply(locCommand, locMachine) );
}
/*******************************************************************************
                PRS_VFI_VB_PutTokenNum                          Apr 98 Jaya                                                                             
   
****************************************************************************/
           
int WINNTAPI PRS_VFI_VB_PutTokenNum( BSTR vid,int type, int Value)
{

        char locVid[_SIZ_GENFIELD];

        
        if ( ( vid == NULL ) )
        {
                return ( TRC_SetAlarm ("PRS_VFI_VB_PutTokenNum",_RET_GENERIC,
                        "NULL Arguments VID "));
        }

        strcpy ( locVid,(char *)vid);
        
    return( PRS_VFI_PutToken (locVid,type,&Value) ); 
}
/*******************************************************************************
                PRS_VFI_VB_PutTokenFloat                                Apr 98 Jaya                                                                             
   
****************************************************************************/
           
int WINNTAPI PRS_VFI_VB_PutTokenFloat( BSTR vid,int type, float Value)
{

        char locVid[_SIZ_GENFIELD];

        
        if ( ( vid == NULL ) )
        {
                return ( TRC_SetAlarm ("PRS_VFI_VB_PutTokenNum",_RET_GENERIC,
                        "NULL Arguments VID "));
        }

        strcpy ( locVid,(char *)vid);
        
    return( PRS_VFI_PutToken (locVid,type,&Value) ); 
}
/*******************************************************************************

   PRS_VFI_VB_PutTokenStr:                             Date:  Apr 98
   ~~~~~~~~~~~~~~~~~                                Author:   Jaya

   
****************************************************************************/
           
int WINNTAPI PRS_VFI_VB_PutTokenStr( BSTR vid,int type, BSTR Value)
{

        char locVid[_SIZ_GENFIELD],locValue[_SIZ_MSGBUFF];

        
        if ( ( vid == NULL )|| ( Value==NULL) )
        {
                return ( TRC_SetAlarm ("PRS_VFI_VB_PutTokenStr",_RET_GENERIC,
                        "NULL Arguments VID or Value"));
        }

        strcpy ( locVid,(char *)vid);
        strcpy ( locValue,(char *)Value);
        
    return( PRS_VFI_PutToken (locVid,type,locValue) ); 
}
/*******************************************************************************

   PRS_VFI_VB_GetTokenStr                             Date:  Apr 98
   ~~~~~~~~~~~~~~~~~                                Author:  Jaya

  
           
*******************************************************************************/

int WINNTAPI PRS_VFI_VB_GetTokenStr (BSTR vid, int  type,BSTR *data) 
{
        char locVid[_SIZ_GENFIELD],locValue[_SIZ_MSGBUFF];

        
        if (  vid == NULL  )
        {
                return ( TRC_SetAlarm ("PRS_VFI_VB_GetTokenStr",_RET_GENERIC,
                        "NULL Arguments VID "));
        }

        strcpy ( locVid,(char *)vid);
        
        
     PRS_VFI_GetToken (locVid,type,locValue);
         
        if ( TRC_ChkAlarm() )
                *data = SysAllocStringByteLen((char *)locValue,strlen(locValue) );
                
         return(TRC_GetAlarm());

  
}
/*******************************************************************************

   PRS_VFI_VB_GetTokenNum                           Date:   APr 98
   ~~~~~~~~~~~~~~~~~                                Author:  Jaya

  
           
*******************************************************************************/

int WINNTAPI PRS_VFI_VB_GetTokenNum (BSTR vid, int  type,int *data) 
{
        char locVid[_SIZ_GENFIELD];

        
        if (  vid == NULL  )
        {
                return ( TRC_SetAlarm ("PRS_VFI_VB_GetTokenNum",_RET_GENERIC,
                        "NULL Arguments VID "));
        }

        strcpy ( locVid,(char *)vid);
        
    return ( PRS_VFI_GetToken (locVid,type,data) );
                
         
  
}
/*******************************************************************************

   PRS_VFI_VB_GetTokenFloat                           Date:   APr 98
   ~~~~~~~~~~~~~~~~~                                Author:  Jaya

  
           
*******************************************************************************/

int WINNTAPI PRS_VFI_VB_GetTokenFloat (BSTR vid, int  type,float *data) 
{
        char locVid[_SIZ_GENFIELD];

        
        if (  vid == NULL  )
        {
                return ( TRC_SetAlarm ("PRS_VFI_VB_GetTokenNum",_RET_GENERIC,
                        "NULL Arguments VID "));
        }

        strcpy ( locVid,(char *)vid);
        
    return ( PRS_VFI_GetToken (locVid,type,data) );
                
         
  
}
/*******************************************************************************

   PRS_VFI_VB_PutArrayStr                          Date:      Apr 98
   ~~~~~~~~~~~~~~~~~                                Author:   Jaya

  
           
*******************************************************************************/

int WINNTAPI PRS_VFI_VB_PutArrayStr (BSTR vid,int type,int numelem,
                                     LPSAFEARRAY *array)
{
        long lbound,ubound; 
        int i,j;
        BSTR bstr; 
        char StrArray[_SIZ_GENFIELD][_SIZ_GENFIELD];
        
        char locVid[_SIZ_GENFIELD];

        
        if ( vid == NULL )
        {
                return ( TRC_SetAlarm ("PRS_VFI_VB_PutArrayStr",_RET_GENERIC,
                        "NULL Arguments for VID"));
        }

        strcpy ( locVid,(char *)vid);

        /* Start of code */

        SafeArrayGetLBound(*array,1,&lbound); 
        SafeArrayGetUBound(*array,1,&ubound);
  
        for (i=lbound,j=0;i<=ubound;i++)
        {
                if (SafeArrayGetElement(*array,&i,&bstr) != S_OK) 
                        break;
                strcpy ( StrArray[j],(char *)bstr);
                j++;

        }       
        return( PRS_VFI_PutArray (locVid, _SIZ_GENFIELD, numelem,StrArray));


}

/*******************************************************************************

   PRS_VFI_VB_PutArrayNum                           Date:    Apr 98
   ~~~~~~~~~~~~~~~~~                                Author:  jaya

  
           
*******************************************************************************/

int WINNTAPI PRS_VFI_VB_PutArrayNum (BSTR vid,int type,int numelem,
                                     LPSAFEARRAY *array)
{
        
        int i,j;

        long numArray[_SIZ_GENFIELD];
        
        int retval;
        long lbound,ubound; 
        char locVid[_SIZ_GENFIELD];

        
        if ( vid == NULL )
        {
                return ( TRC_SetAlarm ("PRS_VFI_VB_PutArrayNum",_RET_GENERIC,
                        "NULL Arguments for VID"));
        }

        strcpy ( locVid,(char *)vid);

        SafeArrayGetLBound(*array,1,&lbound); 
        SafeArrayGetUBound(*array,1,&ubound);
  
        for (i=lbound,j=0;i<=ubound;i++)
        {
                if (SafeArrayGetElement(*array,&i,&retval) != S_OK) 
                        break;
                numArray[j]=retval;
                j++;

        } 
        
        return( PRS_VFI_PutArray (locVid, type, numelem,numArray));

}




/*******************************************************************************

   PRS_VFI_VB_PutArrayNum                           Date:    Apr 98
   ~~~~~~~~~~~~~~~~~                                Author:  jaya

  
           
*******************************************************************************/

int WINNTAPI PRS_VFI_VB_PutArrayFloat (BSTR vid,int type,int numelem,
                                     LPSAFEARRAY *array)
{
        
        int i,j;

        float numArray[_SIZ_GENFIELD];
        
        float retval;
        long lbound,ubound; 
        char locVid[_SIZ_GENFIELD];

        
        if ( vid == NULL )
        {
                return ( TRC_SetAlarm ("PRS_VFI_VB_PutArrayNum",_RET_GENERIC,
                        "NULL Arguments for VID"));
        }

        strcpy ( locVid,(char *)vid);

        SafeArrayGetLBound(*array,1,&lbound); 
        SafeArrayGetUBound(*array,1,&ubound);
  
        for (i=lbound,j=0;i<=ubound;i++)
        {
                if (SafeArrayGetElement(*array,&i,&retval) != S_OK) 
                        break;
                numArray[j]=retval;
                j++;

        } 
        
        return( PRS_VFI_PutArray (locVid, type, numelem,numArray));

}







/*******************************************************************************

   PRS_VFI_VB_GetArrayStr:                             Date:  Oct 1997
   ~~~~~~~~~~~~~~~~~                                Author:  jaya

   
****************************************************************************/
int WINNTAPI PRS_VFI_VB_GetArrayStr (BSTR vid,int maxElem,int type,int *numelem,
                                     LPSAFEARRAY *array)
{
         
        int i;
        char StrArray[_SIZ_GENFIELD][_SIZ_GENFIELD];
        long i1[1];
        BSTR str1;
        char locVid[_SIZ_GENFIELD];

        
        if ( vid == NULL )
        {
                return ( TRC_SetAlarm ("PRS_VFI_VB_PutArrayStr",_RET_GENERIC,
                        "NULL Arguments for VID"));
        }
        strcpy ( locVid,(char *)vid);

        PRS_VFI_GetArray (locVid,maxElem,_SIZ_GENFIELD,numelem,StrArray);
        if ( TRC_ChkAlarm() )
        {
                for ( i=0;i<*numelem;i++)
                {
                        str1=SysAllocStringByteLen(StrArray[i],strlen(StrArray[i]) );
                        i1[0]=i; 
                        SafeArrayPutElement(*array, i1, str1);
                        SysFreeString(str1);
                        TRC_Send("LogFil",1,"ELEMENT %d = %s\n",i,StrArray[i]);
                }
                
        }       
        return(TRC_GetAlarm() );
        
}
/*******************************************************************************

   PRS_VFI_VB_GetArrayNum:                             Date:  Oct 1997
   ~~~~~~~~~~~~~~~~~                                Author:  jaya

   
****************************************************************************/
int WINNTAPI PRS_VFI_VB_GetArrayNum (BSTR vid, int maxelem,
                                                                         int type, int*numelem,
                                                                         LPSAFEARRAY *array){    
        int i;
        long i1[1];
        int data[_SIZ_GENFIELD];
        char locVid[_SIZ_GENFIELD];
        long retval;
        
        
        if ( vid == NULL )
        {
                return ( TRC_SetAlarm ("PRS_VFI_VB_PutArrayStr",_RET_GENERIC,
                        "NULL Arguments for VID"));
        }
        strcpy ( locVid,(char *)vid);

        PRS_VFI_GetArray (locVid,maxelem,type,numelem,data);
        if ( TRC_ChkAlarm() )
        {
                for ( i=0;i<*numelem;i++)
                {
                  retval=data[i];
                  i1[0]=i;      
                  SafeArrayPutElement(*array,i1,&retval);
                }
        }
        return(TRC_GetAlarm() );
}



/*******************************************************************************

   PRS_VFI_VB_GetArrayNum:                             Date:  Oct 1997
   ~~~~~~~~~~~~~~~~~                                Author:  jaya

   
****************************************************************************/
int WINNTAPI PRS_VFI_VB_GetArrayFloat (BSTR vid, int maxelem,
                                                                         int type, int*numelem,
                                                                         LPSAFEARRAY *array){    
        int i;
        long i1[1];
        int data[_SIZ_GENFIELD];
        char locVid[_SIZ_GENFIELD];
        long retval;
        
        
        if ( vid == NULL )
        {
                return ( TRC_SetAlarm ("PRS_VFI_VB_PutArrayStr",_RET_GENERIC,
                        "NULL Arguments for VID"));
        }
        strcpy ( locVid,(char *)vid);

        PRS_VFI_GetArray (locVid,maxelem,type,numelem,data);
        if ( TRC_ChkAlarm() )
        {
                for ( i=0;i<*numelem;i++)
                {
                  retval=data[i];
                  i1[0]=i;      
                  SafeArrayPutElement(*array,i1,&retval);
                }
        }
        return(TRC_GetAlarm() );
}

#endif
