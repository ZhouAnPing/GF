/* 
 * File:   CThreadPool.h
 * Author: cheng
 *
 * Created on December 27, 2012, 6:15 AM
 */

//need CMessagePump
#ifndef CTHREADPOOL_H
#define CTHREADPOOL_H

#include <set>
#include "CMessagePump.h"

#ifdef _STP_NOT_WINDOWS
#include <pthread.h>
typedef void * (*ThreadProc) (void * arg);
#else
#include <process.h>
typedef uintptr_t (*ThreadProc) (void * arg);
#endif



class CThreadPool;

class CMessagePumpBase_ThreadParameter : public CMessagePumpBase
{
    ThreadProc pThreadProc;
    
public:
    
    CMessagePumpBase_ThreadParameter(ThreadProc threadproc, void * arg)
            :pThreadProc(threadproc), CMessagePumpBase(arg)
    {}
    
    void Run()
    {
        pThreadProc(Arg);
    }
};

class CThreadUnit
{
    friend class CThreadPool;

#ifdef _STP_NOT_WINDOWS
    friend void * ThreadPoolFunction(void * arg);
#else
        friend unsigned int __stdcall ThreadPoolFunction(void * arg);
#endif

    CSTPMessagePump <CMessagePumpBase> Pump;
    
    CThreadPool * pThreadPool;  //ThreadPool that owns this ThreadInit
    unsigned int DoNotUseFlag;
    int DetachFlag;
    int State;                  //0 in idle, 1 in use, 2 not in use, but waiting for user to acknowledge end 

#ifdef _STP_NOT_WINDOWS
    pthread_t hThread;
#else
        HANDLE hThread;
#endif

    ThreadProc * pThreadProc;
    
    CThreadUnit(CThreadPool * pthreadpool) : pThreadPool(pthreadpool), DetachFlag(0), DoNotUseFlag(1), State(1)
    {}
    
    ~CThreadUnit()
    {}
    
        int Start();
};


class CThreadPool
{
#ifdef _STP_NOT_WINDOWS
    friend void * ThreadPoolFunction(void * arg);
#else
        friend unsigned int __stdcall ThreadPoolFunction(void * arg);
#endif

    CCriticalSection cs;
        set <CThreadUnit *> ReadyThread;
        set <CThreadUnit *> InUseThread;
    
    void ReturnToPool(CThreadUnit * & p);
    
public:
    CThreadPool()
    {
        //cout << "ThreadPool started" << endl;
    }

    ~CThreadPool();
    
        void EndAll();
    void Detach(CThreadUnit * &pThreadUnit);
    int RunFunction(ThreadProc proc, void * arg, CThreadUnit * &pThreadUnit, const int detachflag = 0, const int priority = 0);
};

extern CThreadPool ThreadPool;



#endif  /* CTHREADPOOL_H */

