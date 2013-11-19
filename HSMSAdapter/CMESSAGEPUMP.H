/* 
 * File:   CMessagePump.h
 * Author: cheng
 *
 * Created on December 26, 2012, 10:20 PM
 */

//needs deque
//needs CCriticalSection

#ifndef CMESSAGEPUMP_H
#define CMESSAGEPUMP_H

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
#include <poll.h>

#endif

#include <string.h>


#include <deque>


#include "CCriticalSection.h"
#include "CCondVar.h"

#include <iostream>
using namespace std;

enum _CMESSAGEPUMP
{
    CMESSAGEPUMP_ERROR = 0,
    CMESSAGEPUMP_USERPOST,
    CMESSAGEPUMP_USERQUIT,
    CMESSAGEPUMP_SYSTEMRUN = 254,
    CMESSAGEPUMP_SYSTEMQUIT
};

class CMessagePumpBase
{
protected:
    
    void * Arg;
    
public:
    CMessagePumpBase(void * arg) : Arg(arg)
    {}
};

template <class T>
class CSTPMessagePump
{
#ifdef _STP_NOT_WINDOWS
    int StartFlag;

#ifdef _STP_MESSAGEPUMP_USEUDP
    int fd;
    unsigned short int PortNumber;
#else
    int pipefd[2];
#endif
    
#else
    CCondVar Event;
    deque <unsigned char> Message;
#endif
    
    CCriticalSection cs;
        
    deque <T *> Data;
    
    public:
#ifdef _STP_NOT_WINDOWS
        
    int Start()
    {
#ifdef _STP_MESSAGEPUMP_USEUDP
        sockaddr_in serveraddr = {0};
        sockaddr_in their_addr;
        socklen_t xxx = sizeof(their_addr);
    
        if (StartFlag == 0)
        {
            if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
            {
                    cout << "socket failed" << endl;
                    return 1;
            }
            
            serveraddr.sin_family = AF_INET;
            serveraddr.sin_port = htons(0);
            serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

            if((bind(fd, (struct sockaddr *) &serveraddr, sizeof(serveraddr))) < 0)
            {
                cout << "bind failed" << endl;
                return 1;
            }
            
            if (getsockname(fd, (struct sockaddr *) &their_addr, &xxx) < 0)
            {
                cout << "getsockname failed" << endl;
            }
            
            PortNumber = ntohs(their_addr.sin_port);
#ifdef _DEBUG
            cout <<"xxx is " << xxx << endl;
            cout << "Bound port is " << PortNumber << endl;
            cout << "Address is " << htonl(their_addr.sin_addr.s_addr) << endl;
#endif
            StartFlag = 1;
            return 0;
        }
#else
        if (StartFlag == 0 && pipe(pipefd) == 0)
        {
            StartFlag = 1;
            return 0;
        }
#endif
        return 1;
    }
    
    int GetEvent()
    {
#ifdef _STP_MESSAGEPUMP_USEUDP
        if (StartFlag)
            return fd;
#else
        if (StartFlag)
            return pipefd[0];
#endif
        return -1;
    }
        
    CSTPMessagePump() : StartFlag(0)
    {
        Start();
    }
    
    virtual ~CSTPMessagePump()
    {
        if (StartFlag)
        {
            size_t loop;
#ifdef _STP_MESSAGEPUMP_USEUDP
            close(fd);
#else
            close(pipefd[0]);
            close(pipefd[1]);
#endif       
            for (loop = 0; loop < Data.size(); ++loop)
                delete Data[loop];
        }
    }
    
    int PostSystemMessage (_CMESSAGEPUMP type, T *t)
    {
        int r = 1;
        if (StartFlag)
        {
            unsigned char tmp = type;
            cs.Enter();
#ifdef _STP_MESSAGEPUMP_USEUDP
            sockaddr_in serveraddr = {0};
            
            serveraddr.sin_family = AF_INET;
            serveraddr.sin_port = htons(PortNumber);
            serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            
            
            if (sendto(fd, &tmp, 1, 0, (sockaddr *) &serveraddr, sizeof(serveraddr)) == 1)
#else
            if (write(pipefd[1], &tmp, 1) == 1)
#endif
            {
                Data.push_back(t);
                r = 0;
            }
            cs.Leave();
        }
        return r;
    }
    
    int PostSystemQuitMessage()
    {
        return PostSystemMessage(CMESSAGEPUMP_SYSTEMQUIT, new T(NULL));
    }
    
    int PostUserQuitMessage ()
    {
        int r = 1;
        if (StartFlag)
        {
            unsigned char tmp = CMESSAGEPUMP_USERQUIT;
            cs.Enter();
            
#ifdef _STP_MESSAGEPUMP_USEUDP
            sockaddr_in serveraddr = {0};
            
            serveraddr.sin_family = AF_INET;
            serveraddr.sin_port = htons(PortNumber);
            serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            
            
            if (sendto(fd, &tmp, 1, 0, (sockaddr *) &serveraddr, sizeof(serveraddr)) == 1)
#else
            if (write(pipefd[1], &tmp, 1) == 1)
#endif
            {
                Data.push_back(new T);
                r = 1;
            }
            cs.Leave();
        }
        return r;
    }
    
