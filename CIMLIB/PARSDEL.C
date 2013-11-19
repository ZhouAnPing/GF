/******************************************************************************/
/*                                                                            */
/*   parsDEL.c:                                                               */
/*   ~~~~~~~~~~                                                               */
/*                                                                            */
/*   This file contains the functions required to parse delimited messages.   */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/* The following are the standard include statements.                         */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <varargs.h>


/* End of standard includes.                                                  */
/******************************************************************************/


/******************************************************************************/
/* The following are include statements for local files.                      */

#include "trcutil.h"
#include "talkutl.h"
#include "parsutl.h"

/* End of include statements for local files.                                 */
/******************************************************************************/


/*******************************************************************************

   PRS_DEL_InitCommand:                               Date:  June 1996
   ~~~~~~~~~~~~~~~~~~~~                             Author:  Sebastien Spicer

   This function initializes the message buffer for standard delimited style
   command.

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

int  PRS_DEL_InitCommand (char *command,
                         char *comment) {

  int status;

  message.msgType = _TLK_TYPDEL;
  strcpy (message.command, command);
  PRS_DEL_SetDelim (_PRS_PTP_DELIMIT);
  message.buffer[0] = _PRS_PTP_DELIMIT;
  message.msgLen = message.msgPtr = 1;
  if ((status = PRS_DEL_PutToken ("%s", command)) == _RET_SUCCESS)
    status = PRS_DEL_PutToken ("%s", comment);
  return (status);
}

/******************** End of function PRS_DEL_InitCommand *********************/


/*******************************************************************************

   PRS_DEL_PutToken:                                  Date:  June 1996
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
int  PRS_DEL_PutToken (format, va_alist)
     char *format;
     va_dcl 
#else
int  PRS_DEL_PutToken (char *format, ...)
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
    status = TRC_SetAlarm ("PRS_DEL_PutToken", _RET_FAILURE);

  } else if ((msgLen + message.msgLen + 2) > _SIZ_MSGBUFF) {
    status = TRC_SetAlarm ("PRS_DEL_PutToken", _RET_FLDTOOLONG,
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

/********************** End of function PRS_DEL_PutToken **********************/


/*******************************************************************************

   PRS_DEL_ChkStatus:                                 Date:  June 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function will return the status code of a reply.  It is assumed
   that 0 is success.  If the reply is not successful, the alarm will be
   set with the returned message.

      Arguments:
      ~~~~~~~~~~
         - success (type is *char - Read only):
              A pointer to a string which contains the success string for 
              the targeted server.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  PRS_DEL_ChkStatus (char *success) {
  
  char status[_SIZ_GENFIELD];
  char descr[_SIZ_GENFIELD];

  if (message.msgLen == 0)
    return (_RET_SUCCESS);

  PRS_DEL_SetDelim (message.buffer[0]);
  message.msgPtr = 1;

  if (TRC_ChkAlarm()) PRS_PTP_GetNext (status, _SIZ_GENFIELD);
  if (TRC_ChkAlarm()) PRS_PTP_GetNext (descr, _SIZ_GENFIELD);

  if (TRC_ChkAlarm()) 
    if (strcmp (status, success) != 0)
      return (TRC_SetAlarm ("PRS_DEL_ChkStatus", _RET_HOSTERROR, descr));

  return (TRC_GetAlarm());
}

/********************** End of function PRS_DEL_ChkStatus *********************/


/*******************************************************************************

   PRS_DEL_SetDelim:                                  Date:  June 1996
   ~~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function will take the given character and use it as a delimiter
   for subsequent parsing.

      Arguments:
      ~~~~~~~~~~
         - delimit (type is char - Read only):
              The value will be used as a delimitor for subsequent parsing.

      Returns:
      ~~~~~~~~
         Nothing.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...
           
*******************************************************************************/

int  PRS_DEL_SetDelim (char delimit) {

  message.delimit = delimit;
message.msgPtr = 0;
  return(0);
}

/********************* End of function PRS_DEL_SetDelim ***********************/


