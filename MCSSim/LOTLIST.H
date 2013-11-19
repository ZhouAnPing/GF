/*
 * LotLIST.H
 *
 *  Created on: Aug 15, 2013
 *      Author: sid105131
 */

#ifndef LOTLIST_H_
#define LOTLIST_H_

#define _SIZ_LOTNAME                    14
#define _INI_LOTLIST                      "MCSSIM:LOTLIST.INI"
#define _INI_LOTLIST_SECTION      "[LOTLIST]"
#define _INI_SECTION_BEGIN              '['
#define _INI_SECTION_END                '\n'
#define _INI_COMMENT                    '#'
#define _INI_LOTLIST_SECTION_SIZ  9
#define LOG_ALIAS               "MCSSIMFail"

struct LotStruct {
  char LotName[_SIZ_LOTNAME];
  struct LotStruct *next;
};

struct LotStruct *LotIdstart; /* GLOBAL base pointer to list of Stockers */

int LoadLotList();
int isLotExist(char *LotName);
int LotListMemFree();
struct LotStruct* LotListMemCreate(FILE *);
#endif /* LOTLIST_H_ */
