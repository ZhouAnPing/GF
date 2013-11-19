/* Include file name: PAMS_C_PROCESS          */
/* File name: P_PROCES.H                    */
/* Usage: #include pams_c_process             */
/*                                            */
/**/
/*                         ---Pool Quota---  UCB   Q    Q  Confrm Perm Name  Chk*/
/*    Queue Name       Num  Bytes  Msgs Ctrl Send Type Own  Style Act  Scope ACL*/
/*------------------- ---- ------- ---- ---- ---- ---- ---- ----- ---- ----- ---*/
/*   Sample Queues*/
#define PAMS_QUEUE_1                          1
#define PAMS_QUEUE_2                          2
#define PAMS_QUEUE_3                          3
#define PAMS_QUEUE_4                          4
#define PAMS_QUEUE_5                          5
#define PAMS_QUEUE_6                          6
#define PAMS_QUEUE_7                          7
#define PAMS_QUEUE_8                          8
#define PAMS_QUEUE_9                          9
#define PAMS_QUEUE_10                         10
/**/
/*  SBS Server uses the following UCB numbers for Optimized Delivery*/
/**/
#define PAMS_SBS_ETH_CONTROL                     74
#define PAMS_SBS_ETH_CHAN1                       75
#define PAMS_SBS_ETH_CHAN2                       76
/**/
/*  Queues 90-100 & 150-199 are reserved for MessageQ utilities*/
#define PAMS_TEMPORARY_Q                          0
#define PAMS_SCREEN_PROCESS                       0
#define PAMS_SPARE1                         90
#define PAMS_ALL_UCBS                         91
#define PAMS_TIMER_QUEUE                         92
#define PAMS_NULL                         93
#define PAMS_NA_SERVER                         94
#define PAMS_QTRANSFER_SERVER                    95
#define PAMS_DEAD_LETTER_QUEUE                   96
#define PAMS_MRS_SERVER                         98
#define PAMS_SBS_SERVER                         99
#define PAMS_COM_SERVER                        100
#define PAMS_DCL_BY_Q_NAME                      151
#define PAMS_TCPIP_LD                        152
#define PAMS_DECNET_LD                        153
#define PAMS_RESERVED_LD                        154
#define PAMS_EVENT_LOGGER                       155
#define PAMS_JRN_SERVER                        156
#define PAMS_DMQ_FULLTEST_PQ                    191
#define PAMS_DMQ_FULLTEST_SQ                    192
#define PAMS_EXAMPLE_Q_1                        193
#define PAMS_EXAMPLE_Q_2                        194
#define PAMS_IVP_UNOWNED_SQ                     195
/**/
/**/
/*        Queue Name                   Group.Queue     Scope*/
/*------------------------------       ----------     -----*/
/*GLOBAL_QUEUE1                             1.234       G*/
#define PAMS_LCL_QUEUE1                        134
#define PAMS_LCL_QUEUE2                        135
#define PAMS_LCL_QUEUE3                        136
#define PAMS_IVP_PRIVATE_MOT1                  4999
#define PAMS_IVP_UNIVERSAL_MOT1                5001
/**/
/* alias names for reserved queues*/
#define PAMS_AVAIL_SERVER                        99
#define PAMS_DECLARE_SERVER                     100
#define PAMS_CONNECT_SERVER                     100
#define PAMS_QUEUE_SERVER                       100
#define PAMS_PAMS_TRANSPORT                     100
/**/
/**/