/*******************************************************************************

   PRS_DEL_GetNext:                                   Date:  April 1996
   ~~~~~~~~~~~~~~~~                                 Author:  Sebastien Spicer

   This function will retrieve the next field in the message buffer.

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

int  PRS_DEL_GetNext (void *data,
                     int  type) {

  char string[_SIZ_GENFIELD], temp[_SIZ_GENFIELD], *endPtr;
  struct tm fmtTime1, fmtTime2;
  int fieldLen;

  /*..........................................................................*/
  /* Flag error if at end of buffer...                                        */
  if ((fieldLen = message.msgLen - message.msgPtr) <= 0)
    return (TRC_SetAlarm ("PRS_DEL_GetNext", _RET_PARSEEND));

  /*..........................................................................*/
  /* We got the start of a token - find the end...                            */

  if ((endPtr = (char *) memchr (&message.buffer[message.msgPtr],
                                 (int) message.delimit, fieldLen)) == NULL)
    return (TRC_SetAlarm ("PRS_DEL_GetNext", _RET_PARSEEND));

  if ((fieldLen = endPtr - &message.buffer[message.msgPtr]) >= sizeof (string))
    return (TRC_SetAlarm ("PRS_DEL_GetNext", _RET_FLDTOOLONG,
                          "token", sizeof (string)));

  memcpy (string, &message.buffer[message.msgPtr], fieldLen);
  string[fieldLen] = 0;
  message.msgPtr += fieldLen + 1;

  /*..........................................................................*/
  /* If the token fits, wear it...                                            */

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
        return (TRC_SetAlarm ("PRS_DEL_GetNext", _RET_PARSEERR, string));
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
      return (TRC_SetAlarm ("PRS_DEL_GetNext", _RET_FAILURE));
      break;
    }
  return (_RET_SUCCESS);
}

/********************** End of function PRS_DEL_GetNext ***********************/


/*******************************************************************************

   PRS_DEL_LogOutMsg:                                 Date:  June 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function log an outgoing message to a standard delimited server.

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

void  PRS_DEL_LogOutMsg (char *logFile,
                        int  verbose) {

  char cmd[_SIZ_GENFIELD], field[_SIZ_GENFIELD];
  int status;

  if (TRC_GetVerb (logFile) < verbose) return;
  TRC_Send (logFile, verbose, _TRC_OUTSTR, TRC_GetTime (),
            message.msgLen, message.host);

  TRC_DisableAlarm (_RET_PARSEEND);
  PRS_DEL_SetDelim (message.buffer[0]);
  message.msgPtr = 1;
  if ((status = PRS_PTP_GetNext (cmd, _SIZ_GENFIELD)) == _RET_SUCCESS)
    status = PRS_PTP_GetNext (field, _SIZ_GENFIELD);
  
  if (status == _RET_SUCCESS)
    TRC_Send (logFile, verbose, "   Command: %s - %s\n", cmd, field);

  if (TRC_GetVerb (logFile) < (verbose + 1)) return;
  status = PRS_DEL_GetNext (field, _SIZ_GENFIELD);
  while (status == _RET_SUCCESS) {
    TRC_Send (logFile, verbose, "      -> %s\n", field);
    status = PRS_DEL_GetNext (field, _SIZ_GENFIELD);
  }
  TRC_EnableAlarm (_RET_PARSEEND);
  
  return;
}

/********************* End of function PRS_DEL_LogOutMsg **********************/


/*******************************************************************************

   PRS_DEL_LogInpMsg:                                 Date:  June 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function log an incomming message from a standard delimited server.

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

void  PRS_DEL_LogInpMsg (char *logFile,
                        int  verbose) {

  char cmd[_SIZ_GENFIELD], field[_SIZ_GENFIELD];
  int status;

  if (TRC_GetVerb (logFile) < verbose) return;
  TRC_Send (logFile, verbose, _TRC_INPSTR, TRC_GetTime (),
            message.msgLen, message.host);

  TRC_DisableAlarm (_RET_PARSEEND);
  PRS_DEL_SetDelim (message.buffer[0]);
  message.msgPtr = 1;
  if ((status = PRS_PTP_GetNext (cmd, _SIZ_GENFIELD)) == _RET_SUCCESS)
    status = PRS_PTP_GetNext (field, _SIZ_GENFIELD);
  
  if (status == _RET_SUCCESS)
    TRC_Send (logFile, verbose, "   Status: %s - %s\n", cmd, field);

  if (TRC_GetVerb (logFile) < (verbose + 1)) return;
  status = PRS_DEL_GetNext (field, _SIZ_GENFIELD);
  while (status == _RET_SUCCESS) {
    TRC_Send (logFile, verbose, "      -> %s\n", field);
    status = PRS_DEL_GetNext (field, _SIZ_GENFIELD);
  }
  TRC_EnableAlarm (_RET_PARSEEND);
  
  return;
}

/********************* End of function PRS_DEL_LogInpMsg **********************/


/******************************************************************************/
/*************************** End of file parsDEL.c ****************************/
/******************************************************************************/





