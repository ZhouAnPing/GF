/******************************************************************************/
/*                                                                            */
/*   cimdefs.h:                                                               */
/*   ~~~~~~~~~~                                                               */
/*                                                                            */
/*   This file contains define statements for the CIM library functions.      */
/*   This file is the primary reference for all field sizes and other         */
/*   related data.                                                            */
/*                                                                            */
/******************************************************************************/


#ifndef _CIM_CIMDEFS
#define _CIM_CIMDEFS
static char *cimdefs_h="$Id: cimdefs.h Ver 01.0 Rel 01.0 $";

/******************************************************************************/
/********************** Start of CIM library return codes *********************/

#define _RET_SUCCESS       0                   /* successful execution        */
#define _RET_SHUTDOWN      1                   /* performing shutdown         */
#define _RET_FAILURE       2                   /* undetermined error          */
#define _RET_GENERIC       3                   /* user defined error          */
#define _RET_BADFILENAME   4                   /* bad file name               */
#define _RET_FILEOPENERR   5                   /* could not open file         */
#define _RET_FILEWRITERR   6                   /* could not write to file     */
#define _RET_NOMEMORY      7                   /* ran out of memory           */
#define _RET_FLDMISSING    8                   /* required field missing      */
#define _RET_FLDNOTXPCT    9                   /* field not expected          */
#define _RET_FLDTOOLONG    10                  /* data exceeded field length  */
#define _RET_FLDBADINTV    11                  /* bad int data for field      */
#define _RET_FLDBADSTRV    12                  /* bad str data for field      */
#define _RET_BADMBOXNAME   13                  /* bad mailbox name            */
#define _RET_MBOXNOTOPN    14                  /* mailbox not opened yet      */
#define _RET_MBOXNOTSND    15                  /* could not sent to mailbox   */
#define _RET_MBOXNOTRCV    16                  /* could not receive mailbox   */
#define _RET_TIMEOUT       17                  /* timeout error               */
#define _RET_HOSTERROR     18                  /* host generated error        */
#define _RET_PARSEERR      19                  /* parsing error               */
#define _RET_PARSEEND      20                  /* end of parse buffer         */
#define _RET_PARSESEQ      21                  /* wrong sequence number       */
#define _RET_RECNOTFND     22                  /* record not found            */
#define _RET_RECEXISTS     23                  /* record already exists       */
#define _RET_DMQERROR      24                  /* DMQ communication error     */
#define _RET_CMDWRONG      25                  /* received wrong command      */
#define _RET_CMDINVALID    26                  /* received invalid command    */
#define _RET_NOARMID       27                  /* No arm in VFEI event        */
#define _RET_READTAGERR    28                  /* Error reading Tag           */
#define _RET_BADARMID      29                  /* ARM Id is wrong             */
#define _RET_LOAD_FAILURE  30                  /* Loading Failure             */
#define _RET_TAGREADFAIL   31                  /* Readtag Failure             */
#define _RET_TAGWRITEFAIL  32                  /* Writetag Failure            */
#define _RET_NULLCNT       33                  /* Received Null Value         */
#define _RET_NOARGS        34                  /* No arguments supplied       */
#define _RET_TOKENTYPERR   35                  /* Token of type missing     */
#define _RET_GENERICSSS    36                  /* Generic %s %s %s      */
#define _RET_GENERICSDS    37                  /* Generic %s %d %s      */
#define _RET_DMQNOMOREMSG  38                 /* No more DMQ Messages     */
/********************** End of CIM library return codes ***********************/
/******************************************************************************/


/******************************************************************************/
/********************** Start of CIM library dimensions ***********************/

#define _MAX_VIDELEM       100                 /* max elements in VID array   */
#define _MAX_DCVAR         25                  /* max number of DCV for SPC op*/
#define _MAX_ALARMS        50                  /* max alarms per equipment    */
#define _MAX_ALTRECP       25                  /* max alt. recipes for a step */
#define _MAX_TRACE         25                  /* max trace records per eqpt. */
#define _MAX_LOTSINBATCH   12                  /* max number of lots in batch */

#define _MAX_VIDS          25                  /* max number of VID           */
#define _MAX_TAG_LINES     12                  /* max number of Tag lines     */
#define _MAX_NO_LOTS       100 

#define _MAX_ADCSITE       49                  /* max number of sites in ADC  */
#define _MAX_ADCWAFR       25                  /* max number of wafers in ADC */
#define _MAX_SAMPLE_LEN     9                   /* meas data size   */

#define _MAX_ALMALRCNT     99999               /* max value for alarm counter */
#define _MAX_PARAMS        7                   /* max value for SPSR parameters */

#define _MAX_COLLTYPE_LEN  9         /* use in adcsqlOracle.pc for predata function */

