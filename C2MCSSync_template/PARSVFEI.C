#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parsvfei.h"

/* End of standard includes.                                                  */
/******************************************************************************/

/*****VERSION*********************************************************/

static char parsVFEIcid[] = "$Id: parsVFEI.c Ver 01.0 Rel 01.0 $";

/*********************************************************************/

/*******************************************************************************

 HSMS_VFI_GetToken:                                  Date:  July 2013
 ~~~~~~~~~~~~~~~~~                                Author:  Zhou An-Ping

 This function will retrieve the data portion of the field associated with
 the given VID.

 Arguments:
 ~~~~~~~~~~
 - vid (type is *char - Read only):
 The referenced string represents the VID to be retrieved.

 - type (type is int - Read only):
 The coded integer represents the type of data.  If the value
 of type is greater than 0, the type is a string and the value
 of type represents the size of the string.

 - data (type is *void - Read/Write):
 The referenced data element is of the specified type.  Its
 contents will be loaded with data from the next field.

 Returns:
 ~~~~~~~~
 Type is int - coded value representing the completion status of the
 function.

 Modifications:
 ~~~~~~~~~~~~~~
 - Author                     Date
 Description...

 *******************************************************************************/

int HSMS_VFI_GetToken(char *vid, int type, void *data) {

  //char token[_SIZ_MSGBUFF], *pattern, *value;
  char *token, *pattern, *value;

  message.msgPtr = 0;
  pattern = HSMS_VFI_SetPattern(vid, type);
  if (pattern[0] == 0)

    return _RET_RECNOTFND;  // (TRC_SetAlarm("HSMS_VFI_GetToken", _RET_RECNOTFND, vid));

  while (HSMS_VFI_GetNext(token, _SIZ_MSGBUFF) == _RET_SUCCESS) {
    if (strncmp(token, pattern, strlen(pattern)) == 0) {
      if ((value = strchr(token, '=')) == NULL)
        return _RET_RECNOTFND;  // (TRC_SetAlarm("HSMS_VFI_GetToken", _RET_RECNOTFND, vid));
      return (HSMS_VFI_SetToken(&value[1], type, data));
    }
  }
  return _RET_SUCCESS;  //(TRC_SetAlarm("HSMS_VFI_GetToken", _RET_RECNOTFND, vid));
}

/********************* End of function HSMS_VFI_GetToken ***********************/

/*******************************************************************************

 HSMS_VFI_GetArray:                                  Date:  July 2013
 ~~~~~~~~~~~~~~~~~                                Author:  Zhou An-Ping

 This function will retrieve the data portion of the array associated with
 the given VID.

 Arguments:
 ~~~~~~~~~~
 - vid (type is *char - Read only):
 The referenced string represents the VID to be retrieved.

 - maxElem (type is int - Read only):
 The numeric value represents the maximum number of element that
 are defined in the array.

 - type (type is int - Read only):
 The coded integer represents the type of data in the array.  If
 the value of type is greater than 0, the type is a string and
 the value of type represents the maximum size of each string
 element in the array.

 - numElem (type is *int - Read/Write):
 The referenced integer will be loaded with the number of elements
 retrieved from the VFEI message.

 - data (type is *void - Read/Write):
 The referenced data element is of the specified type.  Its
 contents will be loaded with data from the next field.

 Returns:
 ~~~~~~~~
 Type is int - coded value representing the completion status of the
 function.

 Modifications:
 ~~~~~~~~~~~~~~
 - Author                     Date
 Description...

 *******************************************************************************/

