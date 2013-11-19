/* 
 * File:   misc.h
 * Author: cheng
 *
 * Created on December 27, 2012, 5:09 PM
 */

#ifndef MISC_H
#define MISC_H

#include "STP_Basic.h"

extern const char A[];

#define HEX(x) (A + ((unsigned int) (x & 0xFF)) * 2)

#ifdef _STP_BIG_ENDIAN
#define SelfSwap16(x)
#define SelfSwap32(x)
#define SelfSwap64(x)
#define Swap16(x) x
#define Swap32(x) x
#define Swap64(x) x
#else
#define SelfSwap16(x) x = MiscFunction::_byteswap_ushort(x)
#define SelfSwap32(x) x = MiscFunction::_byteswap_ulong(x)
#define selfSwap64(x) x = MiscFunction::_byteswap_uint64(x)
#define Swap16(x) MiscFunction::_byteswap_ushort(x)
#define Swap32(x) MiscFunction::_byteswap_ulong(x)
#define Swap64(x) MiscFunction::_byteswap_uint64(x)
#endif

class MiscFunction
{
public:
    static unsigned long _byteswap_ulong(unsigned long i)
    {
        unsigned int j;
        j =  (i << 24);
        j += (i <<  8) & 0x00FF0000;
        j += (i >>  8) & 0x0000FF00;
        j += (i >> 24);
        return j;
    }

    static unsigned short _byteswap_ushort(unsigned short i)
    {
        unsigned short j;
        j =  (i << 8) ;
        j += (i >> 8) ;
        return j;
    }

    static __uint64 _byteswap_uint64(__uint64 i)
    {
        __uint64 j;
        j =  (i << 56);
        j += (i << 40) & 0x00FF000000000000;
        j += (i << 24) & 0x0000FF0000000000;
        j += (i <<  8) & 0x000000FF00000000;
        j += (i >>  8) & 0x00000000FF000000;
        j += (i >> 24) & 0x0000000000FF0000;
        j += (i >> 40) & 0x000000000000FF00;
        j += (i >> 56);
        return j;
    }

        //void hilocopy(void* dst, const void *src, const int size)
        //This routine copy size byte from scr to dst in reverse order
        static void hilocopy(void* dst, const void *src, const int size)
        {
                int x;
                const char *sptr = (const char *) src;
                char *dptr=(char *) dst;
                
                sptr += size;
                for(x = 0; x < size; x++)
                        dptr[x] = *(--sptr);
        }
    
    //hilo copy n elements of 4-bytes
        static void xhilo4b(unsigned int * dst, const unsigned int * src, unsigned int n)
        {
                while (n--)
                {
                        * dst = Swap32(* src);
                        ++dst;
                        ++src;
                }
        }

        //hilo copy n elements of 8-bytes
        static void xhilo8b(__uint64  * dst, const __uint64 * src, unsigned int n)
        {
                while (n--)
                {
                        * dst = Swap64(* src);
                        ++dst;
                        ++src;
                }
        }
};



#endif  /* MISC_H */

