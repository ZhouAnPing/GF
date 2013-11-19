#include <iostream>
#include <string>

#ifdef _STP_NOT_WINDOWS

#ifdef _STP_VMS_ALPHA
#include <in.h>                     /* define internet related constants, functions, and structures   */
#include <inet.h>                   /* define network address info          */
#include <netdb.h>                  /* define network database library info */
#include <socket.h>                 /* define BSD 4.x socket api            */
#include <unixio.h>                 /* define unix i/o                      */
#endif

#ifdef _STP_VMS_ITANIUM
#include <in.h>                     /* define internet related constants, functions, and structures   */
#include <inet.h>                   /* define network address info          */
#include <netdb.h>                  /* define network database library info */
#include <socket.h>                 /* define BSD 4.x socket api            */
#include <unixio.h>                 /* define unix i/o                      */
#endif



#ifdef _STP_LINUX
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#endif

#include <unistd.h>     /* for close() */
#include <netinet/in.h>
#include <sys/poll.h>
#include <errno.h>
#include <string.h>

#else
#include <Winsock2.h>
#define close closesocket
#endif

using namespace std;

#include "STP_Basic.h"
#include "misc.h"
#include "STLHelper.h"
#include "STP_Timer.h"

#include "ObjectSECS.h"
#include "CCriticalSection.h"
#include "HSMS.h"
#define SERVPORT 8002
#define READSIZE 65507

#define STATE_READ_LENGTH       0
#define STATE_READ_MESSAGE      1

//the type of HSMS control messages
#define SELECT_REQUEST          0x01000000
#define SELECT_RESPONSE0        0x02000000
#define SELECT_RESPONSE1        0x02000100
#define SELECT_RESPONSE2        0x02000200
#define SELECT_RESPONSE3        0x02000300
#define LINKTEST_REQUEST        0x05000000
#define LINKTEST_RESPONSE       0x06000000
#define SEPERATE_REQUEST        0x09000000

extern CEventPool EventPool;

int CHSMS::HSMSHandler2()
{
    SOCKET sd = 0;
    unsigned int tmpType;

    unsigned int outsb = 8639;

#ifdef _STP_NOT_WINDOWS
    int optval = 1;
#else
        BOOL optval = 1;
#endif
    
    STP_LARGE_INTEGER tmpChannel;
    
    int pollresult;
    int timeout = -1;
    NewTimerStruct <unsigned __int64> LocalTimer;
    __uint64 absolute;
    
    map <unsigned __int64, SerialInputData *> Input;
    map <unsigned __int64, SerialInputData *>::iterator iHSMS;

    unsigned __int64 uid;     //, u1, u2;
    unsigned int sb;
    unsigned short int deviceid;

    int ReadState = STATE_READ_LENGTH;
    
    unsigned int ExpectedLength = 4;
    unsigned int PartialLength = 0;
    unsigned int ParseIndex = 0;
    char * pBaseBuf = NULL;
    char * pExpectedLength = (char *) &ExpectedLength;
    
    socklen_t sin_size = sizeof (struct sockaddr_in);
    
    SECSOut * out;
    
    sockaddr_in serveraddr = {0};
    sockaddr_in their_addr = {0};
    
    char * ReadBuf = new char[READSIZE];

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "socket failed" << endl;
        goto END_HSMS_HANDLER;
    }

#ifdef _STP_NOT_WINDOWS
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
#else
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof(optval)) == SOCKET_ERROR)
#endif
    {

        cout << "Failed to setsockopt" << endl;
        goto END_HSMS_HANDLER;
    }
    
      /* bind to an address */

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVPORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if((bind(sd, (struct sockaddr *) &serveraddr, sizeof(serveraddr))) < 0)
    {
        cout << "bind failed" << endl;
        goto END_HSMS_HANDLER;
    }
    
    if (listen(sd, 10) < 0)
    {
        cout << "Server-listen() error" << endl;
        goto END_HSMS_HANDLER;
    }

#ifdef _STP_NOT_WINDOWS
    
    ssize_t dwRead;
    pollfd fds[3];
    
AcceptLoop:

    fds[0].fd = EndPump.GetEvent();
    fds[0].events = POLLIN | POLLPRI;
    
    fds[1].fd = sd;
    fds[1].events = POLLIN | POLLPRI;
    
    cout << "Waiting for connection at port " << SERVPORT << endl;
    switch (poll(fds, 2, -1))
    {
        case 0:
            cout << "Timeout waiting for connection" << endl;
            goto END_HSMS_HANDLER;
            
        case -1:
            cout << "An error" << endl;
            goto END_HSMS_HANDLER;
            
        default:
            if (fds[0].revents)
            {
                cout << "Soft end" << endl;
                goto END_HSMS_HANDLER;
            }
            
            if (fds[1].revents)
            {
                if ((sd2 = accept(sd, (struct sockaddr *) &their_addr, &sin_size)) < 0)
                {
                    cout << "Failure in accept from " << endl;
                    goto END_HSMS_HANDLER;
                }
                
                PortState = HSMS_NOT_SELECTED;
                cout << "socket is accepted" << endl;
            }
            break;
    }
    
    fds[1].fd = sd2;        //object->Output.GetEvent();
    LocalTimer.AddTimer(T7 * 1000, new NewTimerUnit <unsigned __int64> (0, TIMER_T7, 50));
    
    fds[2].fd = Output.GetEvent();
    fds[2].events = POLLIN | POLLPRI;

    
