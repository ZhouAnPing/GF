#ifndef _STP_TIMER_H
#define _STP_TIMER_H

#ifdef _STP_NOT_WINDOWS
#include <sys/time.h>
#endif

#include "STP_Integer.h"

#include <map>

using namespace std;

template <class T>
class NewTimerUnit
{
public:
        T UID;
        unsigned int Type;
        unsigned int Tolerance;

        NewTimerUnit(const T uid, const unsigned int type, const unsigned int tolerance)
                :UID(uid), Type(type)
        {
                Tolerance = tolerance * 10000;              //tolerance is in ms, Tolerance is in 100 nanosecond
        }
};

template <class T>
class NewTimerStruct
{
public:

    map <__uint64, NewTimerUnit <T> * > TimerList;                      //absolute UTC expire time, map to UDP TimerUnit

        NewTimerStruct()
    {}

        ~NewTimerStruct ()
        {
            TYPENAME map <__uint64, NewTimerUnit <T> *>::iterator i = TimerList.begin();
            while (i != TimerList.end())
            {
                    delete i->second;
                    ++i;
            }
        }
        
        

        int GetTimeOutSID(T & sid, unsigned int & type)                 //return 1 if there is a time out
        {
                TYPENAME map <__uint64, NewTimerUnit <T> * >::iterator i = TimerList.begin();
                if (i != TimerList.end())
                {
            
                        STP_LARGE_INTEGER tmp;
                        timeval tv;
                        gettimeofday(&tv, NULL);

                        tmp.u = 0;
                        tmp.H.l.u = tv.tv_sec;
                        tmp.u *= 10000000;
                        tmp.u += tv.tv_usec * 10;
            
                        if (tmp.u + i->second->Tolerance > i->first)                      //if already expired
                        {
                                sid = i->second->UID;
                                type = i->second->Type;
                                delete i->second;
                                TimerList.erase(i);
                                return 1;
                        }
                }

                return 0;
        }
        
        __uint64 GetAbsoluteTimeOut()
        {
            TYPENAME map <__uint64, NewTimerUnit <T> * >::iterator i = TimerList.begin();
            
            if (i == TimerList.end())
                return 0;
            
            return i->first;
        }

        const __uint64 AddTimer(const __uint64 expiry, NewTimerUnit <T> * tag)                  //expiry is in milisecond
        {
            STP_LARGE_INTEGER tmp;
            timeval tv;
            gettimeofday(&tv, NULL);
        
            tmp.H.h.u = 0;
            tmp.H.l.u = tv.tv_sec;
            tmp.u *= 10000000;
            tmp.u += tv.tv_usec * 10;
            tmp.u += expiry * 10000;          //tmp is now the expiry time in FILETIME
            while (TimerList.find(tmp.u) != TimerList.end())
            {
                    ++tmp.u;
            }

                TimerList[tmp.u] = tag;
                
                //_RPT3(_CRT_WARN, "Added NewTimer %I64u, %I64u, %Iu\r\n", tmp, tag->UID, TimerList.size());
                return tmp.u;                     //this is the TimerID
        }

        void RemoveTimer(const __uint64 timerid)
        {
                if (timerid)
                {
                        TYPENAME map <__uint64, NewTimerUnit <T> * >::iterator i = TimerList.find(timerid);
                        if (i != TimerList.end())
                        {
                                //_RPT3(_CRT_WARN, "Removing NewTimer %I64u, %I64d, %Iu\r\n", i->first, i->second->UID, TimerList.size());
                                delete i->second;
                                TimerList.erase(timerid);
                        }
                        else
                        {
                                //_RPT0(_CRT_WARN, "Unable to find Timer\r\n");
                        }
                }
        }

        int CheckTimerExist(const __uint64 timerid, unsigned int & type)
        {
                TYPENAME map <__uint64, NewTimerUnit <T> * >::iterator i = TimerList.find(timerid);
                if (i != TimerList.end())
                {
                        type = i->second->Type;
                        return 1;
                }
                return 0;
        }
};

#endif
