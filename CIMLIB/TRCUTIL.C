/******************************************************************************/
/*                                                                            */
/*   trcutil.c:                                                               */
/*   ~~~~~~~~~~                                                               */
/*                                                                            */
/*   This file contains various functions to perform tracing.                 */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/* The following are the standard include statements.                         */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#ifdef WIN32
    #include <stdarg.h>
    #include <windows.h>
#else
    #include <varargs.h>
#endif


/* End of standard includes.                                                  */
/******************************************************************************/


/******************************************************************************/
/* The following are include statements for local files.                      */

#include "ordarray.h"
#include "talkutl.h"
#include "trcutil.h"

/* End of include statements for local files.                                 */
/******************************************************************************/

/*****VERSION*********************************************************/


/*********************************************************************/

/******************************************************************************/
/* Declaration of global variables with file scope.                           */

OAR_array *trcHandle = NULL;
OAR_array *alrHandle = NULL;
void (*shutdownFunc)();
char alrAlias[_SIZ_FILENAME]; 
char alrAppl[_SIZ_GENFIELD];
char alrFunc[_SIZ_GENFIELD];
char alrMesg[_SIZ_MSGBUFF];
char alrSevr;
int  alrCode;

/* End of global variable declarations...                                     */
/******************************************************************************/


/*******************************************************************************

   TRC_InitAlarm:                                     Date:  February 1996
   ~~~~~~~~~~~~~~                                   Author:  Sebastien Spicer

   This function will allow the calling application to initialize the alarm
   definition structure.

      Arguments:
      ~~~~~~~~~~
         - alias (type is *char - Read only):
              The referenced string is the alias to be used for output of
              alarms.

         - appName (type is *char - Read only):
              The referenced string is the applicaqtion name.

         - shutdown (type is *void - Read only):
              The referenced function will be called on shutdown.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int TRC_InitAlarm (char *alias,
                   char *appName,
                   void (*shutdown)()) {

  int status;

  /*..........................................................................*/
  /* Setup global variables...                                                */

  strcpy (alrAlias, alias);
  strcpy (alrAppl, appName);
  shutdownFunc = shutdown;
  TRC_ClrAlarm ();

  /*..........................................................................*/
  /* Populate with default list of alarm codes...                             */

  status = TRC_AddAlarm (_RET_SUCCESS, _TRC_WARN, _TRC_SUCCESS);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_SHUTDOWN, _TRC_FATAL, _TRC_SHUTDOWN);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_FAILURE, _TRC_WARN, _TRC_FAILURE);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_GENERIC, _TRC_WARN, _TRC_GENERIC);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_BADFILENAME, _TRC_WARN, _TRC_BADFILENAME);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_FILEOPENERR, _TRC_WARN, _TRC_FILEOPENERR);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_FILEWRITERR, _TRC_WARN, _TRC_FILEWRITERR);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_NOMEMORY, _TRC_FATAL, _TRC_NOMEMORY);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_FLDMISSING, _TRC_WARN, _TRC_FLDMISSING);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_FLDNOTXPCT, _TRC_WARN, _TRC_FLDNOTXPCT);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_FLDTOOLONG, _TRC_WARN, _TRC_FLDTOOLONG);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_FLDBADINTV, _TRC_WARN, _TRC_FLDBADINTV);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_FLDBADSTRV, _TRC_WARN, _TRC_FLDBADSTRV);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_BADMBOXNAME, _TRC_WARN, _TRC_BADMBOXNAME);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_MBOXNOTOPN, _TRC_WARN, _TRC_MBOXNOTOPN);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_MBOXNOTSND, _TRC_WARN, _TRC_MBOXNOTSND);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_MBOXNOTRCV, _TRC_WARN, _TRC_MBOXNOTRCV);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_TIMEOUT, _TRC_WARN, _TRC_TIMEOUT);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_HOSTERROR, _TRC_WARN, _TRC_HOSTERROR);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_PARSEERR, _TRC_WARN, _TRC_PARSEERR);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_PARSEEND, _TRC_WARN, _TRC_PARSEEND);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_PARSESEQ, _TRC_WARN, _TRC_PARSESEQ);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_RECNOTFND, _TRC_WARN, _TRC_RECNOTFND);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_RECEXISTS, _TRC_WARN, _TRC_RECEXISTS);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_DMQERROR, _TRC_WARN, _TRC_DMQERROR);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_CMDWRONG, _TRC_WARN, _TRC_CMDWRONG);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_CMDINVALID, _TRC_WARN, _TRC_CMDINVALID);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_NOARMID, _TRC_WARN, _TRC_NOARMID);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_BADARMID, _TRC_WARN, _TRC_BADARMID);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_LOAD_FAILURE, _TRC_WARN, _TRC_LOAD_FAILURE);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_TAGREADFAIL, _TRC_WARN, _TRC_TAGREADFAIL);
  if (status != _RET_SUCCESS) return (status);

  status = TRC_AddAlarm (_RET_TAGWRITEFAIL, _TRC_WARN, _TRC_TAGWRITEFAIL);
  if (status != _RET_SUCCESS) return (status);

  return (_RET_SUCCESS);
}