sm1:
    absolute = LocalTimer.GetAbsoluteTimeOut();
    if (absolute)
    {
        timeval tv;
        gettimeofday(&tv, NULL);

        tmpChannel.u = 0;
        tmpChannel.H.l.u = tv.tv_sec;
        tmpChannel.u *= 10000000;
        tmpChannel.u += tv.tv_usec * 10;
        
        if (tmpChannel.u > absolute)       //time out has already occured
            pollresult = 0;
        else
        {
            timeout = (int) ((absolute - tmpChannel.u) / 10000);
            cout << "timeout value is " << timeout << endl;
            pollresult = poll(fds, 3, timeout);
        }
    }
    else
    {
        timeout = -1;
        pollresult = poll(fds, 3, timeout);
    }
    
    switch (pollresult)
    {
        case -1:
            cout << "Poll error" << endl;
            goto END_HSMS_HANDLER;
                    
        case 0:
            while (LocalTimer.GetTimeOutSID(tmpChannel.u, tmpType))
            {
                switch (tmpType)
                {
                        case TIMER_T3:
                                cout << "T3 timeout waiting for data message with system byte " << tmpChannel.H.h.u << endl;
                                iHSMS = Input.find(tmpChannel.u);
                                if (iHSMS != Input.end())
                                {
                                  if (iHSMS->second->SyncID & SID_SENDANDWAIT)
                                  {
                                    EventPool.ReleaseSet((CEventUnit *) iHSMS->second->U1, 0);
                                  }
                                  delete iHSMS->second;
                                  Input.erase(iHSMS);
                                }
                                cout << "Input size is " << Input.size() << endl;
                                break;

                        case TIMER_T6:
                                cout << "T6 timeout waiting for control message with system byte = " << tmpChannel.H.h.u << endl;
                                //no need to clean up input, since it will be clean up at the END_HSMS_HANDLER
                                goto END_HSMS_HANDLER;

                        case TIMER_T7:
                                if (PortState != HSMS_SELECTED)
                                {
                                        cout << "T7 timeout waiting for HSMS selected state" << endl;
                                        goto END_HSMS_HANDLER;
                                }
                                break;

                        case TIMER_T8:
                                cout << "T8 timeout waiting for character" << endl;
                                goto END_HSMS_HANDLER;
                }
            }
            goto sm1;
            
        default:
            if (fds[0].revents)
            {
                cout << "Soft end" << endl;
                goto END_HSMS_HANDLER;
            }
            
            if (fds[2].revents)
            {
                Output.GetMessage(out);

                if (out->S[7] % 2)           //if this is a primary message
                {
                  ++outsb;
                  out->S[10] = outsb >> 24;
                  out->S[11] = outsb >> 16;
                  out->S[12] = outsb >> 8;
                  out->S[13] = outsb;
                }

                dwRead = write(sd2, out->S.c_str(), out->S.size());
                if (dwRead)
                {
                  sb = Swap32(* (unsigned int *) (out->S.c_str() + 10));
                  uid = (__uint64) sb << 32;
                  deviceid = ((out->S[4] & 0x7F) << 8) | out->S[5];

                  if (out->S[6] & 0x80)  //if waitbit is on
                  {
                       if (out->SyncID & SID_SENDANDWAIT)
                       {
                            if (EventPool.SpecialEnterCS((CEventUnit *) out->U1, 2))
                            {
                                 ((CSecsMessage *) out->U2)->SetSFW(9, 253, 0);
                                 EventPool.LeaveCS();
                                 Input[uid] = new SerialInputData(LocalTimer.AddTimer(out->T3v ? out->T3v : T3 * 1000, new NewTimerUnit <unsigned __int64> (uid, TIMER_T3, 50)), out->SyncID, out->U1, out->U2);
                            }
                            else
                            {
                                 Input[uid] = new SerialInputData(LocalTimer.AddTimer(out->T3v ? out->T3v : T3 * 1000, new NewTimerUnit <unsigned __int64> (uid, TIMER_T3, 50)), out->SyncID & 0xFFFFFFFE, 0, 0);
                            }
                       }
                       else
                       {
                            Input[uid] = new SerialInputData(LocalTimer.AddTimer(out->T3v ? out->T3v : T3 * 1000, new NewTimerUnit <unsigned __int64> (uid, TIMER_T3, 50)), out->SyncID, out->U1, out->U2);
                       }
                  }
                  else
                  {
                       //SetLastTransactedSB(sb);
                       if (out->SyncID & SID_SENDANDWAIT)
                       {
                            EventPool.ReleaseSet((CEventUnit *) out->U1, 1);
                       }
                  }
                }
                delete out;
            }
            else
            {
                cout << "No event for fds[2]" << endl;
            }
            
            if (fds[1].revents)
            {
                dwRead = read(sd2, ReadBuf, READSIZE);
                switch (dwRead)
                {
                    case -1:
                        cout << "socket read error" << endl;
                        goto END_HSMS_HANDLER;
                        
                    case 0:
                        cout << "socket is closed" << endl;
                        PortState = HSMS_TCP_NOT_CONNECTED;
                        close(sd2);
                        goto AcceptLoop;
                            
                    default:
                        ParseIndex = 0;
                        ParseAgain:
                        if (ReadState == STATE_READ_LENGTH)
                        {
                            if (dwRead - ParseIndex + PartialLength >= 4)
                            {
                                memcpy(pExpectedLength + PartialLength, ReadBuf + ParseIndex, 4 - PartialLength);
                                ParseIndex += (4 - PartialLength);
                                PartialLength = 0;
                                ReadState = STATE_READ_MESSAGE;
                                SelfSwap32(ExpectedLength);
                                cout << "Got length " << ExpectedLength << endl;
                                if (ExpectedLength < 10 || ExpectedLength > 65535)
                                {
                                     cout << "Bad HSMS Length "<< ExpectedLength << endl;
                                     goto END_HSMS_HANDLER;
                                }
                                pBaseBuf = new char[ExpectedLength];
                            }
                            else
                            {
                                memcpy(pExpectedLength + PartialLength, ReadBuf + ParseIndex, dwRead - ParseIndex);
                                PartialLength += (dwRead - ParseIndex);
                                ParseIndex = dwRead;
                            }
                        }
                        else
                        {
                            if (dwRead - ParseIndex + PartialLength >= ExpectedLength)
                            {
                                memcpy(pBaseBuf + PartialLength, ReadBuf + ParseIndex, ExpectedLength - PartialLength);
                                ParseIndex += (ExpectedLength - PartialLength);
                                PartialLength = 0;
                                ReadState = STATE_READ_LENGTH;

                                deviceid = Swap16(* (unsigned short int *) pBaseBuf);
                                sb = Swap32(* (unsigned int *) (pBaseBuf + 6));


                                    switch (pBaseBuf[5])
                                    {
                                        case 1:         //Select.Req
                                            cout << "Received select request, system byte = " << sb << endl;
                                            PortState = HSMS_SELECTED;
                                            SendControlMessage(SELECT_RESPONSE0, sb, 0, 0);
                                            break;

                                     case 5:        //LinkTest.Req
                                          SendControlMessage(LINKTEST_RESPONSE, sb, 0, 0);
                                          break;

                                        case 0:
                                        {
                                            unsigned int r;

                                            cout << "Received data message with sb = " << sb << endl;

                                            if (pBaseBuf[3] % 2) //if this is a primary message
                                            {
                                                CSecsMessage * msg = new CSecsMessage;
                                                r = msg->Assign(pBaseBuf, ExpectedLength);
                                                if (r)
                                                {
                                                     //message decode error
                                                     delete msg;
                                                }
                                                else
                                                {
                                                     UserQueue.PostUserMessage(new CSecsTag2(msg, sb, 0, 0, 0));
                                                }
                                           }
                                           else                     //this is a secondary message
                                           {
                                                iHSMS = Input.find((unsigned __int64) sb << 32);
                                                if (iHSMS != Input.end())               //this is an expected message
                                                {
                                                     LocalTimer.RemoveTimer(iHSMS->second->TimerID);

                                                     if ((iHSMS->second->SyncID & SID_SENDANDWAIT) && EventPool.SpecialEnterCS((CEventUnit *) iHSMS->second->U1, 2))         //this is a SendAndWait message
                                                     {
                                                         cout << "Fantastic!!!!" << endl;
                                                          r = ((CSecsMessage *) iHSMS->second->U2)->Assign(pBaseBuf, ExpectedLength);
                                                          if (r)
                                                          {
                                                              EventPool.LeaveCSReleaseSet((CEventUnit *) iHSMS->second->U1, 0);
                                                          }
                                                          else
                                                          {
                                                              EventPool.LeaveCSReleaseSet((CEventUnit *) iHSMS->second->U1, 1);
                                                          }
                                                     }
                                                     else                     //not a SendAndWait message
                                                     {
                                                          CSecsMessage * msg = new CSecsMessage;
                                                          r = msg->Assign(pBaseBuf, ExpectedLength);
                                                          if (r)
                                                          {
                                                            delete msg;
                                                          }
                                                          else
                                                          {
                                                            UserQueue.PostUserMessage(new CSecsTag2(msg, sb, 0, 0, 0));
                                                          }
                                                     }
                                                     delete iHSMS->second;
                                                     Input.erase(iHSMS);
                                                }
                                                else                //this is not an expected secondary message
                                                {
                                                     CSecsMessage * msg = new CSecsMessage;
                                                     r = msg->Assign(pBaseBuf, ExpectedLength);
                                                     if (r)
                                                     {
                                                       cout << "Failed to decode unexpected message" << endl;
                                                     }
                                                     else
                                                     {
                                                       cout << "Received unexpected message" << endl;
                                                     }
                                                     delete msg;
                                                }
                                           }
                                           break;
                                        }
                                    }
                                ExpectedLength = 4;
                                delete [] pBaseBuf;
                                pBaseBuf = NULL;
                            }
                            else
                            {
                                memcpy(pBaseBuf + PartialLength, ReadBuf + ParseIndex, dwRead - ParseIndex);
                                PartialLength += (dwRead - ParseIndex);
                                ParseIndex = dwRead;
                            }
                        }
                        
                        if (ParseIndex < dwRead)
                       {
                            cout << "Going to ParseAgain" << endl;
                            goto ParseAgain;
                       }
                        break;
                    
                }
            }
            
            goto sm1;
    }
    
