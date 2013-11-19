/******************************************************************************/
/*   getmcs.h:                                       Date:  September 1995    */
/*   ~~~~~~~~~                                     Author:  Madan Agarwal     */
/*                                                                            */
/*   This file contains the define statements and type definitions for the    */
/*   lot reservation server.  It also includes the prototypes of functions    */
/*   defined in getmcs.c.                                                     */
/*                                                                            */
/******************************************************************************/

#ifndef _GETMCS
#define _GETMCS

/******************************************************************************/
/********************* Start of parameter definitions *************************/
#define LOT_LEN 12
#define LOC_ID_LEN 8
#define EQP_ID_LEN 8
#define _ALL_SPACES   "                 "
char all_space(char *);

enum HSMS_STATES {
  HSMS_NOT_ENABLED = 0,
  HSMS_TCP_NOT_CONNECTED,
  HSMS_NOT_SELECTED,
  HSMS_SELECTED
};

#endif
