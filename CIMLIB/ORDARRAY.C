/******************************************************************************/
/*                                                                            */
/*   ordarray.c:                                                              */
/*   ~~~~~~~~~~~                                                              */
/*                                                                            */
/*   This file contains the functions that will maintain and provide access   */
/*   to a generic dynamic array.  The array is sorted by using a function     */
/*   provided by the user.                                                    */
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

#include "ordarray.h"

/* End of include statements for local files.                                 */
/******************************************************************************/


/*******************************************************************************

   OAR_Open:                                          Date:  October 1994
   ~~~~~~~~~                                        Author:  Sebastien Spicer

   This function will create and initialize a new ordered array.  The minimum
   size will be allocated as specified by the calling application.

      Arguments:
      ~~~~~~~~~~
         - base (type is int - Read only):
              The value of base represents the minimum number of slots to 
              be kept in the array.

         - incr (type is int - Read only):
              The value of incr represents the number of slots to add or
              remove when resizing the array.

         - locate (type is *int - Read only):
              The referenced function will be called whenever an element
              needs to be located in the array.  It should be in the form
              int locate (void *identifier, void *element) where 0 is
              returned when identifier matches element.  The function 
              should return 1 when identifier is greater than element and
              -1 when identifier is less than element.

         - compare (type is *int - Read only):
              The referenced function will be called whenever an element
              needs to be inserted into the array.  It should be in the form
              int compare (void *elem1, void *elem2) where 0 is returned when
              both elements are equal.  THe function returns 1 if elem1 is
              greater than elem2 and -1 if elem1 is less than elem2.  If the
              compare function is NULL, elements appear in the list in the
              same order as they were entered.

      Returns:
      ~~~~~~~~
         Type is *OAR_array - pointer to the newly created structure.  NULL
         is returned if any errors were encountered.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

OAR_array *OAR_Open (int base,
                     int incr,
                         LP_locate locate,
                     LP_compare compare) {

  OAR_array *array;

  /*..........................................................................*/
  /* Allocate a new array structure...                                        */

  if ((array = (OAR_array *) malloc (sizeof (OAR_array))) == NULL)
    return (NULL);

  /*..........................................................................*/
  /* Initialize dynamic array administration parameters...                    */

  array->base = base;
  array->incr = incr;
  array->size = array->free = 0;
  array->cursor = 0;
  array->data = NULL;
  array->locate = locate;
  array->compare = compare;

  /*..........................................................................*/
  /* Allocate initial slot storage space...                                   */

  if (OAR_Resize (array) == _OAR_INVALIDPOS) {
    free (array);
    return (NULL);
  }

  return (array);
}

/************************** End of function OAR_Open **************************/


/*******************************************************************************

   OAR_Close:                                         Date:  October 1994
   ~~~~~~~~~~                                       Author:  Sebastien Spicer

   This function will shutdown an ordered array.  This includes freeing all
   memory allocated for the elements.

      Arguments:
      ~~~~~~~~~~
         - array (type is OAR_array - Read/Write):
              The referenced structure will be completely unallocaated.

      Returns:
      ~~~~~~~~
         Nothing.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void OAR_Close (OAR_array *array) {

  int i;
  
  if (array != NULL) {
    if (array->data != NULL) {
      for (i = 0; i < OAR_Size (array); i++) 
        if (array->data[i] != NULL)
          free (array->data[i]);
      free (array->data);
    }
    free (array);
  }
  return;
}

/************************* End of function OAR_Close **************************/


