#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lotlist.h"
#ifdef VMS
#include "ssp_cimlib:talkutl.h"
#include "ssp_cimlib:cimdefs.h"
#include "ssp_cimlib:parsutl.h"
#include "ssp_cimlib:trcutil.h"
#else
#include "talkutl.h"
#include "cimdefs.h"
#include "parsutl.h"
#include "trcutil.h"
#endif



int isLotExist(char *LotName) {
  struct LotStruct *curr = NULL;
  if (!LotName)
    return -1;
  for (curr = LotIdstart; curr; curr = curr->next) {
    if (!strcmp(curr->LotName, LotName))
      break;
  }

  if (curr)
    return 1;
  else
    return 0;
}

int LoadLotList() {
  FILE *fp = NULL;
  char readbuf[_SIZ_LOTNAME + 1] = { 0 };
  struct LotStruct *cptr = NULL; /* we user the smploc structure to keep the names of stockers with FAB3 lots */
  int count = 0;

  fp = fopen(_INI_LOTLIST, "r");
  if (fp == NULL) {
    /* Report error and return. */
    TRC_Send(LOG_ALIAS, _TRC_LVL_ERROR, "%s: Error opening init file[%s]\n", TRC_GetTime(), _INI_LOTLIST);
    return -1;
  }
  TRC_Send(LOG_ALIAS, _TRC_LVL_INFO, "%s: Open init file[%s] successful\n", TRC_GetTime(), _INI_LOTLIST);

  /*  Search for _INI_LOTLIST_SECTION pattern in the file  */
  fgets(readbuf, _SIZ_LOTNAME, fp);
  do {
    if (readbuf[0] != _INI_SECTION_BEGIN)
      continue;
    if (!strncmp(readbuf, _INI_LOTLIST_SECTION, _INI_LOTLIST_SECTION_SIZ))
      break;
  } while ((fgets(readbuf, _SIZ_LOTNAME, fp)) != NULL);

  if (feof(fp)) /* End of file reached */
  {
    /* Report error and return. */
    TRC_Send(LOG_ALIAS, _TRC_LVL_ERROR, "%s: No FAB3LotStockers section found in init file [%s].\n", TRC_GetTime(),
             _INI_LOTLIST);
    fclose(fp);
    return -1;
  }

  /* if we come here, means it found the section we need. Call function to create FAB3Lot stocker list
   */
  LotIdstart = LotListMemCreate(fp);
  /*
   TRC_Send(LOG_ALIAS, _TRC_LVL_INFO,
   "%s: LotIdstart returned [%c] from LotListMemCreate\n",
   TRC_GetTime(), LotIdstart ? 'T' : 'F' );
   */
  if (!LotIdstart) {
    /* Report error and return. */
    TRC_Send(LOG_ALIAS, _TRC_LVL_ERROR, "%s: No Stockers names for FAB3 lots found in init file [%s].\n", TRC_GetTime(),
             _INI_LOTLIST);
    fclose(fp);
    return -1;
  }

  fclose(fp); /* Close ini file */
  /* Log Stocker names */
  TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: FAB3 Lot Names in [%s]\n", TRC_GetTime(), _INI_LOTLIST);
  for (cptr = LotIdstart, count = 1; cptr; cptr = cptr->next, ++count) {
    TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: [%d][%s][%d]\n", TRC_GetTime(), count, cptr->LotName,
             strlen(cptr->LotName));
  }
  TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: [%d] Stocker%s.\n", TRC_GetTime(), count - 1, count == 1 ? "" : "s");
  return count;
}
/***** END Function **************************************************************************/

/*********************************************************************************************
 Function              : LotListMemFree()
 Parameters    : NONE - LotIdstart could be passed, but its a global pointer
 therefore there is no need to pass it as a parameter
 Returns               : int

 Functionality : This function is called to free the SMPLOClist in case of abnormal shutdown.

 Revision History:
 ~~~~~~~~~~~~~~~~
 Name  Date    Change done
 ----  ------- -------------------------------------------------------------------------------
 Ivan  000926  Started coding this function to add to existing PRISvr.

 *********************************************************************************************/
int LotListMemFree() {
  int i = 0;
  struct LotStruct *curr = NULL, *prev = NULL;

  curr = LotIdstart;
  while (curr) {
    prev = curr;
    curr = curr->next;
    free(prev);
  }
  LotIdstart = NULL;
  return 0;
}
/*********************************************************************************************
 Function              : LotListMemCreate()
 Parameters    : FILE * - file pointer to init file.
 Returns               : struct  *

 Functionality : This function is called ONCE during startup to create a linked list of the
 SMP Stockers that are used for FAB3 Lots.
 Note that to avoid declaration of another structure, we use the smplocations
 structure, which has all the properties required for stocker list.
 not any more - new struct defined

 *********************************************************************************************/
struct LotStruct* LotListMemCreate(FILE * fp) {
  struct LotStruct *sptr=NULL, *curr=NULL, *prev=NULL, *new=NULL;
  char readbuf[_SIZ_LOTNAME + 1] = { 0 };

  while ((fgets(readbuf, _SIZ_LOTNAME, fp)) != NULL) {
    TRC_Send(LOG_ALIAS, _TRC_LVL_INFO, "%s: [%s]\n", TRC_GetTime(), readbuf);
    if (readbuf[0] == _INI_SECTION_BEGIN || readbuf[0] == _INI_SECTION_END)
      break; /* out we go if next section starts or end of this section (blank line)*/

    if (readbuf[0] == _INI_COMMENT)
      continue; /* get next line if this is a comment line */

    if ( !( new=(struct LotStruct*) malloc( sizeof(struct LotStruct)) )) {
      /* Bitch. No more memory */
      TRC_Send(LOG_ALIAS, _TRC_LVL_WARN, "%s: Cannot allocate [%d] bytes of memory for [%s].\n", TRC_GetTime(),
               sizeof(struct LotStruct), readbuf);
      if (sptr)                                        // release currently held stockers as the start
        LotListMemFree();                                        // pointer will be set to null due to this error
    return new; /* return null to indicate error */
  }

  new->next=NULL;
  sscanf( readbuf,"%s",new->LotName);
  //fprintf(stderr,"\t%s.\n",new->LotName);
  if (sptr) /* list exists, insert */
  {
    curr = sptr;
    prev = curr;
    while ((curr) && strcmp( new->LotName, curr->LotName) > 0)
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
/* SMP Stocker List is now ready */
                                        //for (curr=sptr;(curr);curr=curr->next)
                                        //fprintf(stderr,"\t [%s]\n",curr->LotName);
return sptr; /* hopefully into global ptr LotIdstart */
}
/***** END Function **************************************************************************/
