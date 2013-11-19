/* 
 * File:   CCondVar.h
 * Author: cheng
 *
 * Created on January 3, 2013, 10:17 AM
 */

#ifndef CCONDVAR_H
#define CCONDVAR_H

#include "CCriticalSection.h"

#ifdef _STP_NOT_WINDOWS
#include <pthread.h>
#include <set>

using namespace std;

class CCondVar
{
    pthread_cond_t Condition;
    pthread_mutex_t Mutex;
    int flag;
    
public:
    CCondVar() : flag(0)
    {
        pthread_mutex_init (&Mutex, NULL);
        pthread_cond_init (&Condition, NULL);
    }
    
    ~CCondVar()
    {
        pthread_mutex_destroy(&Mutex);
        pthread_cond_destroy(&Condition);
    }
    
    void Set()
    {
        pthread_mutex_lock(&Mutex);
        flag = 1;
        pthread_cond_broadcast(&Condition);
        pthread_mutex_unlock(&Mutex);
    }
    
    void Reset()
    {
        pthread_mutex_lock(&Mutex);
        flag = 0;
        pthread_mutex_unlock(&Mutex);
    }
    
    void Wait()
    {
        if (flag == 0)
        {
            pthread_mutex_lock(&Mutex);
            while (flag == 0)
                pthread_cond_wait(&Condition, &Mutex);
            pthread_mutex_unlock(&Mutex);
        }
    }
    
    int Wait(unsigned int ms)
    {
        timespec ts;
        int rc = 0;
        
        if (flag)
            return 0;
        
        clock_gettime(CLOCK_REALTIME, &ts);
        
        ts.tv_nsec += ((ms % 1000) * 1000000);
        ts.tv_sec += (ms / 1000);
        
        pthread_mutex_lock(&Mutex);
        while (flag == 0 && rc == 0)
        {
            rc = pthread_cond_timedwait(&Condition, &Mutex, &ts);
        }
        rc = flag;
        pthread_mutex_unlock(&Mutex);
        
        if (flag)
            return 0;
        return 1;           //return 1 if timeout
    }
};
#else

#include <windows.h>
class CCondVar
{
        HANDLE Event;

public:
        CCondVar()
        {
                Event = CreateEvent(NULL, TRUE, FALSE, NULL);
        }

        ~CCondVar()
        {
                CloseHandle(Event);
        }

        void Set()
        {
                SetEvent(Event);
        }

        void Reset()
        {
                ResetEvent(Event);
        }

        void Wait()     
        {
                WaitForSingleObject(Event, INFINITE);
        }

        //return 1 if time out
        int Wait(unsigned int ms)
        {
                if (WaitForSingleObject(Event, ms) == WAIT_OBJECT_0)
                        return 0;

                return 1;
        }
};

#endif

class CEventUnit
{
     friend class CEventPool;

     unsigned int SCount;          //if this go to zero, then the CEventUnit can go back to the available pool
     unsigned int data1;

public:
     CCondVar hEvent;

     CEventUnit(unsigned int count) : SCount(count), data1(0)
     {}

     ~CEventUnit()
     {}
};

class CEventPool
{
     CCriticalSection cs;
     set <CEventUnit *> AvailableEvent;
     set <CEventUnit *> InUseEvent;

public:
     CEventPool()
     {}

     ~CEventPool()
     {
          set <CEventUnit *>::iterator i;
          cs.Enter();
          if (InUseEvent.size())
          {
               i = InUseEvent.begin();
               while (i != InUseEvent.end())
               {
                    delete *i;          //delete CEventUnit *
                    ++i;
               }
          }

          i = AvailableEvent.begin();
          while (i != AvailableEvent.end())
          {
               delete *i;
               ++i;
          }
          cs.Leave();

     }

     CEventUnit * RequestNewEvent(const unsigned int releasecount)
     {
          CEventUnit * pEventUnit = NULL;
          if (releasecount)
          {
               cs.Enter();
               set <CEventUnit *>::iterator i = AvailableEvent.begin();
               if (i != AvailableEvent.end())
               {
                    pEventUnit = *i;
                    InUseEvent.insert(pEventUnit);
                    pEventUnit->SCount = releasecount;
                    pEventUnit->hEvent.Reset();
                    AvailableEvent.erase(i);
               }
               else
               {
                    InUseEvent.insert(pEventUnit = new CEventUnit(releasecount));
               }
               cs.Leave();
          }
          return pEventUnit;
     }

     void DropEvent(CEventUnit * &pEventUnit)
     {
          cs.Enter();
          InUseEvent.erase(pEventUnit);
          AvailableEvent.insert(pEventUnit);
          cs.Leave();
          pEventUnit = NULL;
     }

     //return the data1
     unsigned int Release(CEventUnit * &pEventUnit, const unsigned int scount = 1)
     {
          unsigned int x;
          cs.Enter();
          x = pEventUnit->data1;
          if ((pEventUnit->SCount -= scount) == 0)
          {
               AvailableEvent.insert(pEventUnit);
               InUseEvent.erase(pEventUnit);
               pEventUnit = NULL;
          }
          cs.Leave();
          return x;
     }

     //Set the event, set the data, and release SCount by 1, then return the new SCount
     unsigned int ReleaseSet(CEventUnit * pEventUnit, const unsigned int data)
     {
          unsigned int x;
          cs.Enter();
          pEventUnit->data1 = data;
          pEventUnit->hEvent.Set();
          if ((--pEventUnit->SCount) == 0)
          {
               AvailableEvent.insert(pEventUnit);
               InUseEvent.erase(pEventUnit);
          }
          x = pEventUnit->SCount;
          cs.Leave();
          return x;
     }

     //if SCount is >= mincount, then CS is entered (up to user to release the CS), return 1
     //else SCount is release by 1, if SCount is decremented to 0, then the CEventUnit is removed
     int SpecialEnterCS(CEventUnit * p, const unsigned int mincount)
     {
          cs.Enter();
          if (p->SCount < mincount)
          {
               if (--p->SCount == 0)
               {
                    AvailableEvent.insert(p);
                    InUseEvent.erase(p);
               }
               cs.Leave();
               return 0;
          }
          return 1;
     }

     //Release SCount, if still valid, set data and event, else remove PEventUnit
     void LeaveCSReleaseSet(CEventUnit * p, const unsigned int d)
     {
          if (--p->SCount)
          {
               p->data1 = d;
               p->hEvent.Set();
          }
          else
          {
               AvailableEvent.insert(p);
               InUseEvent.erase(p);
          }
          cs.Leave();
     }

     void LeaveCS()
     {
          cs.Leave();
     }
};


#endif  /* CCONDVAR_H */