/*******************************************************************************

   OAR_Merge:                                         Date:  October 1994
   ~~~~~~~~~~                                       Author:  Sebastien Spicer

   This function will merge two ordered arrays.  The caller needs to provide
   a pointer to a function that will map source elements to target elements.
   This function ensures that no duplicates will be loaded into the target.

      Arguments:
      ~~~~~~~~~~
         - target (type is OAR_array - Read/Write):
              The referenced structure is an ordered array that may already
              contain elements.  It will be loaded with elements contained
              in source.

         - source (type is OAR_array - Read only):
              The referenced structure is an ordered array whose elements will
              be loaded into target.

         - mapFunc (type is *void - Read only):
              The referenced function will map elements from source to target.
              The function should allocate memory for the target element.  The
              function is in the form void *mapFunc (void *source) where the
              returned pointer is the target record mapped from the source
              record elem.  If the function fails, NULL should be returned.

      Returns:
      ~~~~~~~~
         Type is int - the returned integer will represent the new size of
         target after the merge.  If an error occured, OAR_INVALIDPOS is
         returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int OAR_Merge (OAR_array *target,
               OAR_array *source,
               LP_mapFunc mapFunc) {

  void *srcElem, *tgtElem;
  int srcCursor;
  int status;

  /*..........................................................................*/
  /* Make a copy of the source cursor (to be restored)...                     */

  srcCursor = source->cursor;
  status = OAR_Size (target);

  /*..........................................................................*/
  /* Copy all source elements into the target...                              */

  srcElem = OAR_GetFirst (source);
  while (srcElem != NULL) {
    if ((tgtElem = mapFunc (srcElem)) == NULL) {
      status = _OAR_INVALIDPOS;
      break;
    }
    if (OAR_GetElement (target, tgtElem) == NULL)
      if (OAR_AddElement (target, tgtElem) == _OAR_INVALIDPOS) {
        status = _OAR_INVALIDPOS;
        break;
      }
    srcElem = OAR_GetNext (source);
  }

  /*..........................................................................*/
  /* Restore the cursor and return the status...                              */

  if (status != _OAR_INVALIDPOS)
    status = OAR_Size (target);
  source->cursor = srcCursor;
  return (status);
}

/************************* End of function OAR_Merge **************************/


/*******************************************************************************

   OAR_AddElement:                                    Date:  October 1994
   ~~~~~~~~~~~~~~~                                  Author:  Sebastien Spicer

   This function will add an element to the ordered array.

      Arguments:
      ~~~~~~~~~~
         - array (type is *OAR_array - Read/Write):
              The referenced structure will be updated to include the element.

         - element (type is *void - Read only):
              The referenced structure is a data element to be inserted into
              the ordered array.

      Returns:
      ~~~~~~~~
         Type is int - the returned value represents the position of the
         inserted element in the array.  If the function was unsuccessful,
         the function returns _OAR_INVALIDPOS.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int OAR_AddElement (OAR_array *array,
                    void      *element) {

  int position;

  /*..........................................................................*/
  /* Resize array for new element if out of free space...                     */

  if (OAR_Resize (array) == _OAR_INVALIDPOS)
    return (_OAR_INVALIDPOS);

  /*..........................................................................*/
  /* Find insertion point for new element...                                  */

  position = OAR_Locate (array, element, _OAR_INSERT);

  /*..........................................................................*/
  /* Make space by moving everything over and insert data...                  */

  if (position < OAR_Size (array))
    memmove (&array->data[position+1], &array->data[position],
             (OAR_Size (array) - position) * sizeof (void *));
  array->data[position] = element;

  /*..........................................................................*/
  /* Adjust cursor to minimize effect of insertion...                         */

  if (array->cursor >= position)
    array->cursor++;

  /*..........................................................................*/
  /* Update administrative variables and return insertion position...         */

  array->free--;
  return (position);
}

/*********************** End of function OAR_AddElement ***********************/


/*******************************************************************************

   OAR_DelPosition:                                   Date:  October 1994
   ~~~~~~~~~~~~~~~~                                 Author:  Sebastien Spicer

   This function will delete the element associated with the given position.
   The associated element space will not be deallocated.

      Arguments:
      ~~~~~~~~~~
         - array (type is *OAR_array - Read only):
              The referenced structure will searched for the specified
              element.

         - position (type is int - Read only):
              The value of position refers to the position in the array to
              retrieve.

      Returns:
      ~~~~~~~~
         Type is *void - the pointer refers to the element deleted from the
         array.  If the function was unsuccessful, NULL is returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void *OAR_DelPosition (OAR_array *array,
                       int       position) {

  void *element;

  /*..........................................................................*/
  /* Make sure the array exists and that the position is sane...              */
 
  if ((array == NULL) || (position < 0) || (position >= OAR_Size (array)))
    return (NULL);

  /*..........................................................................*/
  /* Extract the data element...                                              */

  element = array->data[position];

  /*..........................................................................*/
  /* Delete by packing table and stomping over element space...               */

  memmove (&array->data[position], &array->data[position+1],
           (OAR_Size (array) - position - 1) * sizeof (void *));
  
  /*..........................................................................*/
  /* Adjust cursor to minimize effect of deletion...                          */

  if ((array->cursor >= position) && (array->cursor > 0))
    array->cursor--;

  /*..........................................................................*/
  /* Update administrative variables and resize array if necessary...         */

  array->free++;
  OAR_Resize (array);
  return (element);
}

