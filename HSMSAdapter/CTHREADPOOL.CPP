#include <iostream>

#include <deque>
#include <set>
#include <errno.h>

#ifdef _STP_VMS_ALPHA
#include <unistd.h>
#endif

#ifdef _STP_VMS_ITANIUM
#include <unistd.h>
#endif

using namespace std;

#ifdef _STP_NOT_WINDOWS
#include <sys/poll.h>
#endif

#include "STP_Basic.h"
#include "CCriticalSection.h"
#include "CMessagePump.h"
#include "CThreadPool.h"

CThreadPool ThreadPool;

#ifdef _STP_NOT_WINDOWS
void * ThreadPoolFunction(void * arg)
#else
unsigned int __stdcall ThreadPoolFunction(void * arg)
#endif
{
    CMessagePumpBase * pThreadParam;
    _CMESSAGEPUMP type;
    
    CThreadUnit * pThreadUnit = (CThreadUnit *) arg;
    
    do
    {
        type = pThreadUnit->Pump.GetMessage(pThreadParam);
        
        switch (type)
        {
            case CMESSAGEPUMP_SYSTEMRUN:
                ((CMessagePumpBase_ThreadParameter *) pThreadParam)->Run();
                pThreadUnit->pThreadPool->ReturnToPool(pThreadUnit);
                delete pThreadParam;
                break;
                
            case CMESSAGEPUMP_SYSTEMQUIT:
            case CMESSAGEPUMP_USERPOST:
            case CMESSAGEPUMP_USERQUIT:
                delete pThreadParam;
                break;
        }
        
    } while (type != CMESSAGEPUMP_SYSTEMQUIT);
    return NULL;
}

void CThreadPool::ReturnToPool(CThreadUnit * & p)
{
    if (p->State == 1)          //only if the thread is running
    {
        cs.Enter();
        //SetThreadPriority(p->hThread, 0);
        if (p->DetachFlag)
        {
            p->State = 0;
            p->DoNotUseFlag = 0;    
        }
        else
        {
            p->State = 2;           //waiting for user to "join"
        }

        ReadyThread.insert(p);
        InUseThread.erase(p);

        //SetEvent(p->hIdle);
        cs.Leave();
    }
}

int CThreadPool::RunFunction(ThreadProc proc, void * arg, CThreadUnit * &pThreadUnit, const int detachflag, const int priority)
{
    pThreadUnit = NULL;

    cs.Enter();
    TYPENAME set <CThreadUnit *>::iterator i = ReadyThread.begin();
    while (i != ReadyThread.end())
    {
        if ((*i)->DoNotUseFlag == 0)            //can use this thread
        {
            pThreadUnit = *i;
            InUseThread.insert(pThreadUnit);
            ReadyThread.erase(pThreadUnit);
            break;
        }
        ++i;
    }

    if (pThreadUnit == NULL)
    {
        pThreadUnit = new CThreadUnit (this);

        if (pThreadUnit->Start())
        {
            cerr << "Create new thread failed" << endl;
            delete pThreadUnit;
            cs.Leave();
            return 1;
        }
        InUseThread.insert(pThreadUnit);
    }

    pThreadUnit->DetachFlag = detachflag;
    pThreadUnit->Pump.PostSystemMessage(CMESSAGEPUMP_SYSTEMRUN, new CMessagePumpBase_ThreadParameter(proc, arg));

    cs.Leave();

    if (detachflag)
        pThreadUnit = NULL;
    //ResetEvent(pThreadUnit->hEndEvent);
    //ResetEvent(pThreadUnit->hIdle);

    /*
    switch (priority)
    {
        case -2:
        case -1:
        case 1:
        case 2:
            SetThreadPriority(pThreadUnit->hThread, priority);
            break;

        default:
            SetThreadPriority(pThreadUnit->hThread, 0);
            break;
    }
    */


    return 0;
}

void CThreadPool::Detach(CThreadUnit * &pThreadUnit)
{
    cs.Enter();

    switch (pThreadUnit->State)
    {
        case 0:             //it is waiting
            pThreadUnit->DoNotUseFlag = 0;
            break;

        case 1:             //it is running
            pThreadUnit->DetachFlag = 1;
            break;

        case 2:             //it is waiting for user to "join"
            pThreadUnit->DoNotUseFlag = 0;
            pThreadUnit->State = 0;
            break;
    }
    cs.Leave();

        pThreadUnit = NULL;
}

void CThreadPool::EndAll()
{
        set <CThreadUnit *> ToBeDelete;
    TYPENAME set <CThreadUnit *>::iterator i;
 cout << "At Endall" << endl;
    cs.Enter();
    i = InUseThread.begin();
    while (i != InUseThread.end())
    {
        if ((*i)->DetachFlag == 0)
            cerr << "CThreadPool EndAll Warning : Found an in-use thread that is not detached" << endl;
        (*i)->Pump.PostSystemQuitMessage();
        ToBeDelete.insert(*i);
        ++i;
    }

    i = ReadyThread.begin();
    while (i != ReadyThread.end())
    {
        if ((*i)->State)
            cerr << "CThreadPool EndAll Warning : Found a ready thread that is not detached" << endl;
        
        (*i)->Pump.PostSystemQuitMessage();
        
        ToBeDelete.insert(*i);
        ++i;
    }
    cs.Leave();

cout << "Mid Endall" << endl;

    i = ToBeDelete.begin();
    while (i != ToBeDelete.end())
    {
#ifdef _STP_NOT_WINDOWS
        pthread_join((*i)->hThread, NULL);
#else
        WaitForSingleObject((*i)->hThread, INFINITE);
        CloseHandle((*i)->hThread);
#endif
        delete *i;
        ++i;
    }
    cout << "Completed EndAll" << endl;
}

CThreadPool::~CThreadPool()
{
    cout << "ThreadPool ended.  ReadyThread = " << ReadyThread.size() << ", InUseThread = " << InUseThread.size() << endl;   
}

int CThreadUnit::Start()
{
#ifdef _STP_NOT_WINDOWS
                if (pthread_create(&hThread, NULL, ThreadPoolFunction, this) == 0)
                        return 0;
#else
                hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadPoolFunction, this, 0, NULL);
                if (hThread)
                        return 0;
#endif
                return errno;
        }
