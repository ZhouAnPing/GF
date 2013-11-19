/*
 * HSMS_EVENT.H
 *
 *  Created on: Jul 18, 2013
 *      Author: sid105131
 */

#ifndef HSMS_EVENT_H_
#define HSMS_EVENT_H_

int HSMS_Start(const unsigned short int port);
int HSMS_Stop();
int HSMS_Send(const char * s);
int HSMS_SendReply(const char * s, const unsigned int sb);
int HSMS_SendAndWait(const char * s, char * rs, int rsbuf);
int GetHSMSState();
void checkHSMSState(int* RetSts, char *ErrMsgC);

int ParseCarrierEntered(char *Carrierid, char *Amhsequipid, char *Category, char *Number);
int ParseCarrierRemoved(char *Carrierid, char *Amhsequipid);

#endif /* HSMS_EVENT_H_ */