int HSMS_VFI_GetArray(char *vid, int maxElem, int type, int *numElem, void *data) {

  //char array[_SIZ_MSGBUFF], *strPtr;
  char *array, *strPtr;
  int parse, length;
  int status;

  /*..........................................................................*/
  /* Get the array of data from the message...                                */

  if ((status = HSMS_VFI_GetToken(vid, _SIZ_MSGBUFF, array)) != _RET_SUCCESS)
    return (status);
  length = strlen(array);

  /*..........................................................................*/
  /* Process the entire array...                                              */

  parse = 0;
  *numElem = 0;
  while (parse < length) {

    /*........................................................................*/
    /* Get rid of leading blanks and assign start of token...                 */

    while (isspace(array[parse]) != 0)
      if (++parse >= length)
        return (_RET_SUCCESS);
    strPtr = &array[parse];

    /*........................................................................*/
    /* We got the start of a token - find the end...                          */

    if (*numElem >= maxElem)
      return _RET_PARSEERR;  // (TRC_SetAlarm("HSMS_VFI_GetArray", _RET_PARSEERR, strPtr));
    while (1 == 1) {
      if (isspace(array[parse]) != 0)
        break;
      else if (array[parse] == '"')
        while ((++parse < length) && (array[parse] != '"'))
          ;
      if (++parse >= length)
        break;
    }

    /*........................................................................*/
    /* Assign the token and NULL terminate it...                              */

    array[parse] = 0;
    if (type > 0)
      status = HSMS_VFI_SetToken(strPtr, type, &((char *) data)[(*numElem) * type]);
    else
      switch (type) {
        case _PRS_UINT:
        case _PRS_BIN:
          status = HSMS_VFI_SetToken(strPtr, type, &((unsigned *) data)[(*numElem)]);
          break;
        case _PRS_INT:
          status = HSMS_VFI_SetToken(strPtr, type, &((int *) data)[(*numElem)]);
          break;
        case _PRS_FLOAT:
          status = HSMS_VFI_SetToken(strPtr, type, &((float *) data)[(*numElem)]);
          break;
        default:
          return _RET_PARSEERR;  // (TRC_SetAlarm("HSMS_VFI_GetArray", _RET_PARSEERR, array));
          break;
      }
    if (status != _RET_SUCCESS)
      return (status);
    (*numElem)++;
    parse++;
  }

  return (_RET_SUCCESS);
}

/********************* End of function HSMS_VFI_GetArray ***********************/

/*******************************************************************************

 HSMS_VFI_SetPattern:                                Date:  July 2013
 ~~~~~~~~~~~~~~~~~                                Author:  Zhou An-Ping

 This function will establish the search pattern as a function of the VID
 name and the VID type.

 Arguments:
 ~~~~~~~~~~
 - vid (type is *char - Read only):
 The referenced string represents the VID to be retrieved.

 - type (type is int - Read only):
 The coded integer represents the type of data.  If the value
 of type is greater than 0, the type is a string and the value
 of type represents the size of the string.

 Returns:
 ~~~~~~~~
 Type is *char - pointer to string containing the search pattern.  The
 string is a static variable which will be modified with subsequent
 calls.

 Modifications:
 ~~~~~~~~~~~~~~
 - Author                     Date
 Description...

 *******************************************************************************/

char *HSMS_VFI_SetPattern(char *vid, int type) {

  static char pattern[_SIZ_GENFIELD];

  if (type > 0)
    sprintf(pattern, "%.*s/", _SIZ_GENFIELD - 2, vid);
  else
    switch (type) {
      case _PRS_UINT:
        sprintf(pattern, "%.*s/U", _SIZ_GENFIELD - 3, vid);
        break;
      case _PRS_BIN:
        sprintf(pattern, "%.*s/B", _SIZ_GENFIELD - 3, vid);
        break;
      case _PRS_INT:
        sprintf(pattern, "%.*s/I", _SIZ_GENFIELD - 3, vid);
        break;
      case _PRS_FLOAT:
        sprintf(pattern, "%.*s/F", _SIZ_GENFIELD - 3, vid);
        break;
      default:
        pattern[0] = 0;
        break;
    }

  return (pattern);
}

/******************** End of function HSMS_VFI_SetPattern **********************/

/*******************************************************************************

 HSMS_VFI_SetToken:                                  Date:  July 2013
 ~~~~~~~~~~~~~~~~~                                Author:  Zhou An-Ping

 This function will convert a string to the specified type.  It is assumed
 that the string comes from VFEI.  If a string to string conversion is
 required, the enclosing quotes ("") or brackets ([]) will be removed.

 Arguments:
 ~~~~~~~~~~
 - source (type is *char - Read only):
 The referenced string contains the data to be converted.

 - type (type is int - Read only):
 The coded integer represents the type of data.  If the value
 of type is greater than 0, the type is a string and the value
 of type represents the size of the string.

 - data (type is *void - Read/Write):
 The referenced data element is of the specified type.  Its
 contents will be loaded with data from the next field.

 Returns:
 ~~~~~~~~
 Type is int - coded value representing the completion status of the
 function.

 Modifications:
 ~~~~~~~~~~~~~~
 - Author                     Date
 Description...

 *******************************************************************************/