/*********************** End of function ADC_InitAlarm ************************/


/*******************************************************************************

   TRC_AddAlarm:                                      Date:  February 1996
   ~~~~~~~~~~~~~                                    Author:  Sebastien Spicer

   This function will allow the calling application to add an alarm definition
   to the list of recognized alarms.

      Arguments:
      ~~~~~~~~~~
         - alarmCode (type is int - Read only):
              The value of alarmCode defines the type of alarm being defined.

         - alarmSevr (type is char - Read only):
              The value of alarmSevr defines the severity of the alarm (use
              _ALR_FATAL, _ALR_WARN or _ALR_INFO).

         - alarmMesg (type is *char - Read only):
              The referenced string will contain the prototype of the message
              associated with the alarm.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TRC_AddAlarm (int  alarmCode,
                  char alarmSevr,
                  char *alarmMesg) {

  TRC_alrdef *alrRec;
  
  /*..........................................................................*/
  /* Check if alarm definition list created (create if required)...           */

  if (alrHandle == NULL)
    if ((alrHandle = OAR_Open (1, 1, (LP_locate) TRC_LocAlrm, (LP_compare) TRC_CmpAlrm)) == NULL)
      return (_RET_NOMEMORY);

  /*..........................................................................*/
  /* Get existing alarm, or create a new one...                               */

  if ((alrRec = OAR_GetElement (alrHandle, &alarmCode)) == NULL) {
    if ((alrRec = (TRC_alrdef *) malloc (sizeof (TRC_alrdef))) == NULL)
      return (_RET_NOMEMORY);
    memset (alrRec, 0, sizeof (TRC_alrdef));
    alrRec->code = alarmCode;
    if (OAR_AddElement (alrHandle, alrRec) == _OAR_INVALIDPOS) {
      free (alrRec);
      return (_RET_NOMEMORY);
    }
  }

  /*..........................................................................*/
  /* Set new values for the alarm...                                          */

  alrRec->severity = alarmSevr;
  strcpy (alrRec->message, alarmMesg);
  return (_RET_SUCCESS);
}

/************************ End of function ADC_AddAlarm ************************/


/*******************************************************************************

   TRC_SetAlarm:                                      Date:  February 1996
   ~~~~~~~~~~~~~                                    Author:  Sebastien Spicer

   This function will receive an error code from the calling application.
   It will set the current error fields accordingly.

      Arguments:
      ~~~~~~~~~~
         - funcName (type is *char - Read only):
              The referenced string contains the function name where the
              error occured.

         - errCode (type is int - Read only):
              The value of errCode defines the type of alarm being received.

         Subsequent arguments are variable and depend on code's value.

      Returns:
      ~~~~~~~~
         Type is int - the value of errCode is returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

#ifndef WIN32 
int  TRC_SetAlarm (funcName,  errCode, va_alist )
char *funcName;
int errCode;
va_dcl
#else
int  TRC_SetAlarm (char *funcName,int errCode, ... )
#endif

{
  TRC_alrdef *alrRec;
  va_list argptr;

  if ((alrRec = OAR_GetElement (alrHandle, &errCode)) == NULL) {
    strcpy (alrMesg, _TRC_FAILURE);
    alrSevr = _TRC_WARN;

  } else if (alrRec->disable != 0) {
    return (errCode);

  } else {
#ifndef WIN32
                va_start (argptr);
#else
                va_start (argptr, errCode);
#endif
    
    vsprintf (alrMesg, alrRec->message, argptr);
    va_end (argptr);
    alrSevr = alrRec->severity;
  }

  alrCode = errCode;
  strcpy (alrFunc, funcName);

  if (alrCode == _RET_SUCCESS) {
  } else if (alrCode == _RET_SHUTDOWN) {
    TRC_Send (alrAlias, _TRC_LVL_STATE, "%s: - %s exiting from %s: %s.\n",
              TRC_GetTime (), alrAppl, alrFunc, alrMesg);
    shutdownFunc ();
    exit (0);
  } else if (alrSevr == _TRC_FATAL) {
    TRC_Send (alrAlias, _TRC_LVL_STATE, "%s: Error %d in %s (%s): %s.\n",
              TRC_GetTime (), alrCode, alrAppl, alrFunc, alrMesg);
    shutdownFunc ();
    exit (1);
  } else if (alrSevr == _TRC_WARN) {
    TRC_Send (alrAlias, _TRC_LVL_STATE, "%s: Warning %d in %s (%s): %s.\n",
              TRC_GetTime (), alrCode, alrAppl, alrFunc, alrMesg);
  } else if (alrSevr == _TRC_INFO) {
    TRC_Send (alrAlias, _TRC_LVL_STATE, "%s: Inform %d in %s (%s): %s.\n",
              TRC_GetTime (), alrCode, alrAppl, alrFunc, alrMesg);
    TRC_ClrAlarm ();
  }
  return (errCode);
}

/************************ End of function TRC_SetAlarm ************************/


