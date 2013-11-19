/*************************************************************************

 MCSloc:                                      Date:  July 2013
 ~~~~~~~~~~~~~                                     Author: Zhou An-Ping
*/
/*...Standard include files...*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/*...CIMLIB include files...*/
#ifdef WIN32
#include "cimdefs.h"
#include "ordarray.h"
#include "trcutil.h"
#include "cimtokens.h"
#include "parsutl.h"
#include "talkutl.h"
#else
#include "ssp_cimlib:cimdefs.h"
#include "ssp_cimlib:ordarray.h"
#include "ssp_cimlib:trcutil.h"
#include "ssp_cimlib:cimtokens.h"
#include "ssp_cimlib:parsutl.h"
#include "ssp_cimlib:talkutl.h"
#endif

/*...MHServ include files...*/
#include "mcsint.h"

/*********************************************************************************************
 Function              : LocateIniFileSmpSection()
 Parameters    : None
 Returns               : int

 Functionality : This function is called ONCE during startup to read a init file and create a
 linked list of locations at a site. The site is assumed to be SMP for this
 coding session.

 Revision History:
 ~~~~~~~~~~~~~~~~
 Name  Date    Change done
 ----  ------- -------------------------------------------------------------------------------
 Ivan  000926  Started coding this function to add to existing PRISvr.

 *********************************************************************************************/

int GetInitInfo() {
  FILE *fp = NULL;
  char readbuf[_SIZ_LOCNAME + 1] = { 0 };
  struct iList *cptr = NULL;

  fp = fopen(_INI_MCSINT, "r");
  if ((fp) == NULL) {
    /* Report error and return. */
    TRC_Send(LOG_ALIAS, _TRC_LVL_ERROR, "%s: Error opening init file[%s]\n", TRC_GetTime(), _INI_MCSINT);
    return -1;
  }
  TRC_Send(LOG_ALIAS, _TRC_LVL_INFO, "%s: Open init file[%s] successful\n", TRC_GetTime(), _INI_MCSINT);
  /*  Search for _INI_SMPLOCS pattern in the file  */
  fgets(readbuf, _SIZ_LOCNAME, fp);
  do {
    if (readbuf[0] == _INI_SECTION_BEGIN) {
      if (!strncmp(readbuf, _INI_FAB3LOCS_SECTION, strlen(_INI_FAB3LOCS_SECTION))) {
        fab3LocationStart = CreateiList(fp); /* call function to create locations list */
        TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: FAB3 Locations in [%s]\n", TRC_GetTime(), _INI_MCSINT);
        for (cptr = fab3LocationStart; cptr; cptr = cptr->next)
          TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: %s\n", TRC_GetTime(), cptr->iName);
      }
      if (!strncmp(readbuf, _INI_FAB3STOCKER_SECTION, strlen(_INI_FAB3STOCKER_SECTION))) {
        fab3StockerStart = CreateiList(fp); /* call function to create stocker list */
        TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: FAB3 Stockers in [%s]\n", TRC_GetTime(), _INI_MCSINT);
        for (cptr = fab3StockerStart; cptr; cptr = cptr->next)
          TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: %s\n", TRC_GetTime(), cptr->iName);
      }

      if (!strncmp(readbuf, _INI_FAB2STOCKER_SECTION, strlen(_INI_FAB2STOCKER_SECTION))) {
        fab2StockerStart = CreateiList(fp); /* call function to create stocker list */
        TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: FAB2 Stockers in [%s]\n", TRC_GetTime(), _INI_MCSINT);
        for (cptr = fab2StockerStart; cptr; cptr = cptr->next)
          TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: %s\n", TRC_GetTime(), cptr->iName);
      }

      if (!strncmp(readbuf, _INI_FAB5STOCKER_SECTION, strlen(_INI_FAB5STOCKER_SECTION))) {
        fab5StockerStart = CreateiList(fp); /* call function to create stocker list */
        TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: FAB5 Stockers in [%s]\n", TRC_GetTime(), _INI_MCSINT);
        for (cptr = fab5StockerStart; cptr; cptr = cptr->next)
          TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: %s\n", TRC_GetTime(), cptr->iName);
      }
    }
  } while ((fgets(readbuf, _SIZ_LOCNAME, fp)) != NULL);
  fclose(fp);
  if (!fab3LocationStart) /* End of file reached and no list created */
  {
    /* Report error and return. */
    TRC_Send(LOG_ALIAS, _TRC_LVL_ERROR, "%s: No FAB3 Locations found in init file[%s]\n", TRC_GetTime(), _INI_MCSINT);
  }
  if (!fab3StockerStart) /* End of file reached and no list created */
  {
    /* Report error and return. */
    TRC_Send(LOG_ALIAS, _TRC_LVL_ERROR, "%s: No FAB3 Stockers found in init file[%s]\n", TRC_GetTime(), _INI_MCSINT);
  }

  if (!fab2StockerStart) /* End of file reached and no list created */
  {
    /* Report error and return. */
    TRC_Send(LOG_ALIAS, _TRC_LVL_ERROR, "%s: No FAB2 Stockers found in init file[%s]\n", TRC_GetTime(), _INI_MCSINT);
  }

  if (!fab5StockerStart) /* End of file reached and no list created */
  {
    /* Report error and return. */
    TRC_Send(LOG_ALIAS, _TRC_LVL_ERROR, "%s: No FAB5 Stockers found in init file[%s]\n", TRC_GetTime(), _INI_MCSINT);
  }
  return 0;
}
/***** END Function **************************************************************************/

