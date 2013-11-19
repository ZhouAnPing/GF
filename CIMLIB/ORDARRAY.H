/******************************************************************************/
/*                                                                            */
/*   ordarray.h:                                                              */
/*   ~~~~~~~~~~~                                                              */
/*                                                                            */
/*   This file contains define statements and prototypes for the functions    */
/*   that are contained in ordarray.h.                                        */
/*                                                                            */
/******************************************************************************/


#ifndef _OAR_ORDARRAY
#define _OAR_ORDARRAY

/******************************************************************************/
/********************* Start of parameter definitions *************************/

#define _OAR_INVALIDPOS -1                     /* invalid position in array   */

#define _OAR_LOCATE     1                      /* search for element          */
#define _OAR_INSERT     2                      /* search for insertion point  */

/********************** End of parameter definitions **************************/
/******************************************************************************/


/******************************************************************************/
/*********************** Start of macro definitions ***************************/
typedef int (*LP_locate) (void *elem1, void *elem2);

typedef int (*LP_compare) (void *elem1, void *elem2);

typedef void *(*LP_mapFunc) (void *elem);


#define OAR_Size(array) ( array ->size - array ->free)

/************************ End of macro definitions ****************************/
/******************************************************************************/


/******************************************************************************/
/********************* Start of structure definitions *************************/

typedef struct _OAR_array {          /* Structure for the ordered array       */
  int base;                          /* base number of elements in array      */
  int incr;                          /* quantum to shrink or stretch array    */
  int size;                          /* current size of the ordered array     */
  int free;                          /* number of free elements in the array  */
  int cursor;                        /* cursor for traversing sorted array    */
  void **data;                       /* pointer to the array of elements      */
  LP_compare compare;                 /* function to compare two elements      */
  LP_locate locate;                  /* function to locate an element         */
} OAR_array;

/********************** End of structure definitions **************************/
/******************************************************************************/


/******************************************************************************/
/********************** Start of function prototypes **************************/

OAR_array *  OAR_Open (int base,
                     int incr,
                     LP_locate locate,
                     LP_compare compare);

void  OAR_Close (OAR_array *array);

int  OAR_Merge (OAR_array *target,
               OAR_array *source,
               LP_mapFunc mapFunc);

int  OAR_AddElement (OAR_array *array,
                    void      *element);

void *  OAR_DelPosition (OAR_array *array,
                       int       position);

void *  OAR_DelElement (OAR_array *array,
                      void      *identifier);

void * OAR_DelCurrent (OAR_array *array);

void *  OAR_GetPosition (OAR_array *array,
                       int       position);

void *  OAR_GetElement (OAR_array *array,
                      void      *identifier);

void *  OAR_GetCurrent (OAR_array *array);

void *  OAR_GetPrev (OAR_array *array);

void *  OAR_GetNext (OAR_array *array);

void *  OAR_GetFirst (OAR_array *array);

void *  OAR_GetLast (OAR_array *array);

int  OAR_Resize (OAR_array *array);

int  OAR_Locate (OAR_array *array,
                void      *data,
                int       mode);

/*********************** End of function prototypes ***************************/
/******************************************************************************/


#endif
 
/******************************************************************************/
/************************* End of file ordarray.h *****************************/
/******************************************************************************/