/*******************************************************************************

   TRC_ClrAlarm:                                      Date:  February 1996
   ~~~~~~~~~~~~~                                    Author:  Sebastien Spicer

   This function will clear the current alarm status.

      Arguments:
      ~~~~~~~~~~
         None.

      Returns:
      ~~~~~~~~
         Nothing.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void  TRC_ClrAlarm (void) {

  alrFunc[0] = 0;
  alrSevr = _TRC_INFO;
  strcpy (alrMesg, _TRC_SUCCESS);
  alrCode = _RET_SUCCESS;
  return;
}

/************************ End of function TRC_ClrAlarm ************************/


/*******************************************************************************

   TRC_EnableAlarm:                                   Date:  June 1996
   ~~~~~~~~~~~~~~~~                                 Author:  Sebastien Spicer

   This function will enable an alarm that was previously disabled.

      Arguments:
      ~~~~~~~~~~
         - alarmCode (type is int - Read only):
              The value of alarmCode defines the type of alarm being enabled.

      Returns:
      ~~~~~~~~
         Nothing.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void  TRC_EnableAlarm (int alarmCode) {

  TRC_alrdef *alrRec;

  if ((alrRec = OAR_GetElement (alrHandle, &alarmCode)) != NULL)
    alrRec->disable = 0;
  return;
}

/********************** End of function TRC_EnableAlarm ***********************/


/*******************************************************************************

   TRC_DisableAlarm:                                  Date:  June 1996
   ~~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function will disable an alarm.

      Arguments:
      ~~~~~~~~~~
         - alarmCode (type is int - Read only):
              The value of alarmCode defines the type of alarm being disabled.

      Returns:
      ~~~~~~~~
         Nothing.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void  TRC_DisableAlarm (int alarmCode) {

  TRC_alrdef *alrRec;

  if ((alrRec = OAR_GetElement (alrHandle, &alarmCode)) != NULL)
    alrRec->disable = 1;
  return;
}

/********************** End of function TRC_DisableAlarm **********************/


/*******************************************************************************

   TRC_Open:                                          Date:  November 1994
   ~~~~~~~~~                                        Author:  Sebastien Spicer

   This function will intialize the tracing utility.  It basically sets
   global variables based on the arguments and verifies that tracing can
   work with the given parameters.

   -Ivan Jun 2002-
   Added functionality to create new trace file every _TRC_NEXT_DAY seconds by 
   default.
   
      Arguments:
      ~~~~~~~~~~
         - alias (type is *char - Read only):
              The referenced string is the alias to be used for accessing
              the trace file.

         - name (type is *char - Read only):
              The referenced string is the file name to be used for tracing.
              The function accepts "stdout" and "stderr" as options.

         - verbose (type is int - Read only):
              The given integer is the verbose level to be used when tracing.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Sebastien Spicer           February 1996
           Added the alias concept and changed the function to handle a
           dynamic array of file handles.

*******************************************************************************/
int  TRC_Open (char *alias,
              char *name,
              int  verbose) {
  TRC_handle *trcRec;
  int status;

  /*..........................................................................*/
  /* Make sure that the given parameters are OK...                            */
  
  if ((strlen (name) >= _SIZ_FILENAME) || (name[0] == 0))
    return (TRC_SetAlarm ("TRC_Open", _RET_BADFILENAME, name));

  /*..........................................................................*/
  /* Check to see if the array of files is created (create if required)...    */

  if (trcHandle == NULL)
    if ((trcHandle = OAR_Open (1, 1, (LP_locate) TRC_LocFile, (LP_compare) TRC_CmpFile)) == NULL)
      return (TRC_SetAlarm ("TRC_Open", _RET_NOMEMORY,  ""));

  /*..........................................................................*/
  /* Get existing file handle, or create a new one...                         */

  if ((trcRec = OAR_GetElement (trcHandle, alias)) == NULL) {
    if ((trcRec = (TRC_handle *) malloc (sizeof (TRC_handle))) == NULL)
      return (TRC_SetAlarm ("TRC_Open", _RET_NOMEMORY, ""));
    memset (trcRec, 0, sizeof (TRC_handle));
    strcpy (trcRec->alias, alias);
    if (OAR_AddElement (trcHandle, trcRec) == _OAR_INVALIDPOS) {
      free (trcRec);
      return (TRC_SetAlarm ("TRC_Open", _RET_NOMEMORY, ""));
    }
  } else if ((trcRec->verbose == verbose) && (strcmp (trcRec->name, name) == 0))
    return (_RET_SUCCESS);

  /*..........................................................................*/
  /* Initialize the file handle for the trace utility...                      */
  strcpy (trcRec->name, name);
  trcRec->verbose = verbose;
  /*...Initilize to default values of new log every day...*/
  trcRec->nexOpenTime=time(NULL) + (time_t) _TRC_NEXT_DAY;
  trcRec->chgTime=_TRC_NEXT_DAY;
  trcRec->MaxSize=_LOG_2MB_LIMIT;
  /*..........................................................................*/
  /* Test the file handle to make sure it is OK...                            */
  
  if (verbose == _TRC_DATA)
    status = TRC_QSnd (trcRec, "");
  else
    status = TRC_QSnd (trcRec, _TRC_INITSTR, TRC_GetTime (), name, verbose);
  if (status != _RET_SUCCESS) {
    trcRec = OAR_DelElement (trcHandle, alias);
    free (trcRec);
  }
  return (status);
}