/********************** End of function OAR_DelPosition ***********************/


/*******************************************************************************

   OAR_DelElement:                                    Date:  October 1994
   ~~~~~~~~~~~~~~~                                  Author:  Sebastien Spicer

   This function will delete the element associated with the given identifier.
   The associated element space will not be deallocated.

      Arguments:
      ~~~~~~~~~~
         - array (type is *OAR_array - Read only):
              The referenced structure will searched for the specified
              element.

         - identifier (type is *void - Read only):
              The referenced structure is used as the identifier to be
              passed to the locate function.

      Returns:
      ~~~~~~~~
         Type is *void - the pointer refers to the element deleted from the
         array.  If the function was unsuccessful, NULL is returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Sebastien Spicer           November 1995
           If the element is not found, return NULL instead of trying to
           delete garbage.

*******************************************************************************/

void *OAR_DelElement (OAR_array *array,
                      void      *identifier) {

  int position;
  void *element;

  /*..........................................................................*/
  /* Make sure that the array exists and there is data in the array...        */

  if ((array == NULL) || (OAR_Size (array) <= 0))
    return (NULL);
 
  /*..........................................................................*/
  /* Search for the element...                                                */

  position = OAR_Locate (array, identifier, _OAR_LOCATE);
  if (position == _OAR_INVALIDPOS)
    return (NULL);
  element = array->data[position];

  /*..........................................................................*/
  /* Delete by packing table and stomping over element space...               */

  memmove (&array->data[position], &array->data[position+1],
           (OAR_Size (array) - position - 1) * sizeof (void *));
  
  /*..........................................................................*/
  /* Adjust cursor to minimize effect of deletion...                          */

  if ((array->cursor >= position) && (array->cursor > 0))
    array->cursor--;

  /*..........................................................................*/
  /* Update administrative variables and resize array if necessary...         */

  array->free++;
  OAR_Resize (array);
  return (element);
}

/********************** End of function OAR_DelElement ***********************/


/*******************************************************************************

   OAR_DelCurrent:                                    Date:  October 1994
   ~~~~~~~~~~~~~~~                                  Author:  Sebastien Spicer

   This function will delete the element associated with the cursor position.
   The associated element space will not be deallocated.

      Arguments:
      ~~~~~~~~~~
         - array (type is *OAR_array - Read only):
              The referenced structure will searched for the specified
              element.

      Returns:
      ~~~~~~~~
         Type is *void - the pointer refers to the element deleted from the
         array.  If the function was unsuccessful, NULL is returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void *OAR_DelCurrent (OAR_array *array) {

  int position;
  void *element;

  /*..........................................................................*/
  /* Make sure the array exists and that the cursor is sane...                */
 
  position = array->cursor;
  if ((array == NULL) || (position < 0) || (position >= OAR_Size (array)))
    return (NULL);
  
  /*..........................................................................*/
  /* Extract the data element...                                              */

  element = array->data[position];

  /*..........................................................................*/
  /* Delete by packing table and stomping over element space...               */

  memmove (&array->data[position], &array->data[position+1],
           (OAR_Size (array) - position - 1) * sizeof (void *));
  
  /*..........................................................................*/
  /* Adjust cursor to minimize effect of deletion...                          */

  if ((array->cursor >= position) && (array->cursor > 0))
    array->cursor--;

  /*..........................................................................*/
  /* Update administrative variables and resize array if necessary...         */

  array->free++;
  OAR_Resize (array);
  return (element);
}