#define _MAX_NO_USERIDS    100
#define _MAX_NO_RECPIDS    100
#define _MAX_NO_BATCHIDS   100
#define _MAX_NO_LOTIDS     100
#define _MAX_NO_LOTTYPES   100
#define _MAX_NO_PROCIDS    100
#define _MAX_NO_PARTIDS    100
#define _MAX_NO_INSTNUMS   100

#define _MAX_USERS         50          /* Max no. of user data         */
#define _MAX_USERGROUPS    50          /* Max no. of user groups       */
#define _MAX_ACTIONS       50               /* Max no. of actions for alarm */

#define _MAX_DATAVALUE        490
#define _MAX_DATATYPE          20
#define _MAX_OOCCOUNT          50
#define _MAX_OOSCOUNT          50
#define _MAX_SOURCESECSMODEL  100
#define _MAX_CHARTTYPE         20
#define _MAX_SAMPLELVL        100
#define _MAX_SAMPLELVLNUM     100
#define _MAX_GRAPH                        100
#define _MAX_OOC              100
#define _MAX_OOS              100
#define _MAX_PRECALC          100
#define _MAX_GRAPHTYPE        100
#define _MAX_EQPT             200
#define _MAX_RULE             200
#define _MAX_LOT              200

 
/********************** End of CIM library dimensions *************************/
/******************************************************************************/


/******************************************************************************/
/********************** Start of CIM library field sizes **********************/

#define _SIZ_MSGBUFF       32000               /* maximum message buffer size */
#define _SIZ_FILENAME      256                 /* maximum file name length    */
#define _SIZ_GENFIELD      256                 /* maximum field length        */
#define _SIZ_DMQNAME       256                 /* maximum queue name length   */
#define _SIZ_APPENDIX      256                 /* maximum VFEI Appendix length*/
#define _SIZ_LOGBLOCK      74                  /* # of bytes per log line     */
#define _SIZ_IPADDR        25                  /* size of an IP address       */
#define _SIZ_VMSDATE       24                  /* size of VMS date string     */
#define _SIZ_GENDATE       21                  /* size of a generic date/time */
#define _SIZ_CIMDATE       18                  /* size of CIM date string     */
#define _SIZ_GENFLAG       2                   /* size of generic flag var.   */

#define _SIZ_TRIGGEREVENT  17                  /* size of trigger event string*/
#define _SIZ_STOPEVENT     17                  /* size of stop event string   */
#define _SIZ_TRACEDESC     51                  /* size of trace description   */

#define _SIZ_EQPTMID       32                  /* equipment machine id size   */
#define _SIZ_ADCSTS        25                  /* size of ADC status field    */
#define _SIZ_ADCKEY        13                  /* size of ADC source key      */
#define _SIZ_EQPTSECS      25                  /* size of equipment SECS type */
#define _SIZ_VIDVALUE      256                 /* size of VID value           */
#define _SIZ_VIDNAMES      17                  /* size of VID names           */
#define _SIZ_VIDFORMAT     2                   /* size of VID format          */
#define _SIZ_DCVAR         13                  /* size of DC variable name    */
#define _SIZ_ALARMID       11                  /* size of alarm identifier    */
#define _SIZ_ALARMCNTR     11                  /* size of alarm counter       */
#define _SIZ_ALARMTEXT    128                              /* Size of alarm text          */
#define _SIZ_PORTID        6                   /* Size of Port                */
#define _SIZ_SEVERITYDESC  50                              /* Size of severity description */
#define _SIZ_ALARMCMNT    256                              /* Size of alarm comment       */
#define _SIZ_EQPTCATG      2                   /* size of equipment category  */
#define _SIZ_ADCSRCE       2                   /* size of ADC data source fld */
#define _SIZ_EQPTARM       2                   /* size of equipment arm field */
#define _SIZ_COLLTYPE      6                   /* size of equipment arm field */
#define _SIZ_COLLFLAG      2                   /* size of collection flag     */
#define _SIZ_DATACATG      4                   /* size of data category field */
#define _SIZ_DATAFMT       3                   /* size of format field        */
#define _SIZ_EVACTION      4                   /* size of event action field  */
#define _SIZ_PARNAME       16                                   /* size of SPSR par name       */
#define _SIZ_PARVAL                16                                   /* size of SPSR parameter value*/
#define _SIZ_SPSRNO                16                                   /* size of SPSR number         */
#define _SIZ_RWRKCODE      3                                    /* Size of rework code         */
#define _SIZ_RWRKDES       21                                   /* Rework code description     */