/************************** End of function TRC_Open **************************/


/*******************************************************************************

   TRC_Close:                                         Date:  February 1996
   ~~~~~~~~~~                                       Author:  Sebastien Spicer

   This function will close a file from the tracing utility.  Allocated
   memory will be released.

      Arguments:
      ~~~~~~~~~~
         - alias (type is *char - Read only):
              The referenced string is the alias to be used for accessing
              the trace file.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TRC_Close (char *alias) {

  TRC_handle *trcRec;
  
  /*..........................................................................*/
  /* Get the file handle from the alias...                                    */

  if ((trcRec = OAR_DelElement (trcHandle, alias)) == NULL)
    return (TRC_SetAlarm ("TRC_Close", _RET_FILEOPENERR, alias));

  /*..........................................................................*/
  /* Send the shutdown messages...                                            */
  
  if (trcRec->verbose != _TRC_DATA) {
    TRC_QSnd (trcRec, _TRC_SHUTSTR, TRC_GetTime (), trcRec->name);
    TRC_QSnd (trcRec, _TRC_NEWLIN);
  }    

  /*..........................................................................*/
  /* Get rid of the data element...                                           */
  
  free (trcRec);
  return (_RET_SUCCESS);
}

/************************* End of function TRC_Close **************************/


/*******************************************************************************

   TRC_Shutdown:                                      Date:  February 1996
   ~~~~~~~~~~~~~                                    Author:  Sebastien Spicer

   This function will shutdown the entire tracing utility.  Allocated memory
   will be released.

      Arguments:
      ~~~~~~~~~~
         None.

      Returns:
      ~~~~~~~~
         Nothing.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void  TRC_Shutdown (void) {

  TRC_handle *trcRec;

  trcRec = OAR_GetFirst (trcHandle);
  while (trcRec != NULL) {
    if (trcRec->verbose != _TRC_DATA) {
      TRC_QSnd (trcRec, _TRC_SHUTSTR, TRC_GetTime (), trcRec->name);
      TRC_QSnd (trcRec, _TRC_NEWLIN);
    }    
    trcRec = OAR_GetNext (trcHandle);
  }
  OAR_Close (trcHandle);
  trcHandle = NULL;
  return;
}

/************************ End of function TRC_Shutdown ************************/


/*******************************************************************************

   TRC_Send:                                          Date:  November 1994
   ~~~~~~~~~                                        Author:  Sebastien Spicer

   This function will send a message to the trace file.

      Arguments:
      ~~~~~~~~~~
         - alias (type is *char - Read only):
              The referenced string is the alias to be used for accessing
              the trace file.

         - level (type is int - Read only):
              The value indicates the verbose level of the message being sent.

         - format (type is *char - Read only):
              The referenced string is the format specifier to be used for
              sending to the trace file.  It supports the same syntax as a
              printf statement.

         Subsequent arguments are variable and follow the same style as those
         for fprintf.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Sebastien Spicer           February 1996
           Added the alias concept and changed the function to handle a
           dynamic array of file handles.

*******************************************************************************/
#ifndef WIN32
int  TRC_Send (alias, level,format,va_alist)
char *alias;
int level;
char *format;
va_dcl
#else
int  TRC_Send (char *alias, int level,char *format,...)
#endif