/********************** End of function OAR_DelCurrent ************************/


/*******************************************************************************

   OAR_GetPosition:                                   Date:  October 1994
   ~~~~~~~~~~~~~~~~                                 Author:  Sebastien Spicer

   This function will retrieve the element associated with the given position.

      Arguments:
      ~~~~~~~~~~
         - array (type is *OAR_array - Read only):
              The referenced structure will searched for the specified
              element.

         - position (type is int - Read only):
              The value of position refers to the position in the array to
              retrieve.

      Returns:
      ~~~~~~~~
         Type is *void - the pointer refers to the returned element from the
         array.  If the function was unsuccessful, NULL is returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void *OAR_GetPosition (OAR_array *array,
                       int       position) {

  /*..........................................................................*/
  /* Make sure the array exists and that the position is sane...              */

  if ((array == NULL) || (position < 0) || (position >= OAR_Size (array)))
    return (NULL);

  /*..........................................................................*/
  /* Extract the data element...                                              */

  array->cursor = position;
  return (array->data[position]);
}

/********************** End of function OAR_GetPosition ***********************/


/*******************************************************************************

   OAR_GetElement:                                    Date:  October 1994
   ~~~~~~~~~~~~~~~                                  Author:  Sebastien Spicer

   This function will retrieve the element associated with the given
   identifier.

      Arguments:
      ~~~~~~~~~~
         - array (type is *OAR_array - Read only):
              The referenced structure will searched for the specified
              element.

         - identifier (type is *void - Read only):
              The referenced structure is used as the identifier to be
              passed to the locate function.

      Returns:
      ~~~~~~~~
         Type is *void - the pointer refers to the returned element from the
         array.  If the function was unsuccessful, NULL is returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void *OAR_GetElement (OAR_array *array,
                      void      *identifier) {

  int position;
  
  /*..........................................................................*/
  /* Make sure that the array exists and there is data in the array...        */

  if ((array == NULL) || (OAR_Size (array) <= 0))
    return (NULL);
 
  /*..........................................................................*/
  /* Search for the element...                                                */

  position = OAR_Locate (array, identifier, _OAR_LOCATE);

  /*..........................................................................*/
  /* If the element is found, return it to the caller...                      */

  if (position == _OAR_INVALIDPOS)
    return (NULL);
  else {
    array->cursor = position;
    return (array->data[position]);
  }
}

/*********************** End of function OAR_GetElement ***********************/


/*******************************************************************************

   OAR_GetCurrent:                                    Date:  October 1994
   ~~~~~~~~~~~~~~~                                  Author:  Sebastien Spicer

   This function will retrieve the element pointed to by the array's
   internal cursor.

      Arguments:
      ~~~~~~~~~~
         - array (type is *OAR_array - Read only):
              The referenced structure will be searched for the current
              element.

      Returns:
      ~~~~~~~~
         Type is *void - the pointer refers to the returned element from the
         array.  If the function was unsuccessful, NULL is returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void *OAR_GetCurrent (OAR_array *array) {

  /*..........................................................................*/
  /* Make sure that the array exists and there is data in the array...        */
  
  if ((array == NULL) || (OAR_Size (array) <= 0))
    return (NULL);
 
  /*..........................................................................*/
  /* If the cursor is sane, return the associated element...                  */

  if ((array->cursor < 0) || (array->cursor >= OAR_Size (array)))
    return (NULL);
  else
    return (array->data[array->cursor]);
}

/********************** End of function OAR_GetCurrent ************************/


/*******************************************************************************

   OAR_GetPrev:                                       Date:  October 1994
   ~~~~~~~~~~~~                                     Author:  Sebastien Spicer

   This function will retrieve the element previous to that pointed to by
   the array's internal cursor.

      Arguments:
      ~~~~~~~~~~
         - array (type is *OAR_array - Read only):
              The referenced structure will be searched for the previous
              element.

      Returns:
      ~~~~~~~~
         Type is *void - the pointer refers to the returned element from the
         array.  If the function was unsuccessful, NULL is returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void *OAR_GetPrev (OAR_array *array) {

  /*..........................................................................*/
  /* Make sure that the array exists and there is data in the array...        */
  
  if ((array == NULL) || (OAR_Size (array) <= 0))
    return (NULL);
 
  /*..........................................................................*/
  /* If the cursor is sane, return the associated element...                  */

  if ((array->cursor < 1) || (array->cursor >= OAR_Size (array)))
    return (NULL);
  else
    return (array->data[--array->cursor]);
}