#define _SIZ_EQPCMTLIN     51                  /* size of equipment comment   */
#define _SIZ_LOTCMTLIN     51                  /* size of lot comment         */
#define _SIZ_PASSWORD      33                  /* size of PROMIS password     */
#define _SIZ_PARTID        33                  /* size of part id token       */
#define _SIZ_PROCID        33                  /* size of procedure id token  */
#define _SIZ_PROMPARM      33                  /* size of procedure parameter */
#define _SIZ_PARTNAME      30                  /* size of promis part name    */
#define _SIZ_PRCDNAME      30                  /* size of promis prcd name    */
#define _SIZ_JOBFILE       21                  /* size of job file on stepper */
#define _SIZ_RETICLEID     32
#define _SIZ_NUMEQPCMT     15                  /* max number of comments      */
#define _SIZ_OPERATORID    13                  /* operator id maximum size    */
#define _SIZ_USERID        13                  /* user id maximum size        */
#define _SIZ_SPECLOTID     26                  /* special lot identifier      */
                                               /* EQIPT_ID_RECIPE FOR 8+1+16+1*/
                                               /* PRE POST DATA COLLECTION    */
#define _SIZ_LOTID         13                  /* lot id maximum size         */
#define _SIZ_LOTTYPE        3                  /* lot type length             */
#define _SIZ_COMPID        13                  /* Component (wafer) id size   */
#define _SIZ_PROMLOC       11                  /* PROMIS location name size   */
#define _SIZ_STAGE         11                  /* size of stage name          */
#define _SIZ_AREA          10                  /* Size of area                */
#define _SIZ_EQPTID        9                   /* equipment id maximum size   */
#define _SIZ_GRAPHID        49                   /* graph id maximum size   */
#define _SIZ_MACHID        9                   /* mid maximum size   */
#define _SIZ_EQPTTYPE      9                   /* equipment type maximum size */
#define _SIZ_MCSLOC        9                   /* size of eqpt port (MCS)     */
#define _SIZ_EQPTRECP      21                  /* size of Equipment recipe id */
#define _SIZ_METEQPTRECP   80                  /* size of Metrology Eqp rcp d */
#define _SIZ_RECPID        9                   /* size of recipe id           */
#define _SIZ_SPECRECP      36                  /* size of special recipe id   */
#define _SIZ_SECSMODEL     9        
#define _SIZ_INSTNUM       26                       
#define _SIZ_EQPTRECIPE    26                  /* Size of Eqipment rcp - EM.  */
#define _SIZ_PRCDINST      8                   /* size of a promis instruction*/
#define _SIZ_EQPTSTATE     8                   /* equipment state token size  */
#define _SIZ_EQPTMODE      8                   /* EQUIPMent Mode              */
#define _SIZ_PRCINSTNUM    8                   /* size of instruction number  */
#define _SIZ_LOTSTATE      6                   /* size of lot state field     */
#define _SIZ_APPLID        5                   /* size of application id      */
#define _SIZ_RECPNAME      6                   /* size of promis recipe name  */
#define _SIZ_PRCDVER       4                   /* size of promis prcd version */
#define _SIZ_MATTYPE       3                   /* size of material type       */

#define _SIZ_TAGTEXT       17                  /* size of text in tag per line*/
#define _SIZ_TAGTEXT_LEN   _SIZ_TAGTEXT        /* size of text in tag per line*/
#define _SIZ_MODESTR       15                  /* size of the mode string     */
#define _SIZ_BATCHID       11                  /* size of Batch Id VFEI field */
#define _SIZ_TRACEID       11                  /* size of trace Id VFEI field */
#define _SIZ_LAYERID        8                  /* size of Layer Id VFEI field */

#define _SIZ_RTCLCMT       257                 /* reticle comment length      */
#define _SIZ_RTCLID        21                  /* size of reticle identifier  */
#define _SIZ_RTCLUID       21                  /* size of reticle user id     */
#define _SIZ_RTCLLOC       13                  /* size of reticle location    */
#define _SIZ_RTCLSTA       11                  /* size of reticle status      */
#define _SIZ_RTCLJST       2                   /* size of reticle job status  */
#define _SIZ_DCOPNAME      25              

#define _SIZ_OCAPSRC       9                   /* Size of OCAP source         */
#define _SIZ_OCAPSRCKEY    50                  /* Size of OCAP source key     */
#define _SIZ_OCAPSUBKEY    11                              /* Size of OCAP sub key        */
#define _SIZ_OCAPRUNKEY    9                               /* Size of OCAP run key        */
#define _SIZ_SETUPKEY      9                               /* SECS model or equipment id  */
#define _SIZ_USERINST      51                              /* Size of User instruction    */
#define _SIZ_USERDATA      51                              /* Size of user entered data   */
#define _SIZ_USERGROUP     13                              /* Size of user group          */
#define _SIZ_ACTNAME       26                              /* Size of system action name  */
#define _SIZ_ACTVAL        256                 /* Size of system action value */
#define _SIZ_SRCCATG       9                   /* Size of EQPALARM or SPC tag */
#define _SIZ_SRCSUBCATG    4                               /* SIZE of OOS/OOC or Severity */