{

  va_list argptr;
  int  status;
  FILE *stream;
  TRC_handle *trcRec;

  /*..........................................................................*/
  /* Get the file handle from the alias...                                    */

  if ((trcRec = OAR_GetElement (trcHandle, alias)) == NULL) {
    stream = stderr;

  } else {

     /*.......................................................................*/
     /* Reject messages that do not clear the level...                        */

    if (trcRec->verbose != _TRC_DATA)
      if (trcRec->verbose < level)
        return (_RET_SUCCESS);
    
    /*.......................................................................*/
    /* Open the stream to be used as the trace file...                       */

    if (strcmp (trcRec->name, "stdout") == 0)
      stream = stdout;
    else if (strcmp (trcRec->name, "stderr") == 0)
      stream = stderr;
    else 
    {
       /*...Extended functionality - makes new trace file if curr time...*/
       /*...is greater than nexOpenTime in handle...*/
       if (trcRec->nexOpenTime<=time(NULL))
       {
          int i=0;
          for(i=0;i<3;++i)
          {
            if(!TRC_NewTrcFilename(trcRec))
            {
               TRC_Clean(trcRec->alias);
               break;
            }
          }
       }
       if ((stream = fopen (trcRec->name, "a+")) == NULL)
           stream = stderr;
       else
       {
          /*... file open so check size ...*/
          fseek(stream, 0L, 2);
          if(ftell(stream) > trcRec->MaxSize)
          {
             int i=0;
             fclose(stream);
             for(i=0;i<3;++i)
             {
                if(!TRC_NewTrcFilename(trcRec))
                {
                    TRC_Clean(trcRec->alias);
                    break;
                }
             }
             if ((stream = fopen (trcRec->name, "a+")) == NULL)
                  stream = stderr;
          }
       }
    }
  }

  /*..........................................................................*/
  /* Send the message to the stream...                                        */
#ifndef WIN32
  va_start (argptr );
#else
  va_start (argptr, format);
#endif
 
  status = vfprintf (stream, format, argptr);
  va_end (argptr);

  /*..........................................................................*/
  /* Attempt to close the stream...                                           */
  
  if ((stream != stdout) && (stream != stderr))
    fclose (stream);
  if ((trcRec != NULL) && (status > 0))
    trcRec->hasTraced = 1;

  return (_RET_SUCCESS);
}

/************************** End of function TRC_Send **************************/

/*******************************************************************************

   TRC_Clean:                                          Date:  November 1994
   ~~~~~~~~~                                        Author:  Nawal

   This function will clean the trace file and record the staring time.

      Arguments:
      ~~~~~~~~~~
         - alias (type is *char - Read only):
              The referenced string is the alias to be used for accessing
              the trace file.
      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:

*******************************************************************************/
int  TRC_Clean (char *alias)

{
  int  status;
  FILE *stream;
  TRC_handle *trcRec;

  /*..........................................................................*/
  /* Get the file handle from the alias...                                    */

  if ((trcRec = OAR_GetElement (trcHandle, alias)) == NULL) {
    stream = stderr;

  } else {

     /*.......................................................................*/
     /* Reject messages that do not clear the level...                        */
        return (_RET_SUCCESS);
    
    /*.......................................................................*/
    /* Open the stream to be used as the trace file...                       */

    if (strcmp (trcRec->name, "stdout") == 0)
      stream = stdout;
    else if (strcmp (trcRec->name, "stderr") == 0)
      stream = stderr;
    else if ((stream = fopen (trcRec->name, "w")) == NULL)
      stream = stderr;
  }

  /*..........................................................................*/
  /* Send message to the stream...                                        */
 
  status = vfprintf (stream, "Starting at %s....\n", TRC_GetTime());

  /*..........................................................................*/
  /* Attempt to close the stream...                                           */
  
  if ((stream != stdout) && (stream != stderr))
    fclose (stream);
  if ((trcRec != NULL) && (status > 0))
    trcRec->hasTraced = 1;

  return (_RET_SUCCESS);
}

/************************** End of function TRC_Clean **************************/


/*******************************************************************************

   TRC_QSnd:                                          Date:  February 1996
   ~~~~~~~~~                                        Author:  Sebastien Spicer

   This function will send a message to the trace file (without performing
   basic checks and given the file handle record).

      Arguments:
      ~~~~~~~~~~
         - trcRec (type is *TRC_handle - Read only):
              The referenced record containe the trace record to be used for
              output.

         - format (type is *char - Read only):
              The referenced string is the format specifier to be used for
              sending to the trace file.  It supports the same syntax as a
              printf statement.

         Subsequent arguments are variable and follow the same style as those
         for fprintf.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

#ifndef WIN32
int  TRC_QSnd (trcRec, format,va_alist)
TRC_handle *trcRec;
char *format;
va_dcl
#else
int  TRC_QSnd (TRC_handle *trcRec, char *format,...)
#endif

{

  va_list argptr;
  int  status;
  FILE *stream;

  /*..........................................................................*/
  /* Open the stream to be used as the trace file...                          */

  if (strcmp (trcRec->name, "stdout") == 0)
    stream = stdout;
  else if (strcmp (trcRec->name, "stderr") == 0)
    stream = stderr;
  else if ((stream = fopen (trcRec->name, "a+")) == NULL)
    return (TRC_SetAlarm ("TRC_QSnd", _RET_FILEOPENERR, trcRec->name));

  /*..........................................................................*/
  /* Send the message to the stream...                                        */