#else

#endif
    
    
    
    
END_HSMS_HANDLER:
    if (sd > 0)
    {
        close(sd);
        sd = 0;
    }

    if (sd2 > 0)
    {
        close(sd2);
        sd2 = 0;
    }
    
    delete [] ReadBuf;
    cout << "HSMSHandler ended" << endl;
    UserQueue.PostUserQuitMessage();
    return 0;
}

//void * HSMSHandler(void * arg)
STP_USER_THREADPROC(HSMSHandler)
{
    CHSMS * object = (CHSMS *) arg;
    return (ThreadReturnType) object->HSMSHandler2();
}


int CHSMS::SendControlMessage(const unsigned int type, const unsigned int sb, const unsigned int u1, const unsigned int u2)
{
        char tmp[14] = {'\x00', '\x00', '\x00', '\x0A', '\xFF', '\xFF'};

        string text;
        * (unsigned int *) (tmp + 6) = type;
        
        switch (type)
        {
                case SELECT_REQUEST:
                        text.assign("Select request", 14);                              
                        break;

                case SELECT_RESPONSE0:
                        text.assign("Select response 0 - select was successful", 40);
                        * (unsigned int *) (tmp + 10) = MiscFunction::_byteswap_ulong(sb);
                        break;

                case SELECT_RESPONSE1:
                        text.assign("Select response 1 - communication already active", 48);
                        * (unsigned int *) (tmp + 10) = MiscFunction::_byteswap_ulong(sb);
                        break;

                case SELECT_RESPONSE2:
                        text.assign("Select response 2 - connection not ready", 40);
                        * (unsigned int *) (tmp + 10) = MiscFunction::_byteswap_ulong(sb);
                        break;

                case SELECT_RESPONSE3:
                        text.assign("Select response 3 - connect exhaust", 35);
                        * (unsigned int *) (tmp + 10) = MiscFunction::_byteswap_ulong(sb);
                        break;

                case LINKTEST_REQUEST:
                        text.assign("Linktest request", 16);
                        break;

                case LINKTEST_RESPONSE:
                        text.assign("Linktest response", 17);
                        * (unsigned int *) (tmp + 10) = MiscFunction::_byteswap_ulong(sb);
                        break;

                case SEPERATE_REQUEST:
                        text.assign("Seperate request", 16);
                        break;

                default:
                        return 0;
        }
                
#ifdef _STP_NOT_WINDOWS
            ssize_t x = write(sd2, tmp, 14);
#else
                DWORD dwWrite;
                WriteFile((HANDLE) sd2, tmp, 14, &dwWrite, NULL);
#endif
        return 0;
}

