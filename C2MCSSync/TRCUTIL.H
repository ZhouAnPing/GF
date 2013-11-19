/******************************************************************************/
/*                                                                            */
/*   trcutil.h:                                                               */
/*   ~~~~~~~~~~                                                               */
/*                                                                            */
/*   This file contains define statements and prototypes used by the trace    */
/*   utility functions in trcutil.c.                                          */
/*                                                                            */
/******************************************************************************/

#ifndef _TRC_TRCUTIL
#define _TRC_TRCUTIL

/******************************************************************************/
/********************* Start of parameter definitions *************************/

#define _TRC_SUCCESS       0                   /* successful execution        */
#define _TRC_BADFIL        1                   /* bad file name               */
#define _TRC_NOOPEN        2                   /* could not open message file */
#define _TRC_NOWRITE       3                   /* could not send to target    */

#define _TRC_FNAMSZ        256                 /* maximum file name length    */

#define _TRC_INITSTR       " %s - Trace file set to verbose level %d.\n"
#define _TRC_NEWLIN        " -----------------\n"
#define _TRC_STATUS        " %s - %s: %s.\n"
#define _TRC_DMQERROR      " %s - %s\n"

/********************** End of parameter definitions **************************/
/******************************************************************************/

/******************************************************************************/
/************************ Start of macro definitions **************************/

extern int TRC_Verbose;

#define TRC_Trace0 if (TRC_Verbose >= 0) TRC_Send
#define TRC_Trace1 if (TRC_Verbose >= 1) TRC_Send
#define TRC_Trace2 if (TRC_Verbose >= 2) TRC_Send
#define TRC_Trace3 if (TRC_Verbose >= 3) TRC_Send
#define TRC_Trace4 if (TRC_Verbose >= 4) TRC_Send
#define TRC_Trace5 if (TRC_Verbose >= 5) TRC_Send
#define TRC_Trace6 if (TRC_Verbose >= 6) TRC_Send
#define TRC_Trace7 if (TRC_Verbose >= 7) TRC_Send
#define TRC_Trace8 if (TRC_Verbose >= 8) TRC_Send
#define TRC_Trace9 if (TRC_Verbose >= 9) TRC_Send

/************************ End of macro definitions ****************************/
/******************************************************************************/

/******************************************************************************/
/********************** Start of function prototypes **************************/

int TRC_Open(char *name, int verbose);

int TRC_Send();

void TRC_NewLine(void);

char *TRC_GetTime(void);

/*********************** End of function prototypes ***************************/
/******************************************************************************/

#endif

/******************************************************************************/
/**************************** End of file trcutil.h ***************************/
/******************************************************************************/