#ifndef WIN32
  va_start (argptr );
#else
  va_start (argptr, format); 
#endif
  
  status = vfprintf (stream, format, argptr);
  va_end (argptr);
  if (status == EOF)
    return (TRC_SetAlarm ("TRC_QSnd", _RET_FILEWRITERR, trcRec->name));

  /*..........................................................................*/
  /* Attempt to close the stream...                                           */
  
  if ((stream != stdout) && (stream != stderr))
    fclose (stream);
  if (status > 0)
    trcRec->hasTraced = 1;

  return (_RET_SUCCESS);
}

/************************** End of function TRC_QSnd **************************/


/*******************************************************************************

   TRC_SetVerb:                                       Date:  February 1996
   ~~~~~~~~~~~~                                     Author:  Sebastien Spicer

   This function will reset the verbose level for the trace.

      Arguments:
      ~~~~~~~~~~
         - alias (type is *char - Read only):
              The referenced string is the alias to be used for accessing
              the trace file.

         - verbose (type is int - Read only):
              The given integer is the verbose level to be used when tracing.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Sebastien Spicer           February 1996
           Added the alias concept and changed the function to handle a
           dynamic array of file handles.

*******************************************************************************/

int  TRC_SetVerb (char *alias,
                 int  verbose) {

  TRC_handle *trcRec;
  
  /*..........................................................................*/
  /* Get the file handle from the alias...                                    */

  if ((trcRec = OAR_GetElement (trcHandle, alias)) == NULL)
    return (TRC_SetAlarm ("TRC_SetVerb", _RET_FILEOPENERR, alias));

  /*..........................................................................*/
  /* Reset global variables for the trace utility...                          */

  if (trcRec->verbose == verbose)
    return (_RET_SUCCESS);

  trcRec->verbose = verbose;
  if (verbose == _TRC_DATA)
    return (_RET_SUCCESS);
  else
    return (TRC_QSnd (trcRec, _TRC_INITSTR,
                      TRC_GetTime (), trcRec->name, verbose));
}

/************************ End of function TRC_SetVerb *************************/


/*******************************************************************************

   TRC_GetVerb:                                       Date:  April 1996
   ~~~~~~~~~~~~                                     Author:  Sebastien Spicer

   This function will return the verbose level for the trace.  If the trace
   is not found, a verbose level of 0 is returned.

      Arguments:
      ~~~~~~~~~~
         - alias (type is *char - Read only):
              The referenced string is the alias to be used for accessing
              the trace file.

      Returns:
      ~~~~~~~~
         Type is int - verbose level assigned to the specified log file.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TRC_GetVerb (char *alias) {

  TRC_handle *trcRec;
  
  /*..........................................................................*/
  /* Get the file handle from the alias...                                    */

  if ((trcRec = OAR_GetElement (trcHandle, alias)) == NULL)
    return (0);

  /*..........................................................................*/
  /* Return the requested verbose level...                                    */

  return (trcRec->verbose);
}

/************************ End of function TRC_GetVerb *************************/


/*******************************************************************************

   TRC_NewLine:                                       Date:  December 1994
   ~~~~~~~~~~~~                                     Author:  Sebastien Spicer

   This function will print a new line (block separator) if any tacing has
   been performed since the last new line.

      Arguments:
      ~~~~~~~~~~
         - alias (type is *char - Read only):
              The referenced string is the alias to be used for accessing
              the trace file.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Sebastien Spicer           February 1996
           Added the alias concept and changed the function to handle a
           dynamic array of file handles.

*******************************************************************************/

int  TRC_NewLine (char *alias) {

  TRC_handle *trcRec;
  int status;
  
  /*..........................................................................*/
  /* Get the file handle from the alias...                                    */

  if ((trcRec = OAR_GetElement (trcHandle, alias)) == NULL)
    return (TRC_SetAlarm ("TRC_NewLine", _RET_FILEOPENERR, alias));

  /*..........................................................................*/
  /* Send the new line if it is required...                                   */

  status = _RET_SUCCESS;
  if (trcRec->hasTraced != 0) {
    if (trcRec->verbose != _TRC_DATA)
      status = TRC_QSnd (trcRec, _TRC_NEWLIN, "");
    trcRec->hasTraced = 0;
  }
  return (status);
}

/************************* End of function TRC_NewLine ************************/