#define _SIZ_MSETID        14                  /* size of mask set id         */
#define _SIZ_MSETPROC      31                  /* size of mask set proc name  */
#define _SIZ_MSETJFIL      33                  /* size of mask set job file   */
#define _SIZ_MSETVENDR     31                  /* size of mask set vendor     */
#define _SIZ_MSETCUST      31                  /* size of mask set customer   */
#define _SIZ_MSETTPAS      2                   /* size of test passed flag    */
#define _SIZ_PURPOSE       5                   /* SIZE OF PURPOSE             */
#define _SIZ_GENFLAG       2                   /* Size  for General Flag Defns*/
#define _SIZ_GRAPHTYPE     12                  /* Size  for General Flag Defns*/
#define _SIZ_PRECALC        8                  /* Size  for General Flag Defns*/
#define _SIZ_GRAPHID       49                   /* Size  for General Flag Defns*/

#define _SIZ_BATCHSTATE    10                  /* Size of batch State         */
#define _STR_POST          "POST"              /* ADC Colltype POST           */
#define _STR_PRE           "PRE"               /* ADC Colltype PRE            */
#define _STR_PST           "PST"               /* ADC Colltype PST            */
#define _STR_LOTRESDETAIL   "$LOTRESDETAIL"     /* Lot reservation detail
                                                                                            Parameter name */
#define _ENV_ORACLEUSR      "ORACLE_USR"
#define _ENV_ORACLEPWD      "ORACLE_PWD"
#define _ENV_ORACLESID      "ORACLE_SID"
#define _STR_MIN                        "MIN"
#define _STR_MAX                        "MAX"
#define _STR_BAR                        "BAR"
#define _STR_SIGMA                      "SIG"
#define _STR_RAW                        "RAW"
#define _STR_DER                        "DER"

#define _EQSTATE_IDLE       "IDLE"
#define _EQSTATE_RUNNING    "RUNNING"
#define _EQSTATE_AVAIL      "AVAIL"
#define _EQSTATE_OOC        "OOC"
#define _EQSTATE_OOS        "OOS"
#define _EQSTATE_SAVAIL     "SAVAIL"

#define _NUM_MIN                         2
#define _NUM_MAX                         4
#define _NUM_BAR             8
#define _NUM_SIGMA           16
#define _NUM_RAW             32

#define _BIG_R4                         1.0e30
#define _CHECK_R4                       (_BIG_R4 -1.0)


#define _SIZ_EVENTID            21
#define _SIZ_DATAID                     65      /* size of DataIDtoken for GUI transactions */
#define _SIZ_BUTTONNAME         25              /* Size of button name */
#define _SIZ_FIELDNAME          25              /* Size of field name */
#define _SIZ_FIELDVALUE         25              /* Size of field value */
#define _SIZ_COLOR                      33      /* Size of color name */
#define _SIZ_ARMSTATUS          11              /* Size of Arm status */
#define _SIZ_CMDTYPE            33              /* size of VFEI command type */
#define _SIZ_CMDID                      13      /* Command Id for VFEI message */
#define _SIZ_MID                        33      /* Machine Id for VFEI mesage */
#define _SIZ_RESTYPE            9               /* Size of resource type for activation */
#define _SIZ_PARAMTYPE          9               /* Size of parameter type for activation */
#define _SIZ_MODENAME           9               /* Size of Process-Run mode name  */
#define _SIZ_STARTTIME          25              /* Size of Start time */
#define _SIZ_ENDTIME            25              /* Size of End Time */
#define _SIZ_LOTCOUNT            3              /* Size of lot count */
#define _SIZ_WAFERID             3              /* Size of WaferID for adcsqlsrv.h file  */
#define _SIZ_MEASTYPE            5
#define _SIZ_DATATYPE            5
#define _SIZ_DESCRIPT           50
#define _SIZ_DCVARCATG           3
#define _SIZ_SPCFLAG             2
#define _SIZ_CHARTTYPE          20
#define _SIZ_SAMPLELEVEL        20
#define _SIZ_SOURCESECSMODEL    20
#define _SIZ_DCVARTYPE          20
#define _SIZ_OOS                 2
#define _SIZ_OOC                 2
#define _SIZ_PRECALC             8
#define _SIZ_SPECTYPEFLAG        2
#define _SIZ_RULEDESC           50
#define _SIZ_RULENAME           12
#define _SIZ_COMMENT           100
#define _SIZ_OPERID             12
#define _SIZ_SRCEQPTID           8
#define _SIZ_SRCEQPTTYPE         8


/********************** End of CIM library field sizes ************************/
/******************************************************************************/

#endif

/******************************************************************************/
/**************************** End of file cimdefs.h ***************************/
/******************************************************************************/