/*********************************************************************************************
 Function              : FreeSmpLocationsMem()
 Parameters    : NONE - SMPLOCstart could be passed, but its a global pointer
 therefore there is no need to pass it as a parameter
 Returns               : int

 Functionality : This function is called to free the SMPLOClist in case of abnormal shutdown.

 Revision History:
 ~~~~~~~~~~~~~~~~
 Name  Date    Change done
 ----  ------- -------------------------------------------------------------------------------
 Ivan  000926  Started coding this function to add to existing PRISvr.

 *********************************************************************************************/
int FreeiList() {
  struct iList *curr = NULL, *prev = NULL;

  if (fab3LocationStart) {
    curr = fab3LocationStart;
    while (curr) {
      prev = curr;
      curr = curr->next;
      free(prev);
    }
    fab3LocationStart = NULL;
  }

  if (fab3StockerStart) {
    curr = fab3StockerStart;
    while (curr) {
      prev = curr;
      curr = curr->next;
      free(prev);
    }
    fab3StockerStart = NULL;
  }

  if (fab2StockerStart) {
    curr = fab2StockerStart;
    while (curr) {
      prev = curr;
      curr = curr->next;
      free(prev);
    }
    fab2StockerStart = NULL;
  }

  if (fab5StockerStart) {
    curr = fab5StockerStart;
    while (curr) {
      prev = curr;
      curr = curr->next;
      free(prev);
    }
    fab5StockerStart = NULL;
  }

  return 0;
}
/***** END Function **************************************************************************/


/*.............................................................*/
char *rTrim(char *tStr) {
  int iCnt;
  iCnt = strlen(tStr) - 1;
  for (; tStr[iCnt] == 32; --tStr)
    tStr[iCnt] = 0;
  return tStr;
}

/*.............................................................*/
int getStockerFabNo(char *stockId) {
  int fabNo = -1;
  struct iList *curr = NULL;

  /***  Loop till end of list or find the stocker
   ***/
  for (curr = fab2StockerStart; (curr) && 1; curr = curr->next) {
    if (!strcmp(curr->iName, stockId)) {
      fabNo = 2;  //FAB 2 stocker
      return fabNo;
    }
  }
  for (curr = fab3StockerStart; (curr) && 1; curr = curr->next) {
    if (!strcmp(curr->iName, stockId)) {
      fabNo = 3;  //FAB 3 stocker
      return fabNo;
    }
  }
  for (curr = fab5StockerStart; (curr) && 1; curr = curr->next) {
    if (!strcmp(curr->iName, stockId)) {
      fabNo = 5;
      return fabNo;  //FAB 5 stocker
    }
  }
  return fabNo;
}

/*********************************************************************************************
 Function              : CreateiList()
 Parameters    : FILE * - file pointer to init file.
 Returns               : struct iList *

 Functionality : This function is called ONCE during startup to create a linked list of the
 SMP locations read from the init file. The site is assumed to be SMP for this
 coding session.

 Revision History:
 ~~~~~~~~~~~~~~~~
 Name  Date    Change done
 ----  ------- -------------------------------------------------------------------------------
 Ivan  000926  Started coding this function to add to existing PRISvr.

 *********************************************************************************************/
struct iList *CreateiList(FILE *fp) {
  struct iList *sptr=NULL, *curr=NULL, *prev=NULL, *new=NULL;
  char readbuf[_SIZ_LOCNAME + 1] = { 0 };

  while ((fgets(readbuf, _SIZ_LOCNAME, fp)) != NULL) {
//      TRC_Send(LOG_ALIAS, _TRC_LVL_INFO,
//         "%s: [%s]\n", TRC_GetTime(), readbuf);
    if (readbuf[0] == _INI_SECTION_BEGIN || readbuf[0] == _INI_SECTION_END)
      break; /* out we go if next section starts or blank line to show end of section*/
    if (readbuf[0] == _INI_COMMENT)
      continue; /* get next if comment line */

    if ( !( new=(struct iList *) malloc( sizeof(struct iList)) )) {
      /* No more memory */
      TRC_Send(LOG_ALIAS, _TRC_LVL_WARN, "%s: Cannot allocate [%d] bytes of memory for [%s].\n", TRC_GetTime(),
               sizeof(struct iList), readbuf);
    return new; /* return null to indicate error */
  }

  new->next=NULL;
  sscanf( readbuf,"%s",new->iName);
  if (sptr) /* list exists, insert */
  {
    curr = sptr;
    prev = curr;
    while ((curr) && strcmp( new->iName, curr->iName) > 0)
    {
      prev = curr;
      curr = curr->next;
    }
    if (curr) {
      if (curr == sptr) {
        /* insert at begining of existing list */
      new->next=sptr;
      sptr=new;
    } else {
      /* insert into middle somewhere */
    prev->next=new;
    new->next=curr;
  }
} else {
  /* insert at end of list */
prev->next=new;
new->next=NULL;
}
} else {
/* list is null, add first */
sptr=new;
prev=new;
}
}
/* SMP LOCATIONS List is now ready */
return sptr; /* hopefully into global ptr SMPLOCstart */
}
/***** END Function **************************************************************************/