/*******************************************************************************

   TRC_GetTime:                                       Date:  October 1994
   ~~~~~~~~~~~~                                     Author:  Sebastien Spicer

   This function returns the current time in a formated string.

      Arguments:
      ~~~~~~~~~~
         None.

      Returns:
      ~~~~~~~~
         Type is *char - pointer to a static null terminated string that
         contains the current time.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

char * TRC_GetTime (void) {

  time_t rawTime;
  struct tm *fmtTime;
  static char strTime[_SIZ_GENDATE];
  
  rawTime = time (NULL);
  fmtTime = localtime (&rawTime);
  sprintf (strTime, "%04d/%02d/%02d %02d:%02d:%02d",
           fmtTime->tm_year+1900, fmtTime->tm_mon+1, fmtTime->tm_mday,
           fmtTime->tm_hour, fmtTime->tm_min, fmtTime->tm_sec);
  return (strTime);
}

/************************* End of function TRC_GetTime ************************/


/*******************************************************************************

   TRC_LocFile:                                      Date:  February 1996
   ~~~~~~~~~~~~                                    Author:  Sebastien Spicer

   This function is used when locating an element in an ordered array.

      Arguments:
      ~~~~~~~~~~
         - ident (type is *char - Read only):
              The referenced structure contains the key record for which the
              search is being performed.

         - elem (type is *TRC_handle - Read only):
              The referenced structure contains an element to be compared
              with the identifier.

      Returns:
      ~~~~~~~~
         Type is int - the return value will be 0 when identifier matches
         element.  The function returns 1 when identifier is greater than
         element and -1 when identifier is less than element.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TRC_LocFile (char       *ident,
                 TRC_handle *elem) {

  int comp;

  comp = strcmp (ident, elem->alias);
  if (comp > 0)
    return (1);
  if (comp < 0)
    return (-1);
  return (0);
}

/************************ End of function TRC_LocFile *************************/


/*******************************************************************************

   TRC_CmpFile:                                      Date:  February 1996
   ~~~~~~~~~~~~                                    Author:  Sebastien Spicer

   This function is used when inserting an element in an ordered array.

      Arguments:
      ~~~~~~~~~~
         - elem1 (type is *TRC_handle - Read only):
              The referenced structure contains an element to be compared
              with elem2.

         - elem2 (type is *TRC_handle - Read only):
              The referenced structure contains an element to be compared
              with elem1

      Returns:
      ~~~~~~~~
         Type is int - the return value will be 0 when the two elements are 
         equal.  The function returns 1 when elem1 is greater than elem2 and
         -1 when elem1 is less than elem2.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int TRC_CmpFile (TRC_handle *elem1,
                 TRC_handle *elem2) {

  int comp;

  comp = strcmp (elem1->alias, elem2->alias);
  if (comp > 0)
    return (1);
  if (comp < 0)
    return (-1);
  return (0);
}

/************************ End of function TRC_CmpFile *************************/


/*******************************************************************************

   TRC_LocAlrm:                                      Date:  February 1996
   ~~~~~~~~~~~~                                    Author:  Sebastien Spicer

   This function is used when locating an element in an ordered array.

      Arguments:
      ~~~~~~~~~~
         - ident (type is *int - Read only):
              The referenced structure contains the key record for which the
              search is being performed.

         - elem (type is *TRC_alrdef - Read only):
              The referenced structure contains an element to be compared
              with the identifier.

      Returns:
      ~~~~~~~~
         Type is int - the return value will be 0 when identifier matches
         element.  The function returns 1 when identifier is greater than
         element and -1 when identifier is less than element.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int TRC_LocAlrm (int        *ident,
                 TRC_alrdef *elem) {

  int comp;

  comp = *ident - elem->code;
  if (comp > 0)
    return (1);
  if (comp < 0)
    return (-1);
  return (0);
}

/************************ End of function TRC_LocAlrm *************************/


/*******************************************************************************

   TRC_CmpAlrm:                                      Date:  February 1996
   ~~~~~~~~~~~~                                    Author:  Sebastien Spicer

   This function is used when inserting an element in an ordered array.

      Arguments:
      ~~~~~~~~~~
         - elem1 (type is *TRC_alrdef - Read only):
              The referenced structure contains an element to be compared
              with elem2.

         - elem2 (type is *TRC_alrdef - Read only):
              The referenced structure contains an element to be compared
              with elem1

      Returns:
      ~~~~~~~~
         Type is int - the return value will be 0 when the two elements are 
         equal.  The function returns 1 when elem1 is greater than elem2 and
         -1 when elem1 is less than elem2.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TRC_CmpAlrm (TRC_alrdef *elem1,
                 TRC_alrdef *elem2) {

  int comp;

  comp = elem1->code - elem2->code;
  if (comp > 0)
    return (1);
  if (comp < 0)
    return (-1);
  return (0);
}

/************************ End of function TRC_CmpAlrm *************************/

