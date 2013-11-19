/* 
 * File:   misc.h
 * Author: cheng
 *
 * Created on December 27, 2012, 5:09 PM
 */

#ifndef HSMS_H
#define HSMS_H

#include "CCriticalSection.h"
#include "ObjectSECS.h"
#include "CMessagePump.h"
#include "CThreadPool.h"

class CHSMS
{
    int StartFlag;
    unsigned int T3, T4, T5, T6, T7, T8;
    
    SOCKET sd2;
 
    CCriticalSection cs;
    
    CThreadUnit * DriverThread;
    
    CSTPMessagePump <int> EndPump;
    

    unsigned int DeviceID;
    unsigned short int TCPPort;
    
    
public:
    CSTPMessagePump <SECSOut> Output;
    CSTPMessagePump <CSecsTag2> UserQueue;
    unsigned int PortState;
    
    CHSMS() : T3(45), T4(45), T5(10), T6(5), T7(10), T8(5), StartFlag(0), sd2(0), PortState(0), DeviceID(0), TCPPort(6000)
    {}

  void SetTCPPort(const unsigned short int port)
  {
    TCPPort = port;
  }
    
    int Start();
    
    int Stop();
    
    int SendControlMessage(const unsigned int type, const unsigned int sb, const unsigned int u1, const unsigned int u2);
    int SendSecsMessage(CSecsMessage & msg, const unsigned int sb, unsigned int option, const __uint64 userdata, const __uint64 userdata2, const unsigned int t3v, const __uint64 channel);

    void ParseHSMS(const char * pBaseBuf, const unsigned int length);
    
    int HSMSHandler2();
    
};

#endif  /* MISC_H */