int HSMS_VFI_SetToken(char *source, int type, void *data) {

  int length;

  length = strlen(source);
  if (length == 0)
    return _RET_PARSEERR;  // (TRC_SetAlarm("HSMS_VFI_SetToken", _RET_PARSEERR, source));

  if (type > 0) {
    if ((length > 1)
        && (((source[0] == '[') && (source[length - 1] == ']')) || ((source[0] == '"') && (source[length - 1] == '"')))) {
      if ((length - 2) >= type)
        return _RET_PARSEERR;  // (TRC_SetAlarm("HSMS_VFI_SetToken", _RET_PARSEERR, source));
      strncpy((char *) data, &source[1], length - 2);
      ((char *) data)[length - 2] = '\0';
    } else {
      if (length >= type)
        return _RET_PARSEERR;  // (TRC_SetAlarm("HSMS_VFI_SetToken", _RET_PARSEERR, source));
      strcpy((char *) data, source);
    }
  } else
    switch (type) {
      case _PRS_UINT:
      case _PRS_BIN:
        *((unsigned *) data) = (unsigned) atol(source);
        break;
      case _PRS_INT:
        *((int *) data) = (int) atol(source);
        break;
      case _PRS_FLOAT:
        sscanf(source, "%e", data);

        /*     *(data) = (float) atof (source);  */
        break;

      default:
        return _RET_PARSEERR;  // (TRC_SetAlarm("HSMS_VFI_SetToken", _RET_PARSEERR, source));
        break;
    }

  return (_RET_SUCCESS);
}

/********************* End of function HSMS_VFI_SetToken ***********************/

/*******************************************************************************

 HSMS_VFI_GetNext:                                   Date:  July 2013
 ~~~~~~~~~~~~~~~~~                                Author:  Zhou An-Ping

 This function will retrieve the next VFEI field in the message buffer.
 It is assumed that the message pointer has been initialized.

 Arguments:
 ~~~~~~~~~~
 - data (type is *char - Read/Write):
 The referenced data element will be loaded with the next token.

 - size (type is int - Read only):
 The integer represents the maximum number of bytes that data
 can support.

 Returns:
 ~~~~~~~~
 Type is int - coded value representing the completion status of the
 function.

 Modifications:
 ~~~~~~~~~~~~~~
 - Author                     Date
 Description...

 *******************************************************************************/

int HSMS_VFI_GetNext(char *data, int size) {

  int parse, tokSize;

  /*..........................................................................*/
  /* Check if given size is appropriate...                                    */
  if (size <= 0)
    return (_RET_PARSEERR);
  if (message.msgPtr >= message.msgLen)
    return (_RET_PARSEERR);
  /*..........................................................................*/
  /* Get rid of leading blanks...                                             */

  while (isspace(message.buffer[message.msgPtr]) != 0)
    if (++message.msgPtr >= message.msgLen)
      return (_RET_PARSEERR);

  /*..........................................................................*/
  /* We got the start of a token - find the end...                            */

  parse = message.msgPtr;
  while (1 == 1) {
    if (isspace(message.buffer[parse]) != 0)
      break;
    else if (message.buffer[parse] == '[')
      while ((++parse < message.msgLen) && (message.buffer[parse] != ']'))
        ;
    else if (message.buffer[parse] == '"')
      while ((++parse < message.msgLen) && (message.buffer[parse] != '"'))
        ;
    if (++parse >= message.msgLen)
      break;
  }

  /*..........................................................................*/
  /* Assign the token and NULL terminate it...                                */

  tokSize = parse - message.msgPtr;
  if (tokSize >= size)
    return (_RET_PARSEERR);
  strncpy(data, &message.buffer[message.msgPtr], tokSize);
  data[tokSize] = 0;
  message.msgPtr = parse;
  return (_RET_SUCCESS);
}

/********************** End of function HSMS_VFI_GetNext ***********************/