int CHSMS::Start()
{
    ThreadPool.RunFunction(HSMSHandler, this, DriverThread, 1);
    return 0;
}

int CHSMS::Stop()
{
    EndPump.PostUserQuitMessage();
    return 0;
}

int CSecsMessage::ConvertBtoD()
{
  unsigned int r;
  unsigned int offset = 0;

  if (BinaryStream.size())
  {
    if (Item == NULL)
      Item = new CSecsDataItem;

    r = Item->Secs2Parse(BinaryStream.data(), offset, (unsigned int) BinaryStream.size());

    if (r)
      return r;

    if (offset == BinaryStream.size())
      return 0;
    return (offset & 0x0FFFFFFF) | 0x90000000;
  }
  return 0;
}

unsigned int CSecsDataItem::Secs2Parse(const char *secs2, unsigned int &offset, const unsigned int maxlength)
{
        int formatbyte;
        int lengthbyte;
        int nlengthbyte;
        int loop;
        unsigned int r;

        if (offset >= maxlength)
                return (offset & 0x0FFFFFFF) | 0x10000000;

        lengthbyte = 0;
        formatbyte = secs2[offset] & 0xfc;
        nlengthbyte = secs2[offset] & 0x03;

        if (nlengthbyte == 0)
                return (offset & 0x0FFFFFFF) | 0x20000000;

        if (++offset + nlengthbyte > maxlength)
                return (offset & 0x0FFFFFFF) | 0x30000000;

        memcpy(((char *) &lengthbyte) + 4 - nlengthbyte, secs2 + offset, nlengthbyte);

        SelfSwap32(lengthbyte);
    offset += nlengthbyte;

        if (formatbyte == iL)
        {
                SetHeader(iL, lengthbyte, 0);
                for (loop = 0; loop < lengthbyte; loop++)
                {
                        if (r = (* (vector <CSecsDataItem> *)pItem)[loop].Secs2Parse(secs2, offset, maxlength))
                                return r;
                }
        }
        else
        {
                if (offset + lengthbyte > maxlength)
                        return (offset & 0x0FFFFFFF) | 0x40000000;

                switch (formatbyte)
                {
                        case iI1:
                        case iU1:
                        case iB:
                        case iBOO:
                        case iA:
                        case iJ:
                                SetHeader(formatbyte, lengthbyte, 0);
                                if (lengthbyte)
                                        memcpy(pItem, secs2 + offset, lengthbyte);
                                break;

                        case iU2:
                        case iA2:
                        case iI2:
                                if (lengthbyte % 2)
                                        return (offset - nlengthbyte & 0x0FFFFFFF) | 0x50000000;

                                SetHeader(formatbyte, lengthbyte/2, 0);
                                if (lengthbyte)
                                        swab((char *) (secs2 + offset), pItem, lengthbyte);
                                break;

                        case iF4:
                        case iU4:
                        case iI4:
                                if (lengthbyte % 4)
                                        return (offset - nlengthbyte & 0x0FFFFFFF) | 0x60000000;
                        
                                SetHeader(formatbyte, lengthbyte/4, 0);
                                if (lengthbyte)
                                        MiscFunction::xhilo4b((unsigned int *) pItem, (unsigned int *) (secs2 + offset), lengthbyte / 4);
                                break;

                        case iF8:
                        case iI8:
                        case iU8:
                                if (lengthbyte % 8)
                                        return (offset - nlengthbyte & 0x0FFFFFFF) | 0x70000000;
                                SetHeader(formatbyte, lengthbyte / 8, 0);
                                if (lengthbyte)
                                        MiscFunction::xhilo8b((unsigned __int64 *) pItem, (unsigned __int64 *) (secs2 + offset), lengthbyte / 8);
                                break;

                        default:
                                return (offset - nlengthbyte - 1 & 0x0FFFFFFF) | 0x80000000;
                }
                offset += lengthbyte;
        }
        return 0;
}

