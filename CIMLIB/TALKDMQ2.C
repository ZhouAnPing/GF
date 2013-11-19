/******************************************************************************/
/*                                                                            */
/*   talkDMQ2.c:                                                              */
/*   ~~~~~~~~~~                                                               */
/*                                                                            */
/*   This file contains the functions required to communicate with            */
/*   applications on the DMQ.                                                 */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/* The following are the standard include statements.                         */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/**  .........................  **/
/**  MessageQ include files  **/
/**  .........................  **/
#include "p_entry.h"
#include "p_return.h"
#include "p_symbol.h"
#include "p_typecl.h"
#include "p_proces.h"
#include "p_msg.h"

/* End of standard includes.                                                  */
/******************************************************************************/

#define PSEL_ALL_RULES 1
#define PSEL_ANY_RULE  2
#define PSEL_OPER_AND  7

#define MSG_TYPE_SBS_REGISTER_REQ -1196 /* Long form registration           */
#define MSG_TYPE_SBS_REGISTER_RESP -1197 /* Response to REGISTER_REQ        */
#define MSG_TYPE_SBS_DEREGISTER_REQ -1170 /* Deregister from broadcast      */
#define MSG_TYPE_SBS_DEREGISTER_RESP -1172 /* Response to DEREGISTER_REQ    */

/******************************************************************************/
/* The following are include statements for local files.                      */


/* End of include statements for local files.                                 */
/******************************************************************************/

/*****VERSION*********************************************************/

static char talkDMQ2cid[] = "$Id: talkDMQ2.c Ver 01.0 Rel 01.0 $";

/*********************************************************************/

/******************************************************************************/

int32
TLK_DMQ_PutSBSReg ( q_addr, mot_q_addr )
   q_address   q_addr, mot_q_addr;

{
   char        priority;
   char        delivery = PDEL_MODE_NN_MEM; 
   char        uma  = PDEL_UMA_DISC;
   short       class;
   short       type;
   short       size;
   int32       dmq_status;
   int32       put_timeout;
   struct PSB  put_psb;
   SBS_REGISTER_REQ  SBS_reg;
   q_address   target_q;
   int32       resp_q = 0;

    int32  severity;               /*  severity code           */
    char   msg_text[512];          /*  pams_status_text output */
    int32  msg_text_len = 512;     /*  length of buffer        */
    int32  ret_len;                /* returned string length   */

/*
   char        dmq_bus_group[10];
   char        dmq_group_id[5];
   short       dmq_group_def;

   if (dmq_bus_group = getenv("DMQ$BUS_GROUP") != NULL)
		printf("DMQ$BUS_GROUP = %s\n", dmq_bud_group);
   dmq_group_id[0] = &dmq_bus_group[5];
   dmq_group_id[5] = '\0';
   dmq_group_def = atoi(dmq_group_id);
*/

   /* ....................................................... */
   /*  Put the sbs reg message to the sbs server queue        */
   /* ....................................................... */
   printf( "\n" );

   priority    = '\0';           /* Regular priority; use 0, NOT '0'     */

   put_timeout     = 100;            /* Wait 10 seconds before giving up     */

   type = MSG_TYPE_SBS_REGISTER_REQ;
   priority = 0;
   target_q.au.group = mot_q_addr.au.group;
   target_q.au.queue = PAMS_SBS_SERVER;
   class = MSG_CLAS_PAMS;                
   size = sizeof (SBS_reg);
   
   SBS_reg.head.version = 40;
   SBS_reg.head.user_tag = 5155;
   SBS_reg.head.mot = mot_q_addr.au.queue;
   SBS_reg.head.distribution_q.all = q_addr.all;
   SBS_reg.head.req_ack = 1;           /* Ack requested */
   SBS_reg.head.seq_gap_notify = 1;        /* SEQGAP requested */
   SBS_reg.head.auto_dereg = 1;     /* Auto dereg*/

   SBS_reg.head.rule_count = 0;
   SBS_reg.head.rule_conjunct = PSEL_ANY_RULE;
/*
   SBS_reg.head.rule_count = 2;
   SBS_reg.head.rule_conjunct = PSEL_ALL_RULES;

   SBS_reg.rule[0].offset = PSEL_TYPE;
   SBS_reg.rule[0].data_operator = PSEL_OPER_EQ;
   SBS_reg.rule[0].length = 4;
   SBS_reg.rule[0].operand =  25;

   SBS_reg.rule[1].offset = PSEL_CLASS;
   SBS_reg.rule[1].data_operator = PSEL_OPER_EQ;
   SBS_reg.rule[1].length = 4;
   SBS_reg.rule[1].operand =  50;
*/
   dmq_status = pams_put_msg(
                     (char *) &SBS_reg,
                     &priority,
                     &target_q,       
                     &class,
                     &type,
                     &delivery,
                     &size,
                     &put_timeout,
                     &put_psb,
                     &uma,
                     (q_address *) &resp_q,
                     (int32 *) 0,
                     (char *) 0,
                     (char *) 0 );
      
    if ( dmq_status == PAMS__SUCCESS )
         printf( "\tSBS register: succeeded \n" );
    else
       {
         pams_status_text(&dmq_status,&severity, msg_text,&msg_text_len,
                                &ret_len);

         printf( "Error registering...status returned is: %s (%ld)\n\n",
                  msg_text,
                  dmq_status );
        }

   return ( dmq_status );

} /*  end of ExamplePutReg  */


