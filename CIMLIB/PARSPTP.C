/******************************************************************************/
/*                                                                            */
/*   parsPTP.c:                                                               */
/*   ~~~~~~~~~~                                                               */
/*                                                                            */
/*   This file contains the functions required to parse Promis TP messages.   */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/* The following are the standard include statements.                         */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef WINNT
#include <stdarg.h>
#include <windows.h>
#define WINNTAPI FAR PASCAL
#else
#define WINNTAPI 
#include <varargs.h>
#endif



/* End of standard includes.                                                  */
/******************************************************************************/


/******************************************************************************/
/* The following are include statements for local files.                      */

#include "trcutil.h"
#include "talkutl.h"
#include "parsutl.h"

/* End of include statements for local files.                                 */
/******************************************************************************/

/*****VERSION*********************************************************/

static char PtpDelimiter = _PRS_PTP_DELIMIT ;


/*********************************************************************/

/*******************************************************************************

   PRS_PTP_InitCommand:                               Date:  February 1996
   ~~~~~~~~~~~~~~~~~~~~                             Author:  Sebastien Spicer

   This function initializes the message buffer for Promis TP style command.

      Arguments:
      ~~~~~~~~~~
         - command (type is *char - Read only):
              The referenced string contains the command.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int PRS_PTP_InitCommand (char *command) {

  message.msgType = _TLK_TYPPTP;
  strcpy (message.command, command);
  sprintf (message.buffer, _PRS_PTP_CMDSTR, command, _PRS_PTP_DELIMIT);
  message.msgPtr = strlen (message.buffer);
  if (_PRS_PTP_DELIMIT == 0)
    message.msgPtr++;
  message.msgLen = message.msgPtr;
  return (_RET_SUCCESS);
}

/******************** End of function PRS_PTP_InitCommand *********************/


/*******************************************************************************

   PRS_PTP_PutToken:                                  Date:  February 1996
   ~~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function will add a token to the message buffer.  The parameters
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

#ifndef WINNT
int  PRS_PTP_PutToken (format, va_alist)
     char *format;
     va_dcl 
#else
int  PRS_PTP_PutToken (char *format, ...)
#endif

       
{
  va_list argptr;
  int  msgLen;
  int  status;
  char hold[_SIZ_MSGBUFF];

  /*..........................................................................*/
  /* Copy the formated data into the hold string...                           */

#ifdef WINNT
  va_start ( argptr,format );
#else
  va_start(argptr);
#endif
  
  msgLen = vsprintf (hold, format, argptr);
  va_end (argptr);

  /*..........................................................................*/
  /* Check for potential errors...                                            */

  if (msgLen == EOF) {
    status = TRC_SetAlarm ("PRS_PTP_PutToken", _RET_FAILURE);

  } else if ((msgLen + message.msgLen + 2) > _SIZ_MSGBUFF) {
    status = TRC_SetAlarm ("PRS_PTP_PutToken", _RET_FLDTOOLONG,
                           "message buffer", _SIZ_MSGBUFF);

  /*..........................................................................*/
  /* All is clear, append to buffer...                                        */

  } else {
    strcpy (&message.buffer[message.msgPtr], hold);
    message.msgPtr += msgLen;
    message.buffer[message.msgPtr++] = _PRS_PTP_DELIMIT;
    message.buffer[message.msgPtr] = 0;
    message.msgLen = message.msgPtr;
    status = _RET_SUCCESS;
  }

  return (status);
}

/********************** End of function PRS_PTP_PutToken **********************/


/*******************************************************************************

   PRS_PTP_ChkStatus:                                 Date:  February 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function will return the status code of a reply.  It is assumed
   that 0 is success.  If the reply is not successful, the alarm will be
   set with the returned message.

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

int  PRS_PTP_ChkStatus (void) {
  
  char statStr[_PRS_PTP_STALEN+1];
  char field[_SIZ_GENFIELD];

  if (message.buffer[0] == 0)
    return (_RET_SUCCESS);

  strncpy (statStr, &message.buffer[_PRS_PTP_STASTRT], _PRS_PTP_STALEN);
  statStr[_PRS_PTP_STALEN] = 0;
  
  if (strcmp (statStr, _PRS_PTP_SUCMSG) != 0) {
    PRS_PTP_GetFirst (field, _SIZ_GENFIELD);
    PRS_PTP_GetNext (field, _SIZ_GENFIELD);
    return (TRC_SetAlarm ("PRS_PTP_ChkStatus", _RET_HOSTERROR, field));
  } else
    return (_RET_SUCCESS);
}

/********************** End of function PRS_PTP_ChkStatus *********************/


