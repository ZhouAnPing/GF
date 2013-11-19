#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <algorithm>

#ifdef _STP_NOT_WINDOWS
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
typedef ssize_t READSIZE_T;

#else

#include <io.h>
#define open _open
#define fstat _fstat
#define stat _stat
#define read _read
typedef int READSIZE_T;

#endif

#include "STP_Basic.h"
#include "STP_Integer.h"

using namespace std;

#ifdef _STP_NOT_WINDOWS
int ReadFileIntoSTLString(LPCSTR FileName, string &s, STP_LARGE_INTEGER * pft, int appendflag, size_t n)                        //return 0 if no problem
#else
int ReadFileIntoSTLString(LPCSTR FileName, string &s, STP_LARGE_INTEGER * pft, int appendflag, unsigned int n)                        //return 0 if no problem
#pragma warning( disable : 4996)

#endif
{
        int r = 0;
        
    int fd = open(FileName, O_RDONLY);
    if (fd != -1)
        {
                READSIZE_T dwRead = 0;
        
        struct stat st;
        fstat(fd, &st);

                if (pft)
        {
#ifdef _STP_LINUX
            pft->i = 0;
            pft->H.l.u = st.st_mtim.tv_sec;
            pft->u *= 10000000;            //make it into 100 ns unit
            pft->u += (st.st_mtim.tv_nsec / 100);
#else
            pft->i = 0;
#ifdef _STP_NOT_WINDOWS
            pft->H.l.u = st.st_mtime;
#else
                        pft->u = st.st_mtime;
#endif
            pft->u *= 10000000;            //make it into 100 ns unit
#endif
        }

                if (appendflag == 0)
                {
                        s.clear();
                }
        
        s.reserve(st.st_size);
        
        char * ptr = new char [n];
        do
        {
            s.append(ptr, dwRead);
            dwRead = read (fd, ptr, n);
        }
        while (dwRead > 0);
        
        if (dwRead < 0)
            r = errno;
        
        delete [] ptr;
    }
    else
    {
        r = errno;
    }
        
        return r;
}

void _stp_itoa(string & dest, int val)
{
        char s[32];
        char * p = s;
           
    if (val < 0)
    {
        *p++ = '-';
        val = -val;
    }
    char * first = p;

    do
    {
        *p++ = '0' + val % 10;
        val /= 10;
    } while (val);
    *p = 0;
        
        while (first < --p)
        {
                swap(*first++, *p);
        }
        
        dest.append(s);
}

void _stp_itoa(string & dest, unsigned int val)
{
        char s[32];
        char * p = s;
        char * first = p;

    do
    {
        *p++ = ('0' + val % 10);
        val /= 10;
    } while (val);
    *p = 0;

        while (first < --p)
        {
                swap(*first++, *p);
        }
        
        dest.append(s);
}


void _stp_itoa(string & dest, DWORD val)
{
        char s[32];
        char * p = s;
        char * first = p;

    do
    {
        *p++ = (char) ('0' + val % 10);
        val /= 10;
    } while (val);
    *p = 0;

        while (first < --p)
        {
                swap(*first++, *p);
        }
        
        dest.append(s);
}

void _stp_itoa(string & dest, __int64 val)
{   
    char s[32];
        char * p = s;
           
    if (val < 0)
    {
        *p++ = '-';
        val = -val;
    }
    char * first = p;

    do
    {
        dest.append(1, (char) ('0' + val % 10));
        val /= 10;
    } while (val);
    *p = 0;
        
        while (first < --p)
        {
                swap(*first++, *p);
        }
        
        dest.append(s);
}

void _stp_itoa(string & dest, __uint64 val)
{
        char s[32];
        char * p = s;
        char * first = p;

    do
    {
        *p++ = (char) ('0' + val % 10);
        val /= 10;
    } while (val);
    *p = 0;

        while (first < --p)
        {
                swap(*first++, *p);
        }
        
        dest.append(s);
}

string & operator << (string & s, const string & x)
{
        return s.append(x);
}

const char * operator << (string & s, const void * x)
{
        return s.c_str();
}

string & operator << (string & dest, int val)
{
        char s[32];
        char * p = s;
           
    if (val < 0)
    {
        *p++ = '-';
        val = -val;
    }
    char * first = p;

    do
    {
        *p++ = '0' + val % 10;
        val /= 10;
    } while (val);
    *p = 0;
        
        while (first < --p)
        {
                swap(*first++, *p);
        }
        
        return dest.append(s);
}

string & operator << (string & s, const char * x)
{
        return s.append(x);
}

string & operator << (string & dest, unsigned int val)
{
        char s[32];
        char * p = s;
        char * first = p;

    do
    {
        *p++ = ('0' + val % 10);
        val /= 10;
    } while (val);
    *p = 0;

        while (first < --p)
        {
                swap(*first++, *p);
        }
        
        return dest.append(s);
}

string & operator << (string & dest, __int64 val)
{
        char s[32];
        char * p = s;
           
    if (val < 0)
    {
        *p++ = '-';
        val = -val;
    }
    char * first = p;

    do
    {
        *p++ = (char) ('0' + val % 10);
        val /= 10;
    } while (val);
    *p = 0;
        
        while (first < --p)
        {
                swap(*first++, *p);
        }
        
        return dest.append(s);
}

string & operator << (string & dest, __uint64 val)
{
        char s[32];
        char * p = s;
        char * first = p;

    do
    {
        *p++ = (char) ('0' + val % 10);
        val /= 10;
    } while (val);
    *p = 0;

        while (first < --p)
        {
                swap(*first++, *p);
        }
        
        return dest.append(s);
}


string & operator << (string & dest, DWORD val)
{
        char s[32];
        char * p = s;
        char * first = p;

    do
    {
        *p++ = (char) ('0' + val % 10);
        val /= 10;
    } while (val);
    *p = 0;

        while (first < --p)
        {
                swap(*first++, *p);
        }
        
        return dest.append(s);
}

string & operator << (string & s, const float c)
{
        char tmp[32];
        char * p;

        //_gcvt_s(tmp, 32, c, 6);
    gcvt(c, 6, tmp);

        if (p = strchr(tmp, '.'))               //assignment
        {
                if (p != tmp)           //if . is not the first character
                {
                        if (* (p + 1))                          //if . is not the last character
                        {
                                if (* (p + 1) > '9')            //if it is in scientific format
                                {
                                        ++p;
                                        memmove(p + 1, p, 32 - (p - tmp));
                                        p[0] = '0';
                                }
                        }
                        else                    //if . is the last character, just append a 0 at the end
                                strcat(p + 1, "0");
                        
                }
                else                            //if . is the first character, just insert 0 before the .
                {
                        memmove(p + 1, p, 31);
                        p[0] = '0';
                }
        }
        return s.append(tmp);
}

string & operator << (string & s, const double c)
{
        char tmp[32];
        char * p;

        //_gcvt_s(tmp, 32, c, 15);
    gcvt(c, 15, tmp);

        if (p = strchr(tmp, '.'))               //assignment
        {
                if (p != tmp)           //if . is not the first character
                {
                        if (* (p + 1))                          //if . is not the last character
                        {
                                if (* (p + 1) > '9')            //if it is in scientific format
                                {
                                        ++p;
                                        memmove(p + 1, p, 32 - (p - tmp));
                                        p[0] = '0';
                                }
                        }
                        else                    //if . is the last character, just append a 0 at the end
                                strcat(p + 1, "0");
                        
                }
                else                            //if . is the first character, just insert 0 before the .
                {
                        memmove(p + 1, p, 31);
                        p[0] = '0';
                }
        }
        return s.append(tmp);
}
