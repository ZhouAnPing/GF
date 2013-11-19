/* 
 * File:   STLHelper.h
 * Author: cheng
 *
 * Created on December 27, 2012, 4:50 PM
 */

#ifndef STLHELPER_H
#define STLHELPER_H

#include <string>
#include "STP_Basic.h"
#include "STP_Integer.h"
using namespace std;

#ifdef _STP_NOT_WINDOWS
int ReadFileIntoSTLString(LPCSTR FileName, string &s, STP_LARGE_INTEGER * pft = NULL, int appendflag = 0, size_t n = 131072);
#else
int ReadFileIntoSTLString(LPCSTR FileName, string &s, STP_LARGE_INTEGER * pft = NULL, int appendflag = 0, unsigned int n = 131072);
#endif

string & operator << (string & s, unsigned int x);
string & operator << (string & s, int x);
string & operator << (string & s, const float x);
string & operator << (string & s, const double x);
string & operator << (string & s, __int64);
string & operator << (string & s, __uint64 x);
string & operator << (string & s, DWORD x);
string & operator << (string & s, const string & x);
string & operator << (string & s, const char * x);

#endif  /* STLHELPER_H */