/*******************************************************************************

   PRS_PTP_GetStatus:                                 Date:  April 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function will return the status string of a reply.

      Arguments:
      ~~~~~~~~~~
         None.

      Returns:
      ~~~~~~~~
         Type is *char - pointer to string with status code.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

char * PRS_PTP_GetStatus (void) {
  
  static char statStr[_PRS_PTP_STALEN+1];
  char *endPtr;

  strncpy (statStr, &message.buffer[_PRS_PTP_STASTRT], _PRS_PTP_STALEN);
  statStr[_PRS_PTP_STALEN] = 0;
  endPtr = strchr (statStr, ' ');
  endPtr[0] = 0;
  return (statStr);
}

/********************** End of function PRS_PTP_GetStatus *********************/


/*******************************************************************************

   PRS_PTP_GetFirst:                                  Date:  March 1996
   ~~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function will retrieve the field field in the message buffer.

      Arguments:
      ~~~~~~~~~~
         - data (type is *void - Read/Write):
              The referenced data element is of the specified type.  Its
              contents will be loaded with data from the next field.

         - type (type is int - Read only):
              The coded integer represents the type of data.  If the value
              of type is greater than 0, the type is a string and the value
              of type represents the size of the string.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...
           
*******************************************************************************/

int  PRS_PTP_GetFirst (void *data,
                      int  type) {

  message.msgPtr = _PRS_PTP_MSGSTRT;
  return (PRS_PTP_GetNext (data, type));
}

/********************* End of function PRS_PTP_GetFirst ***********************/

static char* OldPtr;
void PRS_PTP_GetSubField (char *InStr,char Del,char *Value) 
{
   int i,j;
        int Length;
   
   if ( InStr!= (char*)NULL)
     OldPtr=InStr;
   

   if ( *(OldPtr) ==(char) NULL ) 
   {
      strcpy ( Value,"");
      return;
   }

   Length = strlen( OldPtr);

   for (i=0,j=0; i < Length ;  i++)
   {
     if ( *(OldPtr) == (char) NULL ) break;
     if ( *(OldPtr) == Del ) break;
     if ( *(OldPtr) == ' ')
     { OldPtr++;
       continue;
     }
     Value[j++]=*OldPtr;
     OldPtr++;
   }
   OldPtr++;
   
   Value[j]=(char)NULL;
  
   return; 
}