    int PostUserMessage(T * t)
    {
        int r = 1;
        if (StartFlag)
        {
            unsigned char tmp = CMESSAGEPUMP_USERPOST;
            cs.Enter();
#ifdef _STP_MESSAGEPUMP_USEUDP
            sockaddr_in serveraddr = {0};
            
            serveraddr.sin_family = AF_INET;
            serveraddr.sin_port = htons(PortNumber);
            serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            
            
            if (sendto(fd, &tmp, 1, 0, (sockaddr *) &serveraddr, sizeof(serveraddr)) == 1)
#else
            if (write(pipefd[1], &tmp, 1) == 1)
#endif
            {
                Data.push_back(t);
                r = 0;
            }
            cs.Leave();
        }
        return r;
    }
    
    _CMESSAGEPUMP GetMessage(T * &t)
    {
        unsigned char tmp;
        cs.Enter();
        Recheck:

        if (Data.size())
        {
#ifdef _STP_MESSAGEPUMP_USEUDP
            sockaddr_in addr;
            
            socklen_t xxx;
            
            Rogue:
            xxx = sizeof(addr);

            if (recvfrom(fd, &tmp, 1, 0, (sockaddr *) &addr ,&xxx))
            {
#ifdef _DEBUG
                in_addr x;
                cout <<"Rogue xxx is " << xxx << endl;
                cout << "Bound port is " << ntohs(addr.sin_port) << endl;
                cout << "Address is " << ntohl(addr.sin_addr.s_addr) << endl;
                x.s_addr = addr.sin_addr.s_addr;
                cout << "Test address " << inet_ntoa(x) << endl;
#endif
                
                if (ntohl(addr.sin_addr.s_addr) != 2130706433 || ntohs(addr.sin_port) != PortNumber)
                {
#ifdef _DEBUG
                    cout << "Removing rogue message " << endl;
#endif
                    goto Rogue;
                }
                
                t = Data.front();
                Data.pop_front();
            }
#else
            if (read(pipefd[0], &tmp, 1) == 1)
            {
                t = Data.front();
                Data.pop_front();
            }
#endif
                
            else
            {
                tmp = CMESSAGEPUMP_ERROR;
                t = NULL;
            }
            cs.Leave();
            return (_CMESSAGEPUMP) tmp;
        }
        cs.Leave();
        
        pollfd fds;
#ifdef _STP_MESSAGEPUMP_USEUDP
        fds.fd = fd;
#else
        fds.fd = pipefd[0];
#endif
        fds.events = POLLIN;
        
        int WaitFlag = poll(&fds, 1, -1);

        if (WaitFlag == 1)
        {
            cs.Enter();
#ifdef _STP_MESSAGEPUMP_USEUDP
            sockaddr_in addr;
            
            socklen_t xxx;
            xxx = sizeof(addr);
            
            Rogue2:
            if (recvfrom(fd, &tmp, 1, 0, (sockaddr *) &addr ,&xxx))
            {
#ifdef _DEBUG
                in_addr x;
                cout <<"Rogue2 xxx is " << xxx << endl;
                cout << "Bound port is " << ntohs(addr.sin_port) << endl;
                cout << "Address is " << ntohl(addr.sin_addr.s_addr) << endl;
                x.s_addr = addr.sin_addr.s_addr;
                cout << "Test address " << inet_ntoa(x) << endl;
#endif
                
                if (ntohl(addr.sin_addr.s_addr) != 2130706433 || ntohs(addr.sin_port) != PortNumber)
                {
#ifdef _DEBUG
                    cout << "Removing rogue message " << endl;
#endif
                    goto Recheck;
                }
                
                t = Data.front();
                Data.pop_front();
            }
#else
            if (read(pipefd[0], &tmp, 1) == 1)
            {
                t = Data.front();
                Data.pop_front();
            }
#endif
            
            else
            {
                tmp = CMESSAGEPUMP_ERROR;
                t = NULL;
            }
            cs.Leave();
            return (_CMESSAGEPUMP) tmp;
        }
        return CMESSAGEPUMP_ERROR;
    }
    
#else
    CSTPMessagePump()
    {}
    
    virtual ~CSTPMessagePump()
        {
                size_t loop;
                for (loop = 0; loop < Data.size(); ++loop)
                        delete Data[loop];
        }
    
    void PostSystemQuitMessage()
    {
        PostSystemMessage(CMESSAGEPUMP_SYSTEMQUIT, new CMessagePumpBase(NULL));
    }
 
    void PostSystemMessage (_CMESSAGEPUMP type, T *t)
    {
                cs.Enter();
                Message.push_back(type);
                Data.push_back(t);
                Event.Set();
                cs.Leave();
    }
    
    void PostUserQuitMessage ()
    {
                cs.Enter();
                Message.push_back(CMESSAGEPUMP_USERQUIT);
                Data.push_back(new T);
                Event.Set();
                cs.Leave();
    }
    
    void PostUserMessage(T * t)
    {
                cs.Enter();
                Message.push_back(CMESSAGEPUMP_USERPOST);
                Data.push_back(t);
                Event.Set();
                cs.Leave();
    }
    
    _CMESSAGEPUMP GetMessage(T * &t)
    {
        unsigned char tmp = CMESSAGEPUMP_ERROR;
        cs.Enter();
                if (Data.size() == 0)
                {
                        cs.Leave();
                        Event.Wait();
                        cs.Enter();
                }

                t = Data.front();
                Data.pop_front();
                if (Data.size() == 0)
                        Event.Reset();
                tmp = Message.front();
                Message.pop_front();

                cs.Leave();        

        return (_CMESSAGEPUMP) tmp;
    }
    
#endif
    
    size_t GetQueueSize()
    {
        size_t x;
        cs.Enter();
        x = Data.size();
        cs.Leave();
        return x;
    }
};

#endif  /* CMESSAGEPUMP_H */