/******************************************************************************/

int32
TLK_DMQ_GetSBSReply( )
{
   SBS_REGISTER_RESP reg_reply;
   char        priority;
   short       msg_class;
   short       msg_type;
   short       msg_data_len;
   short       msg_area_len;
   int32       dmq_status;
   int32       sel_filter;
   int32       timeout;
   q_address   msg_source;
   struct PSB  get_psb;

    int32  severity;               /*  severity code           */
    char   msg_text[512];          /*  pams_status_text output */
    int32  msg_text_len = 512;     /*  length of buffer        */
    int32  ret_len;                /* returned string length   */

   /* ............................................................... */
   /*  Get the reply to the sbs_registration message                  */ 
   /*................................................................ */
   priority       = 0;

   /* ................................................ */
   /*  select all messages, i.e. don't filter any out  */
   /* ................................................ */
   sel_filter     = 0;
   timeout        = 100;

   msg_area_len   = sizeof (struct _SBS_REGISTER_RESP);
   dmq_status     = PAMS__SUCCESS;

  /* ................................................. */
  /*  Initialize the message area to hold the message  */
  /* ................................................. */
  memset( &reg_reply, '\0', sizeof(struct _SBS_REGISTER_RESP) );

  /* ............... */
  /*  Get the reply  */
  /* ............... */
  dmq_status = pams_get_msgw(
		    (char *) &reg_reply,
		    &priority,
		    &msg_source,
		    &msg_class,
		    &msg_type,
		    &msg_area_len,
		    &msg_data_len,
		    &timeout,
		    &sel_filter,
		    &get_psb,
		    (struct show_buffer *) 0,
		    (int32 *) 0,
		    (int32 *) 0,
		    (int32 *) 0,
		    (char *) 0 );

  switch ( dmq_status )
  {
     case PAMS__SUCCESS :
	printf("\tGot Sbs Reply:\n");
	printf ("\t\tstatus = %d\n\t\t registration_id = %d\n\t\t registration_number = %d\n\t\tuser_tag = %d\n", 
		    reg_reply.status, reg_reply.reg_id,
		    reg_reply.number_reg,
                    reg_reply.user_tag);
     break;

     case PAMS__NOMOREMSG :
	/*  Status returned when queue is empty  */
     break;

     case PAMS__TIMEOUT :
	/*  Status returned when queue is empty  */
     break;

     default :
        pams_status_text(&dmq_status,&severity, msg_text,&msg_text_len,
                                &ret_len);

	printf( "\n\tError getting message:...status returned is: %s (%ld)\n\n",
	      msg_text,
	      dmq_status );
     break;
  }


  return ( dmq_status );

} /* end of ExampleGet  */

/******************************************************************************/
/*************************** End of file talk2DMQ.c ***************************/
/******************************************************************************/