/*******************************************************************************

   PRS_PTP_GetNext:                                   Date:  March 1996
   ~~~~~~~~~~~~~~~~                                 Author:  Sebastien Spicer

   This function will retrieve the next field in the message buffer.
   A call to PRS_PTP_GetFirst is required to initialize the pointer.

      Arguments:
      ~~~~~~~~~~
         - data (type is *void - Read/Write):
              The referenced data element is of the specified type.  Its
              contents will be loaded with data from the next field.

         - type (type is int - Read only):
              The coded integer represents the type of data.  If the value
              of type is greater than 0, the type is a string and the value
              of type represents the size of the string.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  PRS_PTP_GetNext (void *data,
                     int  type) {

  char *endPtr, string[_SIZ_GENFIELD], temp[_SIZ_GENFIELD];
  int  fldLen;
  struct tm fmtTime1, fmtTime2;


  if ((fldLen = message.msgLen - message.msgPtr) <= 0) 
    return (TRC_SetAlarm ("PRS_PTP_GetNext", _RET_PARSEEND));

  //if ((endPtr = (char *) memchr (&message.buffer[message.msgPtr],
        //                       (int) _PRS_PTP_DELIMIT, fldLen)) == NULL)

  if ((endPtr = (char *) memchr (&message.buffer[message.msgPtr],
                                 (int) PtpDelimiter, fldLen)) == NULL)
    return (TRC_SetAlarm ("PRS_PTP_GetNext", _RET_PARSEEND));

  if ((fldLen = endPtr - &message.buffer[message.msgPtr]) >= _SIZ_GENFIELD)
    fldLen = _SIZ_GENFIELD - 1;

  memcpy (string, &message.buffer[message.msgPtr], fldLen);
  string[fldLen] = 0;
  message.msgPtr += fldLen + 1;

  if (type > 0)
    strncpy ((char *) data, string, type);
  else
    switch (type) {
    case _PRS_CHAR:
      *((char *) data) = string[0];
      break;
    case _PRS_INT:
      *((int *) data) = (int) atol (string);
      break;
    case _PRS_DOUBLE:
      *((double *) data) = (double) atol (string);
      break;
    case _PRS_TIME:
      temp[2] = 0;
      strncpy (temp, &string[0], 2);
      fmtTime1.tm_mday = atoi (temp);
      temp[3] = 0;
      strncpy (temp, &string[3], 3);
      if (strcmp (temp, "JAN") == 0) fmtTime1.tm_mon = 0;  else
      if (strcmp (temp, "FEB") == 0) fmtTime1.tm_mon = 1;  else
      if (strcmp (temp, "MAR") == 0) fmtTime1.tm_mon = 2;  else
      if (strcmp (temp, "APR") == 0) fmtTime1.tm_mon = 3;  else
      if (strcmp (temp, "MAY") == 0) fmtTime1.tm_mon = 4;  else
      if (strcmp (temp, "JUN") == 0) fmtTime1.tm_mon = 5;  else
      if (strcmp (temp, "JUL") == 0) fmtTime1.tm_mon = 6;  else
      if (strcmp (temp, "AUG") == 0) fmtTime1.tm_mon = 7;  else
      if (strcmp (temp, "SEP") == 0) fmtTime1.tm_mon = 8;  else
      if (strcmp (temp, "OCT") == 0) fmtTime1.tm_mon = 9;  else
      if (strcmp (temp, "NOV") == 0) fmtTime1.tm_mon = 10; else
      if (strcmp (temp, "DEC") == 0) fmtTime1.tm_mon = 11; else
        return (TRC_SetAlarm ("PRS_PTP_GetNext", _RET_PARSEERR, string));
      temp[4] = 0;
      strncpy (temp, &string[7], 4);
      fmtTime1.tm_year = atoi (temp) - 1900;
      temp[2] = 0;
      strncpy (temp, &string[12], 2);
      fmtTime1.tm_hour = atoi (temp);
      strncpy (temp, &string[15], 2);
      fmtTime1.tm_min = atoi (temp);
      strncpy (temp, &string[18], 2);
      fmtTime1.tm_sec = atoi (temp);
      fmtTime2 = fmtTime1;
      fmtTime1.tm_isdst = 1;
      fmtTime2.tm_isdst = 0;
      *((time_t *) data) = mktime (&fmtTime1);
      endPtr = ctime ((time_t *) data);
      if (strncmp (&string[12], &endPtr[11], 2) != 0)
        *((time_t *) data) = mktime (&fmtTime2);
      break; 
    default:
      return (TRC_SetAlarm ("PRS_PTP_GetNext", _RET_PARSEERR, string));
      break;
    }

  return (_RET_SUCCESS);
}

/********************** End of function PRS_PTP_GetNext ***********************/


/*******************************************************************************

   PRS_PTP_LogOutMsg:                                 Date:  April 1996
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

void  PRS_PTP_LogOutMsg (char *logFile,
                        int  verbose) {

  char field[_SIZ_GENFIELD];
  int status;

  if (TRC_GetVerb (logFile) < verbose) return;
  TRC_Send (logFile, verbose, _TRC_OUTSTR, TRC_GetTime (),
            message.msgLen, message.host);
  TRC_Send (logFile, verbose, "   Command: %s\n", PRS_PTP_GetStatus ());

  if (TRC_GetVerb (logFile) < (verbose + 1)) return;
  TRC_DisableAlarm (_RET_PARSEEND);
  status = PRS_PTP_GetFirst (field, _SIZ_GENFIELD);
  while (status == _RET_SUCCESS) {
    TRC_Send (logFile, verbose, "      -> %s\n", field);
    status = PRS_PTP_GetNext (field, _SIZ_GENFIELD);
  }
  TRC_EnableAlarm (_RET_PARSEEND);
  
  return;
}

/********************* End of function PRS_PTP_LogOutMsg **********************/


/*******************************************************************************

   PRS_PTP_LogInpMsg:                                 Date:  April 1996
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

void  PRS_PTP_LogInpMsg (char *logFile,
                        int  verbose) {

  char field[_SIZ_GENFIELD];
  int status;

  if (TRC_GetVerb (logFile) < verbose) return;
  TRC_Send (logFile, verbose, _TRC_INPSTR, TRC_GetTime (),
            message.msgLen, message.host);
  TRC_Send (logFile, verbose, "   Status: %s\n", PRS_PTP_GetStatus ());

  if (TRC_GetVerb (logFile) < (verbose + 1)) return;
  TRC_DisableAlarm (_RET_PARSEEND);
  status = PRS_PTP_GetFirst (field, _SIZ_GENFIELD);
  while (status == _RET_SUCCESS) {
    TRC_Send (logFile, verbose, "      -> %s\n", field);
    status = PRS_PTP_GetNext (field, _SIZ_GENFIELD);
  }
  TRC_EnableAlarm (_RET_PARSEEND);
  
  return;
}

void PRS_PTP_SetDelimiter (char Value)
{
  PtpDelimiter=Value;
}
