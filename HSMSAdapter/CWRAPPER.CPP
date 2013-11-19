/* 
 * File:   main.cpp
 * Author: cheng
 *
 * Created on December 26, 2012, 5:59 AM
 */

/*
 * 
 */
#include <iostream>
#include <errno.h>
#include <map>
#include <string>
#include <deque>
#include <set>

using namespace std;
#include "CCondVar.h"
#include "CCriticalSection.h"
#include "misc.h"
#include "STP_Basic.h"
#include "STP_Integer.h"
#include "STP_Timer.h"
#include "STLHelper.h"

#include "CMessagePump.h"
#include "CThreadPool.h"
#include "HSMS.h"

#ifndef _STP_NOT_WINDOWS
#define sleep(x) Sleep((x) * 1000)
#endif

CHSMS hsms;
CEventPool EventPool;
CThreadUnit * pThread1;

CCondVar E;
CSecsMessage Reply(99, 0, 0, 0);

extern "C" void HSMS_Event(const char * s, unsigned int waitflag, unsigned int sb);

STP_USER_THREADPROC(Thread1)
{
  CSecsTag2 * tag;
  CSecsMessage ReplyMsg;
  string kk;

  while (hsms.UserQueue.GetMessage(tag) != CMESSAGEPUMP_USERQUIT) {
    cout << "processing msg with sb " << tag->SystemByte << endl << tag << endl;
 
    if (tag->Msg->GetStream() == 99
        && tag->Msg->GetFunction() == 101
        && tag->Msg->GetNumberOfItem()
        && tag->Msg->Item->GetType() == iA)
      {
        kk.clear();
        (*tag->Msg->Item).GetBinaryData(kk);
        HSMS_Event(kk.c_str(), tag->Msg->GetWaitBit(), tag->SystemByte);
      }

    delete tag;
  }

  return 0;
}

extern "C" int HSMS_Start(const unsigned short int port)
{
#ifndef _STP_NOT_WINDOWS
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  hsms.SetTCPPort(port);
  hsms.Start();
  ThreadPool.RunFunction(Thread1, (void *) 0, pThread1, 1);

  return 0;
}

extern "C" int HSMS_Stop()
{
  hsms.Stop();
  ThreadPool.EndAll();
  return 0;
}

extern "C" int HSMS_Send(const char *s)
{
  CSecsMessage msg;
  msg.SetSFW(99, 99, 1);
  (msg(iA, strlen(s)).Item)->SetData_A(s, strlen(s));
  hsms.SendSecsMessage(msg, 0, 0, 0, 0, 0, 0);

  return 0;
}

extern "C" int HSMS_SendReply(const char * s, const unsigned int sb)
{
  CSecsMessage msg;
  msg.SetSFW(99, 100, 0);
  (msg(iA, strlen(s)).Item)->SetData_A(s, strlen(s));
  hsms.SendSecsMessage(msg, sb, 0, 0, 0, 0, 0);

  return 0;
}

extern "C" int HSMS_SendAndWait(const char * s, char * rs, int rs_size)
{
  CSecsMessage msg, in;
  CEventUnit * pEventUnit;
  int n;

  msg.SetSFW(99, 99, 1);
  (msg(iA, strlen(s)).Item)->SetData_A(s, strlen(s));
  pEventUnit = EventPool.RequestNewEvent(2);
  if (hsms.SendSecsMessage(msg, 0, SID_SENDANDWAIT, (__uint64) pEventUnit, (__uint64) &in , 0, 0))
  {
    EventPool.DropEvent(pEventUnit);
    return 0;
  }

  pEventUnit->hEvent.Wait();
  if (EventPool.Release(pEventUnit, 1) == 1 && in.GetNumberOfItem() && in.Item->GetType() == iA)
  {
    string kk;
    in.Item->GetBinaryData(kk);
    n = kk.size() < rs_size ? kk.size() : rs_size;
    memcpy(rs, kk.c_str(), rs_size);
    rs[n] = 0;
    return n;
  }
  return 0;
}

extern "C" int GetHSMSState()           //return 3 if connected
{
  return hsms.PortState;
}