/************************ End of function OAR_GetPrev *************************/


/*******************************************************************************

   OAR_GetNext:                                       Date:  October 1994
   ~~~~~~~~~~~~                                     Author:  Sebastien Spicer

   This function will retrieve the element next to that pointed to by
   the array's internal cursor.

      Arguments:
      ~~~~~~~~~~
         - array (type is *OAR_array - Read only):
              The referenced structure will be searched for the next element.

      Returns:
      ~~~~~~~~
         Type is *void - the pointer refers to the returned element from the
         array.  If the function was unsuccessful, NULL is returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void *OAR_GetNext (OAR_array *array) {

  /*..........................................................................*/
  /* Make sure that the array exists and there is data in the array...        */
  
  if ((array == NULL) || (OAR_Size (array) <= 0))
    return (NULL);
 
  /*..........................................................................*/
  /* If the cursor is sane, return the associated element...                  */

  if ((array->cursor < 0) || (array->cursor >= (OAR_Size (array) - 1)))
    return (NULL);
  else
    return (array->data[++array->cursor]);
}

/************************ End of function OAR_GetNext *************************/


/*******************************************************************************

   OAR_GetFirst:                                      Date:  October 1994
   ~~~~~~~~~~~~~                                    Author:  Sebastien Spicer

   This function will retrieve the first element in the array.

      Arguments:
      ~~~~~~~~~~
         - array (type is *OAR_array - Read only):
              The referenced structure will be searched for the first element.

      Returns:
      ~~~~~~~~
         Type is *void - the pointer refers to the returned element from the
         array.  If the function was unsuccessful, NULL is returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void *OAR_GetFirst (OAR_array *array) {

  /*..........................................................................*/
  /* Make sure that the array exists and there is data in the array...        */
  
  if ((array == NULL) || (OAR_Size (array) <= 0))
    return (NULL);
 
  /*..........................................................................*/
  /* If the cursor is sane, return the associated element...                  */

  array->cursor = 0;
  return (array->data[0]);
}

/************************ End of function OAR_GetFirst ************************/


/*******************************************************************************

   OAR_GetLast:                                       Date:  October 1994
   ~~~~~~~~~~~~                                     Author:  Sebastien Spicer

   This function will retrieve the last element in the array.

      Arguments:
      ~~~~~~~~~~
         - array (type is *OAR_array - Read only):
              The referenced structure will be searched for the last element.

      Returns:
      ~~~~~~~~
         Type is *void - the pointer refers to the returned element from the
         array.  If the function was unsuccessful, NULL is returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void *OAR_GetLast (OAR_array *array) {

  /*..........................................................................*/
  /* Make sure that the array exists and there is data in the array...        */
  
  if ((array == NULL) || (OAR_Size (array) <= 0))
    return (NULL);
 
  /*..........................................................................*/
  /* If the cursor is sane, return the associated element...                  */

  array->cursor = OAR_Size (array) - 1;
  return (array->data[array->cursor]);
}

/************************ End of function OAR_GetLast *************************/


