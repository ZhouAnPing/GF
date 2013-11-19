#ifndef _STP_INTEGER_H
#define _STP_INTEGER_H

#ifdef _STP_NOT_WINDOWS

#else
#include <windows.h>
#endif


typedef union _STP_CHAR
{
        unsigned char u;
        char c;
} STP_CHAR;

typedef union _STP_SHORT
{
#ifdef _STP_BIG_ENDIAN
        struct H
        {
        STP_CHAR h;
                STP_CHAR l;
        }       
#else
        struct _H
        {
        STP_CHAR l;
                STP_CHAR h;
        } H;

        short int i;
        unsigned short int u;

#endif

} STP_SHORT;

typedef union _STP_INTEGER
{
        struct _H
        {
#ifdef _STP_BIG_ENDIAN
                STP_SHORT h;
                STP_SHORT l;
#else
                STP_SHORT l;
                STP_SHORT h;
#endif
        } H;

        struct _Q
        {
#ifdef _STP_NOT_INTEL
        STP_CHAR q3;
                STP_CHAR q2;
                STP_CHAR q1;
                STP_CHAR q0;
#else
        STP_CHAR q0;
                STP_CHAR q1;
                STP_CHAR q2;
                STP_CHAR q3;
#endif
        } Q;
        int i;
        unsigned int u;
} STP_INTEGER;

typedef union _STP_LARGE_INTEGER
{
#ifndef _STP_NOT_WINDOWS
    LARGE_INTEGER LI;
    FILETIME FT;
#endif
  
        struct _H
        {
#ifdef _STP_BIG_ENDIAN
                STP_INTEGER h;
                STP_INTEGER l;
#else
                STP_INTEGER l;
                STP_INTEGER h;
#endif
        } H;

        struct _Q
        {
#ifdef _STP_BIG_ENDIAN
                STP_SHORT q3;
                STP_SHORT q2;
                STP_SHORT q1;
                STP_SHORT q0;
#else
                STP_SHORT q0;
                STP_SHORT q1;
                STP_SHORT q2;
                STP_SHORT q3;
#endif
        } Q;

        struct _QQ
        {
#ifdef _STP_BIG_ENDIAN
                STP_CHAR qq7;
                STP_CHAR qq6;
                STP_CHAR qq5;
                STP_CHAR qq4;
                STP_CHAR qq3;
                STP_CHAR qq2;
                STP_CHAR qq1;
                STP_CHAR qq0;
#else
                STP_CHAR qq0;
                STP_CHAR qq1;
                STP_CHAR qq2;
                STP_CHAR qq3;
                STP_CHAR qq4;
                STP_CHAR qq5;
                STP_CHAR qq6;
                STP_CHAR qq7;
        } QQ;
#endif
        
    __uint64 u;
    __int64 i;

} STP_LARGE_INTEGER;

#endif