int CSecsMessage::ConvertDtoT(const unsigned int option)
{
  Description.clear();
  Description << "S" << Stream << "F" << Function;
    
  if (WaitBit)
    Description.append(" W", 2);

  if (Item)
  {
    Description.append("\r\n", 2);
    Item->FillText(Description, 0, option | FULL_STOP);
  }
  else
  {
    Description.append(".\r\n", 3);
  }
  return 0;
}

int CSecsDataItem::FillText(string &s, unsigned int depth, unsigned int option)
{
        size_t leadingspace;    //leading space before each data value
        unsigned int loop;
        char tmp[32];

        switch (Type)
        {
                case iL:
                        s.append(depth, '\t');
                        if (option & LIST_COUNT)
            {
                                s << "<L[" << NumberOfItem << "]";
                        }
                        else
            {
                                s << "<L";
            }

                        if (NumberOfItem)
                        {
                                s.append("\r\n", 2);
                                for (loop = 0; loop < NumberOfItem; loop++)
                                        (* (vector <CSecsDataItem> *)pItem)[loop].FillText(s, depth + 1, option & ~FULL_STOP);

                                s.append(depth, '\t');
                        }
                        break;

                case iI1:
                        s.append(depth, '\t');

                        leadingspace = s.size();

                        if (option & OTHER_COUNT)
                        {
                                s << "<I1[" << NumberOfItem << "]";
                        }
                        else
                                s << "<I1";

                        leadingspace = s.size() - leadingspace;

                        if (NumberOfItem == 1)
                        {
                                s << " " << pItem[0];
                        }
                        else
                        {
                                for (loop = 0; loop < NumberOfItem; loop++)
                                {
                                        if (loop)
                                                s.append(leadingspace, ' ');
                                        s << " " << pItem[loop] << "\r\n";
                    s.append(depth, '\t');
                                }
                        }
                        break;

                case iI2:
                        s.append(depth, '\t');

                        leadingspace = s.size();

                        if (option & OTHER_COUNT)
                        {
                                s << "<I2[" << NumberOfItem << "]";
                        }
                        else
                                s.append("<I2", 3);

                        leadingspace = s.size() - leadingspace;

                        if (NumberOfItem == 1)
                        {
                                s.append(1, ' ') << ((short *)pItem)[0];
                        }
                        else
                        {
                                for (loop = 0; loop < NumberOfItem; loop++)
                                {
                                        if (loop)
                                                s.append(leadingspace, ' ');
                                        s.append(1, ' ') << ((short *)pItem)[loop] << "\r\n";
                    s.append(depth, '\t');
                                }
                        }
                        break;

                case iI4:
                        s.append(depth, '\t');

                        leadingspace = s.size();

                        if (option & OTHER_COUNT)
                        {
                                s.append("<I4[", 4) << NumberOfItem << "]";
                        }
                        else
                                s.append("<I4", 3);

                        leadingspace = s.size() - leadingspace;

                        if (NumberOfItem == 1)
                        {
                                s.append(1, ' ') << ((int *)pItem)[0];
                        }
                        else
                        {
                                for (loop = 0; loop < NumberOfItem; loop++)
                                {
                                        if (loop)
                                                s.append(leadingspace, ' ');
                                        s.append(1, ' ') << ((int *)pItem)[loop] << "\r\n";
                    s.append(depth, '\t');
                                }
                        }
                        break;
                        
                case iU1:
                        s.append(depth, '\t');

                        leadingspace = s.size();

                        if (option & OTHER_COUNT)
                        {
                                s.append("<U1[", 4) << NumberOfItem << "]";
                        }
                        else
            {
                                s.append("<U1", 3);
            }
            
                        leadingspace = s.size() - leadingspace;

                        if (NumberOfItem == 1)
                        {
                                s.append(1, ' ') << ((unsigned char *) pItem)[0] ;
                        }
                        else
                        {
                                for (loop = 0; loop < NumberOfItem; loop++)
                                {
                                        if (loop)
                                                s.append(leadingspace, ' ');
                                        s.append(1, ' ') << ((unsigned char *) pItem)[loop] << "\r\n";
                    s.append(depth, '\t');
                                }
                        }
                        break;

                case iU2:
                        s.append(depth, '\t');

                        leadingspace = s.size();

                        if (option & OTHER_COUNT)
                        {
                                s.append("<U2[", 4) << NumberOfItem << "]";
                        }
                        else
                                s.append("<U2", 3);

                        leadingspace = s.size() - leadingspace;

                        if (NumberOfItem == 1)
                        {
                                s.append(1, ' ') << ((unsigned short *) pItem)[0];
                        }
                        else
                        {
                                for (loop = 0; loop < NumberOfItem; loop++)
                                {
                                        if (loop)
                                                s.append(leadingspace, ' ');
                                        s.append(1, ' ') << ((unsigned short *) pItem)[loop] << "\r\n";
                    s.append(depth, '\t');
                                }
                        }
                        break;

                case iU4:
                        s.append(depth, '\t');

                        leadingspace = s.size();

                        if (option & OTHER_COUNT)
                        {
                                s.append("<U4[", 4) << NumberOfItem << "]";
                        }
                        else
                                s.append("<U4", 3);

                        leadingspace = s.size() - leadingspace;

                        if (NumberOfItem == 1)
                        {
                                s.append(1, ' ') << ((unsigned int *) pItem)[0];
                        }
                        else
                        {
                                for (loop = 0; loop < NumberOfItem; loop++)
                                {
                                        if (loop)
                                                s.append(leadingspace, ' ');
                                        s.append(1, ' ') << ((unsigned int *) pItem)[loop] << "\r\n";
                    s.append(depth, '\t');
                                }
                        }
                        break;

                case iI8:
                        s.append(depth, '\t');

                        leadingspace = s.size();

                        if (option & OTHER_COUNT)
                        {
                                s.append("<I8[", 4) << NumberOfItem << "]";
                        }
                        else
                                s.append("<I8", 3);

                        leadingspace = s.size() - leadingspace;

                        if (NumberOfItem == 1)
                        {
                                s.append(1, ' ') << ((__int64 *) pItem)[0];
                        }
                        else
                        {
                                for (loop = 0; loop < NumberOfItem; loop++)
                                {
                                        if (loop)
                                                s.append(leadingspace, ' ');
                                        s.append(1, ' ') << ((__int64 *) pItem)[loop] << "\r\n";
                    s.append(depth, '\t');
                                }
                        }
                        break;

                case iU8:
                        s.append(depth, '\t');

                        leadingspace = s.size();

                        if (option & OTHER_COUNT)
                        {
                                s.append("<U8[", 4) << NumberOfItem << "]";
                        }
                        else
                                s.append("<U8", 3);

                        leadingspace = s.size() - leadingspace;

                        if (NumberOfItem == 1)
                        {
                                s.append(1, ' ') << ((unsigned __int64 *) pItem)[0];
                        }
                        else
                        {
                                for (loop = 0; loop < NumberOfItem; loop++)
                                {
                                        if (loop)
                                                s.append(leadingspace, ' ');
                                        s.append(1, ' ') << ((unsigned __int64 *) pItem)[loop] << "\r\n";
                    s.append(depth, '\t');
                                }
                        }
                        break;

                case iF4:
                        s.append(depth, '\t');

                        leadingspace = s.size();

                        if (option & OTHER_COUNT)
                        {
                                s.append("<F4[", 4) << NumberOfItem << "]";
                        }
                        else
                                s.append("<F4", 3);

                        leadingspace = s.size() - leadingspace;

                        if (NumberOfItem == 1)
                        {
                                s.append(1, ' ') << ((float *) pItem)[0];
                                //StringHelper::append_number(s, );
                        }
                        else
                        {
                                for (loop = 0; loop < NumberOfItem; loop++)
                                {
                                        if (loop)
                                                s.append(leadingspace, ' ');
                                        s.append(1, ' ') << ((float *) pItem)[loop];
                                        //StringHelper::append_number(s, ((float *) pItem)[loop]);
                                        s.append("\r\n", 2).append(depth, '\t');
                                }
                        }
                        break;

                case iF8:
                        s.append(depth, '\t');

                        leadingspace = s.size();

                        if (option & OTHER_COUNT)
                        {
                                s.append("<F8[", 4) << NumberOfItem << "]";
                        }
                        else
                                s.append("<F8", 3);

                        leadingspace = s.size() - leadingspace;

                        if (NumberOfItem == 1)
                        {
                                s.append(1, ' ') << ((double *) pItem)[0];
                        }
                        else
                        {
                                for (loop = 0; loop < NumberOfItem; loop++)
                                {
                                        if (loop)
                                                s.append(leadingspace, ' ');
                                        s.append(1, ' ') << ((double *) pItem)[loop];
                                        s.append("\r\n", 2).append(depth, '\t');
                                }
                        }
                        break;

                case iA:
                        s.append(depth, '\t');

                        leadingspace = s.size();

                        if (option & OTHER_COUNT)
                        {
                                s.append("<A[", 3) << NumberOfItem << "]";
                        }
                        else
                                s.append("<A", 2);

                        leadingspace = s.size() - leadingspace;

                        if (NumberOfItem)
                        {
                                unsigned char a;
                                for (loop = 0; loop < NumberOfItem; loop++)
                                {
                                        a = ((unsigned char *)pItem)[loop];
                                        if (loop % 100 == 0)
                                        {
                                                if (loop)
                                                        s.append(leadingspace, ' ');
                                                s.append(" \"", 2);
                                                
                                        }
                                        if (a < 32 || a > 126)
                                        {
                                                //wsprintf(tmp, "\\x%02X", a);
                                                s.append("\\x").append(HEX(a), 2);
                                        }
                                        else
                                        {
                                                switch (a)
                                                {
                                                        case '"':
                                                                //wsprintf(tmp, "\\x%02X", a);
                                                                s.append("\\x").append(HEX(a), 2);
                                                                break;

                                                        case '\\':
                                                                s.append(2, '\\');
                                                                break;

                                                        default:
                                                                s.append(1, a);
                                                }
                                                
                                        }
                                        if (loop % 100 == 99)
                                        {
                                                s.append(1, '\"');
                                                if (NumberOfItem > 100)
                                                        s.append("\r\n", 2).append(depth, '\t');
                                        }
                                }
                                if (loop % 100)
                                {
                                        s.append(1, '\"');
                                        if (NumberOfItem > 100)
                                                s.append("\r\n", 2).append(depth, '\t');
                                }
                        }
                        break;

                case iJ:
                        s.append(depth, '\t');

                        leadingspace = s.size();

                        if (option & OTHER_COUNT)
                        {
                                s.append("<J[", 3) << NumberOfItem << "]";
                        }
                        else
                                s.append("<J", 2);

                        leadingspace = s.size() - leadingspace;

                        if (NumberOfItem)
                        {
                                unsigned char a;
                                for (loop = 0; loop < NumberOfItem; loop++)
                                {
                                        a = ((unsigned char *)pItem)[loop];
                                        if (loop % 100 == 0)
                                        {
                                                if (loop)
                                                        s.append(leadingspace, ' ');
                                                s.append(" \"", 2);
                                                
                                        }
                                        if (a < 32 || a > 126 || a == 34)
                                        {
                                                s.append("\\x").append(HEX(a), 2);
                                        }
                                        else
                                                s.append(1, a);
                                        if (loop % 100 == 99)
                                        {
                                                s.append(1, '\"');
                                                if (NumberOfItem > 100)
                                                        s.append("\r\n", 2).append(depth, '\t');
                                        }
                                }
                                if (loop % 100)
                                {
                                        s.append(1, '\"');
                                        if (NumberOfItem > 100)
                                                s.append("\r\n", 2).append(depth, '\t');
                                }
                        }
                        break;

                case iA2:
                        s.append(depth, '\t');

                        leadingspace = s.size();

                        if (option & OTHER_COUNT)
                        {
                                s.append("<A2[", 4) << NumberOfItem << "]";
                        }
                        else
                                s.append("<A2", 3);

                        leadingspace = s.size() - leadingspace;

                        if (NumberOfItem)
                        {
                                unsigned short a;

                                for (loop = 0; loop < NumberOfItem; loop++)
                                {
                                        a = ((unsigned short *) pItem)[loop];

                                        if (loop % 10 == 0 && loop)
                                                s.append(leadingspace, ' ');

                                        //wsprintf(tmp, " %04X", a);
                                        s.append(1, ' ').append(HEX(a >> 8), 2).append(HEX(a & 0xFF), 2);

                                        if (loop % 10 == 9 && NumberOfItem > 10)
                                                s.append("\r\n", 2).append(depth, '\t');
                                }
                                if (loop % 10 && NumberOfItem > 10)
                                        s.append("\r\n", 2).append(depth, '\t');
                        }
                        break;

                case iB:
                        s.append(depth, '\t');

                        leadingspace = s.size();

                        if (option & OTHER_COUNT)
                        {
                                s.append("<B[", 3) << NumberOfItem << "]";
                        }
                        else
                                s.append("<B", 2);

                        leadingspace = s.size() - leadingspace;

                        if (NumberOfItem)
                        {
                                unsigned char a;

                                for (loop = 0; loop < NumberOfItem; loop++)
                                {
                                        a = ((unsigned char *)pItem)[loop];

                                        if (loop % 20 == 0 && loop)
                                                s.append(leadingspace, ' ');

                                        s.append(1, ' ').append(HEX(a), 2);

                                        if (loop % 20 == 19 && NumberOfItem > 20)
                                                s.append("\r\n", 2).append(depth, '\t');
                                }
                                if (loop % 20 && NumberOfItem > 20)
                                        s.append("\r\n", 2).append(depth, '\t');
                        }
                        break;

                case iBOO:
                        s.append(depth, '\t');

                        leadingspace = s.size();

                        if (option & OTHER_COUNT)
                        {
                                s.append("<BOOLEAN[", 9) << NumberOfItem << "]";
                        }
                        else
                                s.append("<BOOLEAN", 8);

                        if (NumberOfItem)
                        {
                                for (loop = 0; loop < NumberOfItem; loop++)
                                {
                                        if (loop % 10 == 0 && loop)
                                                s.append(leadingspace, ' ');

                                        if (((unsigned char *)pItem)[loop])
                                                s.append(" T", 2);
                                        else
                                                s.append(" F", 2);

                                        if (loop % 10 == 9 && NumberOfItem > 10)
                                                s.append("\r\n", 2).append(depth, '\t');
                                }
                                if (NumberOfItem > 10 && loop % 10)
                                        s.append("\r\n", 2).append(depth, '\t');
                        }
                        break;
        }
        
        if (option & FULL_STOP)
                s.append(">.\r\n", 4);
        else
                s.append(">\r\n", 3);

        return 0;
}

