/**************************************

Chew Cheng Wu

13 June 2004

This class needs stl

***************************************/

#ifndef KOBJECT_H
#define KOBJECT_H

#include "STP_Basic.h"
#include "STP_Integer.h"

enum KType_enum
{
        KTYPE_CLOSE_FOLDER,
        KTYPE_OPEN_FOLDER,      
        KTYPE_FOLDER_END,
        KTYPE_CENGINECONFIG,
        KTYPE_CLASS,
        KTYPE_TASK,
        KTYPE_MESSAGE,
        KTYPE_FILE
};

class KObject
{
public:
        string Name;
        string Description;
        KObject()
        {}

        KObject(const KObject &rhs)
        {
                Name = rhs.Name;
                Description = rhs.Description;
        }

        KObject(const string & name, const string & desc)
                :Name(name), Description(desc)
        {}

        virtual KType_enum GetKType() = 0;

        virtual ~KObject()
        {}
};

class KClosedFolder : public KObject
{
public:
        KClosedFolder(const KObject & rhs)
                : KObject(rhs)
        {}

        KClosedFolder(const string & name)
        {
                Name.assign(name);
        }
        virtual KType_enum GetKType()
        {
                return KTYPE_CLOSE_FOLDER;
        }
};

class KOpenedFolder : public KObject
{
public:
        KOpenedFolder(const string & name)
        {
                Name.assign(name);
        }
        KOpenedFolder(const KObject & rhs)
                : KObject(rhs)
        {}

        virtual KType_enum GetKType()
        {
                return KTYPE_OPEN_FOLDER;
        }
};

class KEndFolder : public KObject
{
public:
        virtual KType_enum GetKType()
        {
                return KTYPE_FOLDER_END;
        }
};

class KFile : public KObject
{
public:
        STP_LARGE_INTEGER ftime;
    
#ifdef _STP_NOT_WINDOWS
#else
    KFile(const FILETIME &f)
    {
                ftime.FT.dwHighDateTime = f.dwHighDateTime;
                ftime.FT.dwLowDateTime = f.dwLowDateTime;
    }
#endif
    
    virtual KType_enum GetKType()
    {
            return KTYPE_FILE;
    }
};

/*
class StringHelper
{
public:
        static void assign_number(string &s, const unsigned __int64 c)
        {
                char tmp[32];
                _ui64toa_s(c, tmp, 32, 10);
                s.assign(tmp);
        }

        static void assign_number(string &s, const __int64 c)
        {
                char tmp[32];
                _i64toa_s(c, tmp, 32, 10);
                s.assign(tmp);
        }

        static void assign_number(string &s, const unsigned int c)
        {
                char tmp[32];
                _ultoa_s(c, tmp, 32, 10);
                s.assign(tmp);
        }

        static void assign_number(string &s, const int c)
        {
                char tmp[32];
                _ltoa_s(c, tmp, 32, 10);
                s.assign(tmp);
        }

        static void assign_number(string &s, const float c)
        {
                char tmp[32];
                size_t x;

                _gcvt_s(tmp, 32, c, 6);
                s.assign(tmp);
                x = s.find('.');
                if (x != string::npos)
                {
                        if (x)
                        {
                                x++;
                                if (x == s.size())
                                        s.append(1, '0');
                                else
                                {
                                        if (s[x] > '9')
                                                s.insert(x, 1, '0');
                                }
                        }
                        else
                                s.insert(0, 1, '0');
                }
        }

        static void assign_number(string &s, const double c)
        {
                char tmp[32];
                size_t x;

                _gcvt_s(tmp, 32, c, 15);
                s.assign(tmp);
                x = s.find('.');
                if (x != string::npos)
                {
                        if (x)
                        {
                                x++;
                                if (x == s.size())
                                        s.append(1, '0');
                                else
                                {
                                        if (s.at(x) > '9')
                                                s.insert(x, 1, '0');
                                }
                        }
                        else
                                s.insert(0, 1, '0');
                }
        }

        static void append_number(string &s, const unsigned __int64 c)
        {
                char tmp[32];
                _ui64toa_s(c, tmp, 32, 10);
                s.append(tmp);
        }

        static void append_number(string &s, const __int64 c)
        {
                char tmp[32];
                _i64toa_s(c, tmp, 32, 10);
                s.append(tmp);
        }

        static void append_number(string &s, const unsigned int c)
        {
                char tmp[32];
                _ultoa_s(c, tmp, 32, 10);
                s.append(tmp);
        }

        static void append_number(string &s, const int c)
        {
                char tmp[32];
                _ltoa_s(c, tmp, 32, 10);
                s.append(tmp);
        }

        static void append_number(string &s, const float c)
        {
                char tmp[32];
                size_t x;
                size_t i = s.size();

                _gcvt_s(tmp, 32, c, 6);
                s.append(tmp);
                x = s.find('.', i);
                if (x != string::npos)
                {
                        if (x != i)                             //check for . to appear as first character
                        {
                                x++;
                                if (x == s.size())              //check for x to appear as last character
                                        s.append(1, '0');
                                else
                                {
                                        if (s.at(x) > '9')
                                                s.insert(x, 1, '0');
                                }
                        }
                        else
                                s.insert(i, 1, '0');
                }
        }

        static void append_number(string &s, const double c)
        {
                char tmp[32];
                size_t x;
                size_t i = s.size();
                _gcvt_s(tmp, 32, c, 15);
                s.append(tmp);
                x = s.find('.', i);
                if (x != string::npos)
                {
                        if (x != i)                             //check for . to appear as first character
                        {
                                x++;
                                if (x == s.size())              //check for x to appear as last character
                                        s.append(1, '0');
                                else
                                {
                                        if (s.at(x) > '9')
                                                s.insert(x, 1, '0');
                                }
                        }
                        else
                                s.insert(i, 1, '0');
                }
        }

        static void to_number(unsigned long &i, const string &s)
        {
                i = MiscFunction::ATOL_10_EX(s.c_str());
        }

        static void to_number(char &i, const string &s)
        {
                i = MiscFunction::ATOL_10_EX(s.c_str());
        }

        static void to_number(unsigned char &i, const string &s)
        {
                i = MiscFunction::ATOL_10_EX(s.c_str());
        }

        static void to_number(short int &i, const string &s)
        {
                i = MiscFunction::ATOL_10_EX(s.c_str());
        }

        static void to_number(unsigned short int &i, const string &s)
        {
                i = MiscFunction::ATOL_10_EX(s.c_str());
        }

        static void to_number(int &i, const string &s)
        {
                i = MiscFunction::ATOL_10_EX(s.c_str());
        }

        static void to_number(unsigned int &i, const string &s)
        {
                i =  MiscFunction::ATOL_10_EX(s.c_str());
        }

        static void to_number(__int64 &i, const string &s)
        {
                i = MiscFunction::ATOI64_10_EX(s.c_str());
        }

        static void to_number(unsigned __int64 &i, const string &s)
        {
                i = MiscFunction::ATOI64_10_EX(s.c_str());
        }

        static void to_number(float &i, const string &s)
        {
                _CRT_FLOAT x;
                if (strictlydouble(s.c_str(), s.size()) || _atoflt(&x, (char *) s.c_str()))
                        throw string("Failed to convert \"").append(s).append("\" to type float");
                i = x.f;
        }

        static void to_number(double &i, const string &s)
        {
                _CRT_DOUBLE x;
                if (strictlydouble(s.c_str(), s.size()) || _atodbl(&x, (char *) s.c_str()))
                        throw string("Failed to convert \"").append(s).append("\" to type double");
                i = x.x;
        }
};
 * 
 * */

#endif