/* This routine will return the Error message to the caller */

/*******************************************************************************

   TRC_ErrorMsg                                     Date:  Apr'98
   ~~~~~~~~~~~~                                    Author:  Jaya

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

char *TRC_ErrorMsg() 
{
        return ( alrMesg);
}
/*******************************************************************************

   TRC_ChkAlarm                                      Date:  Apr'98
   ~~~~~~~~~~~~                                    Author:  Jaya

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

/* This will return the error status to the caller */

int  TRC_ChkAlarm () { 

        return (alrCode == _RET_SUCCESS);
}
/*******************************************************************************

   TRC_CmpAlarm                                     Date:  Apr'98
   ~~~~~~~~~~~~                                    Author:  Jaya

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

/* Check the error code with the given code */

int  TRC_CmpAlarm(int code) {
         return (alrCode == code);
}

/* Return the alarm error code */
/*******************************************************************************

   TRC_GetAlarm                                     Date:  Apr'98
   ~~~~~~~~~~~~                                    Author:  Jaya

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TRC_GetAlarm () {
        return alrCode;
}


/*...rename trace file to dated saved-file...*/
/*******************************************************************************

   TRC_NewTrcFilename                              Date:  Jun' 2002
   ~~~~~~~~~~~~                                    Author:  Ivan

   This function is used to rename the trace file to a dated file to save
   the trace info.
   Extended functionality - makes new trace file if curr time is greater than 
   nexOpenTime in handle

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int TRC_NewTrcFilename(TRC_handle *trcRec)
{

   /*...Extended functionality - makes new trace file if curr time...*/
   /*...is greater than nexOpenTime in handle...*/
   char tfname[_SIZ_FILENAME];
   char ttime[_SIZ_GENDATE], q;
   int yy,mm,dd,hh,mi,ss;

   strcpy(ttime, TRC_GetTime());
   sscanf(ttime, "%04d%c%02d%c%02d%c%02d%c%02d%c%02d"
      , &yy, &q, &mm, &q, &dd, &q, &hh, &q, &mi, &q, &ss);
   sprintf(ttime,"%04d-%02d-%02d-%02d%02d%02d", yy, mm, dd, hh, mi, ss);

   /*...now all date part vars are free for reuse...*/
   ss=strlen(trcRec->name)-4;
   sprintf(tfname,"%*.*s-%17.17s%4.4s", ss, ss, trcRec->name, ttime, trcRec->name+ss);

   ss=rename (trcRec->name, tfname);
   /*...If success, update change time...*/
   if(!ss)
      trcRec->nexOpenTime = time(NULL) + (time_t) trcRec->chgTime;
   return ss;
}


/*******************************************************************************

   TRC_OpenEx:                                      Date:  Jun 2002
   ~~~~~~~~~                                        Author:  Ivan

   This function EXTENDS the existing TRC_Open function by adding a time parameter 
   to the function which overides the default chgTime of _TRC_NEXT_DAY used in
   TRC_Open.
   The rest of the functionality is the same.

   This function will intialize the tracing utility.  It basically sets
   global variables based on the arguments and verifies that tracing can
   work with the given parameters.

      Arguments:
      ~~~~~~~~~~
         - alias (type is *char - Read only):
              The referenced string is the alias to be used for accessing
              the trace file.

         - name (type is *char - Read only):
              The referenced string is the file name to be used for tracing.
              The function accepts "stdout" and "stderr" as options.

         - verbose (type is int - Read only):
              The given integer is the verbose level to be used when tracing.

      // NEW PARAMETER
         - Duration (type is int - Read only):
              The given integer is the duration in seconds before next 
              log is opened. By default set to _TRC_NEXT_DAY in TRC_Open.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Sebastien Spicer           February 1996
           Added the alias concept and changed the function to handle a
           dynamic array of file handles.
         - Extended to rename file based to save log file after x number of 
           seconds

*******************************************************************************/

int  TRC_OpenEx (char *alias,  char *name, int  verbose, int Duration, long Fsize) {

   int status=0;
   TRC_handle *trcRec;
   /*...Extended functionality - makes new trace file if curr time...*/
   /*...is greater than nexOpenTime in handle...*/
   if( (status=TRC_Open(alias, name, verbose)) ==_RET_SUCCESS)
   {
      if ((trcRec = OAR_GetElement (trcHandle, alias)) == NULL)
          return (TRC_SetAlarm ("TRC_OpenEx", _RET_FILEOPENERR, alias));
      /*...Overwrite the defaults set by TRC_Open...*/
      trcRec->nexOpenTime = time(NULL) + (time_t) Duration;
      trcRec->chgTime = Duration;
      trcRec->MaxSize=Fsize;
   }
   return status;
}
