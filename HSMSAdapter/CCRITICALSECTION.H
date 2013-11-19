/* 
 * File:   CCriticalSection.h
 * Author: cheng
 *
 * Created on December 19, 2012, 9:23 AM
 */

#ifndef CCRITICALSECTION_H
#define CCRITICALSECTION_H

#ifdef _STP_NOT_WINDOWS
#include <pthread.h>
#else
#include <windows.h>
#endif

class CCriticalSection
{
private:
#ifdef _STP_NOT_WINDOWS
        pthread_mutex_t cs;
#else
        CRITICAL_SECTION cs;
#endif

#ifdef _DEBUG
        int CSCount;
#endif

public:
        CCriticalSection()
        {

#ifdef _STP_NOT_WINDOWS
                pthread_mutex_init(&cs, NULL);
#else
                InitializeCriticalSection(&cs);
#endif

#ifdef _DEBUG
                CSCount = 0;
#endif
        }

        ~CCriticalSection()
        {
#ifdef _STP_NOT_WINDOWS
                pthread_mutex_destroy(&cs);
#else
                DeleteCriticalSection(&cs);
#endif
        }

        void Enter()
        {
#ifdef _STP_NOT_WINDOWS
                pthread_mutex_lock(&cs);
#else
                EnterCriticalSection(&cs);
#endif
#ifdef _DEBUG
                ++CSCount;
#endif
        }

        void Leave()
        {
#ifdef _DEBUG
                --CSCount;
#endif

#ifdef _STP_NOT_WINDOWS
                pthread_mutex_unlock(&cs);
#else
                LeaveCriticalSection(&cs);
#endif
        }
};

#ifdef _STP_NOT_WINDOWS
class CCriticalSectionRecursive
{
private:
        pthread_mutex_t cs;
#ifdef _DEBUG
        int CSCount;
#endif

public:
        CCriticalSectionRecursive()
        {
                pthread_mutexattr_t attr;
                pthread_mutexattr_init(&attr);
                pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
                pthread_mutex_init(&cs, &attr);
                pthread_mutexattr_destroy(&attr);
#ifdef _DEBUG
                CSCount = 0;
#endif
        }

        ~CCriticalSectionRecursive()
        {
                pthread_mutex_destroy(&cs);
        }

        void Enter()
        {
                pthread_mutex_lock(&cs);
#ifdef _DEBUG
                ++CSCount;
#endif
        }

        void Leave()
        {
#ifdef _DEBUG
                --CSCount;
#endif
                pthread_mutex_unlock(&cs);
        }
};
#else
#define CCriticalSectionRecursive CCriticalSection
#endif

#endif  /* CCRITICALSECTION_H */