int CHSMS::SendSecsMessage(CSecsMessage & msg, const unsigned int sb, unsigned int option, const __uint64 userdata, const __uint64 userdata2, const unsigned int t3v, const __uint64 channel)
{

    if (PortState != HSMS_SELECTED)
    {
        return -1;
    }

    string text;
    char tmp[14] = {0};
    char *stream = tmp + 6;
    char *function = tmp + 7;

    //set the device id
    if (option & OPTION_MESSAGE_DEVICE_ID)
            *(unsigned short int *) (tmp + 4) = Swap16(msg.GetDeviceID());
    else
            *(unsigned short int *) (tmp + 4) = Swap16(DeviceID);

    *stream = msg.GetStream();
    *function = msg.GetFunction();
    * (unsigned int *) (tmp + 10) = Swap32(sb);   //CW temporary
    if (*function % 2)
    {
            if (msg.GetWaitBit())
            {
                    *stream |= 0x80;
            }
    }
    else
    {
            * (unsigned int *) (tmp + 10) = Swap32(sb);                                                                                     //system byte
    }

	msg.ConvertDtoT(0);
     cout << "Sending message " << endl << msg.Description.c_str() << endl;

     if (msg.BinarySize() == 0 || (option & OPTION_SUPPRESS_BREGEN) == 0)
       msg.ConvertDtoB();

     * (unsigned int *) tmp = Swap32((unsigned long) (msg.BinarySize() + 10));                       //HSMS Length

     Output.PostUserMessage(new SECSOut(text, string(tmp, 14).append(msg.BStream()), option, userdata, userdata2));

     return 0;
}