/*******************************************************************************

   OAR_Resize:                                        Date:  October 1994
   ~~~~~~~~~~~                                      Author:  Sebastien Spicer

   This function determines if the array needs to be shrunk or stretched.
   This includes the case where no space has yet been allocated for the
   elements.  If the function finds that a resize is required, it will be
   performed.  If memory allocation is unsuccessful, the operation will
   cause a fatal error.

      Arguments:
      ~~~~~~~~~~
         - array (type is *OAR_array - Read/Write):
              The referenced structure will be resized if required.

      Returns:
      ~~~~~~~~
         Type is int - the returned value represents the new size of the
         array.  If the operation fails, _OAR_INVALIDPOS is returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int OAR_Resize (OAR_array *array) {

  int new_size, new_free;

  /*..........................................................................*/
  /* Check to see if we need to stretch the array (if so, set temp values)... */

  if (array->free <= 0) {
    if (array->size < array->base)
      new_size = array->base;
    else
      new_size = array->size + array->incr;
    new_free = array->free + new_size - array->size;
    
  /*..........................................................................*/
  /* Check to see if we need to shrink the array (if so, set temp values)...  */

  } else if ((array->size > array->base) && (array->free >= array->incr)) {
    new_size = (array->base > (array->size - array->incr)) ?
      array->base : (array->size - array->incr);
    new_free = array->free + new_size - array->size;
    
  /*..........................................................................*/
  /* Otherwise, get out of here...                                            */

  } else
    return (array->size);

  /*..........................................................................*/
  /* Resize the array...                                                      */

  array->data = (void **) realloc (array->data, new_size * sizeof (void *));
  if (array->data == NULL)
    return (_OAR_INVALIDPOS);

  /*..........................................................................*/
  /* Set the new size in the array structure...                               */
  
  array->size = new_size;
  array->free = new_free;
  return (array->size);
}

/************************ End of function OAR_Resize **************************/


/*******************************************************************************

   OAR_Locate:                                        Date:  October 1994
   ~~~~~~~~~~~                                      Author:  Sebastien Spicer

   This function will locate an element in a sorted array of pointers of
   elements.  This function operates in two modes.  The first mode returns
   the position of the found element.  The second mode will return the 
   insertion point of a new element.

      Arguments:
      ~~~~~~~~~~
         - array (type is *OAR_array - Read only):
              The referenced structure contains the list of elements to
              be searched.

         - data (type is *void - Read only):
              The referenced structure contains either the element to be
              inserted or an identifier used to perform the search.  This
              is a function of the mode.

         - mode (type is int - Read only):
              The value of the integer indicates the mode to be used when
              searching the array.  Possible values are _OAR_LOCATE for
              finding an element (data is expected to be a pointer to an
              identifier) or _OAR_INSERT for finding an insertion point
              (data is expected to be a pointer to an element).

      Returns:
      ~~~~~~~~
         Type is int - the returned is an integer representing the element 
         position in the array or the insertion point.  If the operation
         fails, _OAR_INVALIDPOS is returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int OAR_Locate (OAR_array *array,
                void      *data,
                int       mode) {

  int low, mid, high, result;
        int save_mid;

  high = OAR_Size (array) - 1;
  low = 0;

  /*..........................................................................*/
  /* Handle the case where an element needs to be found...                    */

  if (mode == _OAR_LOCATE) {
    if (array->locate == NULL)
      return (_OAR_INVALIDPOS);

    /*........................................................................*/
    /* Handle the case where the list is not ordered (compare is NULL)...     */

    if (array->compare == NULL)
      while (low <= high) {
        result = array->locate (data, array->data[low]);
        if (result == 0)
          return (low);
        else
          low++;
      }

    /*........................................................................*/
    /* Otherwise, search an ordered list...                                   */

    else
      while (low <= high) {
        save_mid= mid = (low + high) / 2;
        result = array->locate (data, array->data[mid]);
        mid=save_mid;
        if (result > 0)
          low = mid + 1;
        else if (result < 0)
          high = mid - 1;
        else
          return (mid);
      }

    return (_OAR_INVALIDPOS);
    
  /*..........................................................................*/
  /* Otherwise, handle the case where an insertion point needs to be found... */

  } else {

    /*........................................................................*/
    /* Handle the case where the list is not ordered (compare is NULL)...     */

    if (array->compare == NULL)
      return (high + 1);

    /*........................................................................*/
    /* Otherwise, search an ordered list...                                   */

    else
      while (low <= high) {
        result = array->compare (data, array->data[low]);
        if (result >= 0)
          low++;
        else
          return (low);
      }

    return (low);
  }
}

/************************* End of function OAR_Locate *************************/


/******************************************************************************/
/*************************** End of file ordarray.c ***************************/
/******************************************************************************/









