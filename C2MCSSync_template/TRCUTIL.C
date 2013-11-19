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
#include <varargs.h>
#include <string.h>

/* End of standard includes.                                                  */
/******************************************************************************/

/******************************************************************************/
/* The following are include statements for local files.                      */

#include "trcutil.h"

/* End of include statements for local files.                                 */
/******************************************************************************/

/******************************************************************************/
/* Declaration of global variables with file scope.                           */

int TRC_Verbose = 9;
int TRC_HasTraced = 0;
char TRC_FileName[_TRC_FNAMSZ] = "stdout";

/* End of global variable declarations...                                     */
/******************************************************************************/

/*******************************************************************************

 TRC_Open:                                          Date:  November 1994
 ~~~~~~~~~                                        Author:  Sebastien Spicer

 This function will intialize the tracing utility.  It basically sets
 global variables based on the arguments and verifies that tracing can
 work with the given parameters.

 Arguments:
 ~~~~~~~~~~
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
 - Author                     Date
 Description...

 *******************************************************************************/

int TRC_Open(char *name, int verbose) {

	int status;
	int verbHold;
	char nameHold[_TRC_FNAMSZ];

	/*..........................................................................*/
	/* Initialize global variables for the trace utility...                     */

	strcpy(nameHold, TRC_FileName);
	verbHold = TRC_Verbose;
	strcpy(TRC_FileName, name);
	TRC_Verbose = verbose;

	/*..........................................................................*/
	/* Make sure that the given parameters are OK...                            */

	if (strlen(name) >= _TRC_FNAMSZ)
		status = _TRC_BADFIL;
	else
		status = TRC_Send(_TRC_INITSTR, TRC_GetTime(), TRC_Verbose);

	/*..........................................................................*/
	/* If there was an error, reset the global variables...                     */

	if (status != _TRC_SUCCESS) {
		strcpy(TRC_FileName, nameHold);
		TRC_Verbose = verbHold;
	}
	return (status);
}

/************************** End of function TRC_Open **************************/

/*******************************************************************************

 TRC_Send:                                          Date:  November 1994
 ~~~~~~~~~                                        Author:  Sebastien Spicer

 This function will send a message to the trace file.

 Arguments:
 ~~~~~~~~~~
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

int TRC_Send(format, va_alist)
char *format;
va_dcl
{

	va_list argptr;
	int status;
	FILE *stream;

	/*..........................................................................*/
	/* Open the stream to be used as the trace file...                          */

	if (TRC_FileName[0] == 0)
	return (_TRC_NOOPEN);
	else if (strcmp (TRC_FileName, "stdout") == 0)
	stream = stdout;
	else if (strcmp (TRC_FileName, "stderr") == 0)
	stream = stderr;
	else if ((stream = fopen (TRC_FileName, "a+")) == NULL)
	return (_TRC_NOOPEN);

	/*..........................................................................*/
	/* Send the message to the stream...                                        */

	va_start (argptr);
	status = vfprintf (stream, format, argptr);
	va_end (argptr);
	if (status == EOF)
	return (_TRC_NOWRITE);

	/*..........................................................................*/
	/* Attempt to close the stream...                                           */

	if ((stream != stdout) && (stream != stderr))
	fclose (stream);
	if (status > 0)
	TRC_HasTraced = 1;

	return (_TRC_SUCCESS);
}

/************************** End of function TRC_Send **************************/

/*******************************************************************************

 TRC_NewLine:                                       Date:  December 1994
 ~~~~~~~~~~~~                                     Author:  Sebastien Spicer

 This function will print a new line (block separator) if any tacing has
 been performed since the last new line.

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

void TRC_NewLine(void) {

	if (TRC_HasTraced != 0) {
		TRC_Send(_TRC_NEWLIN, "");
		TRC_HasTraced = 0;
	}
	return;
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

char *TRC_GetTime(void) {

	time_t rawTime;
	struct tm *fmtTime;
	static char strTime[18];

	rawTime = time(NULL);
	fmtTime = localtime(&rawTime);
	sprintf(strTime, "%02d/%02d/%02d %02d:%02d:%02d", fmtTime->tm_year,
			fmtTime->tm_mon + 1, fmtTime->tm_mday, fmtTime->tm_hour,
			fmtTime->tm_min, fmtTime->tm_sec);
	return (strTime);
}

/************************* End of function TRC_GetTime ************************/

/******************************************************************************/
/***************************** End of file trcutil.c **************************/
/******************************************************************************/