int CSecsDataItem::FillBinary(string & cws)
{
        unsigned int loop;
        int nlengthbyte;                //can be 1, 2, or 3;
        int x;
        int length;                             //      NumberOfItem * x

        switch (Type)
        {
                case iL:
                        length = NumberOfItem;
                        break;

                case iA: case iJ: case iI1: case iU1: case iBOO: case iB:
                        length = NumberOfItem * 1;
                        break;

                case iU2: case iA2: case iI2:
                        length = NumberOfItem * 2;
                        break;

                case iU4: case iF4: case iI4:
                        length = NumberOfItem * 4;;
                        break;

                case iU8: case iI8: case iF8:
                        length = NumberOfItem * 8;
                        break;

                default:
                        cout << "Bad type in FillBinary " << Type << endl;
        }

        if (length < 256)
                nlengthbyte = 1;
        else
        {
                if (length < 65536)
                        nlengthbyte = 2;
                else
                        nlengthbyte = 3;
        }

        if (lb && lb > nlengthbyte)
                nlengthbyte = lb;                       //temporary for LaserFront

        cws.append(1, Type | nlengthbyte);
        x = Swap32(length);
        cws.append(((char *) &x) + 4 - nlengthbyte, nlengthbyte);

        switch (Type)
        {
                case iL:
                        for (loop = 0; loop < NumberOfItem; loop++)
                                (* (vector <CSecsDataItem> *) pItem)[loop].FillBinary(cws);
                        break;

                case iA: case iJ: case iI1: case iU1: case iBOO: case iB:
                        if (length)
                                cws.append(pItem, length);
                        break;

                case iU2: case iA2: case iI2:
                        if (length)
                        {
                                char * s = new char [length];
                                swab(pItem, s, length);
                                cws.append(s, length);
                                delete [] s;
                        }
                        break;

                case iU4: case iF4: case iI4:
                        if (length)
                        {
                                unsigned int * s = new unsigned int[NumberOfItem];
                                MiscFunction::xhilo4b(s, (unsigned int *) pItem, NumberOfItem);
                                cws.append((const char *) s, length);
                                delete [] s;
                        }
                        break;

                case iU8: case iI8: case iF8:
                        if(length)
                        {
                                __uint64 * s = new __uint64[NumberOfItem];
                                MiscFunction::xhilo8b(s, (__uint64 *)  pItem, NumberOfItem);
                                cws.append((const char *) s, length);
                                delete [] s;
                        }
                        break;
        }
        return 0;
}
