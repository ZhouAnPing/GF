/* 
 * File:   misc.h
 * Author: cheng
 *
 * Created on December 27, 2012, 5:09 PM
 */

#ifndef OBJECTSECS_H
#define OBJECTSECS_H

#include <string>
#include <string.h>
#include <vector>

#include "KObject.h"

//these are the options for SepcialSend procedures
#define OPTION_SPECIALSEND                      0x00000002
#define OPTION_SPECIALSENDEX            0x00000004
#define OPTION_SPECIALSENDEX2           0x00000008
#define OPTION_SPECIALSENDEX3           0x00000010
#define OPTION_SPECIALSENDEX4           0x00000020


//the option for direction bit for SECS I
#define MESSAGE_TO_HOST                 1
#define MESSAGE_TO_EQUIPMENT    0

//the type of HSMS control messages
#define SELECT_REQUEST          0x01000000
#define SELECT_RESPONSE0        0x02000000
#define SELECT_RESPONSE1        0x02000100
#define SELECT_RESPONSE2        0x02000200
#define SELECT_RESPONSE3        0x02000300
#define LINKTEST_REQUEST        0x05000000
#define LINKTEST_RESPONSE       0x06000000
#define SEPERATE_REQUEST        0x09000000


enum TimerType
{
        TIMER_UNKNOWN,
        TIMER_T2,
        TIMER_T3,
        TIMER_T4,
        TIMER_T5,
        TIMER_T6,
        TIMER_T7,
        TIMER_T8
};

//these are the HSMS state 
enum HSMS_STATES
{
        HSMS_NOT_ENABLED = 0,
        HSMS_TCP_NOT_CONNECTED,
        HSMS_NOT_SELECTED,
        HSMS_SELECTED
};

enum SECS_message_type
{
        SECS_NORMAL,
        SECS_BINARY_STREAM,

        SECS_SERIAL_CLRDTR,
        SECS_SERIAL_SETDTR,
        SECS_SERIAL_CLRRTS,
        SECS_SERIAL_SETRTS,

        SECS_BINARY_STREAM_UDPSENDTO,
        SECS_BINARY_STREAM_UDPSENDTOEX,
        SECS_BINARY_STREAM_UDPSENDTOCHANNEL,

        SHUTDOWN_SOCKET,
        DISCONNECT_TCP,

        TO_SECSI_MODE,
        TO_RAW_MODE
};

//these are the option for SECSLog behavior
#define BYTETRACE                       1
#define S2TRACE                         2
#define OTHERTRACE                      4
#define TCPTRACE                        8
#define CONTROLTRACE            16

//option for CSecsDataItem::FillText()
#define FULL_STOP                       32
#define LIST_COUNT                      64              //put in item count for list type
#define OTHER_COUNT                     128             //put in item count for all other type
#define LIST_NLB                        256             //put in NLB for list
#define OTHER_NLB                       512             //put in NLB for all other type

#define iL              0                       //0x00
#define iB              32                      //0x20
#define iBOO    36                      //0x24
#define iA              64                      //0x40
#define iJ              68                      //0x44
#define iA2             72                      //0x48
#define iI8             96                      //0x60
#define iI1             100                     //0x64
#define iI2             104                     //0x68
#define iI4             112                     //0x70
#define iF8             128                     //0x80
#define iF4             144                     //0x90
#define iU8             160                     //0xA0
#define iU1             164                     //0xA4
#define iU2             168                     //0xA8
#define iU4             176                     //0xB0


//these are the availble option for SendSecsMessage functions
#define SID_SENDANDWAIT                                 0x00000001
#define OPTION_MESSAGE_DEVICE_ID                0x40000000      //bit 30, Set this to use message device id
#define OPTION_SUPPRESS_TREGEN                  0x00080000      //bit 19
#define OPTION_SUPPRESS_BREGEN                  0x00040000      //bit 18
#define OPTION_SUPPRESS_REGEN                   0x000C0000      //bit 18 and bit 19

class SerialInputData
{
public:

unsigned __int64 TimerID;
unsigned int SyncID;
unsigned __int64 U1;
unsigned __int64 U2;

SerialInputData(unsigned __int64 t, unsigned int sid, const unsigned __int64 u1, const unsigned __int64 u2)
     :TimerID(t),
     SyncID(sid),
     U1(u1),
     U2(u2)
{}
};

using  namespace std;

class CSecsDataItem
{

friend class CSecsMessage;
template <class T> friend  void SetSECSItem(CSecsDataItem &p, const unsigned int type, const unsigned int nItem, T * ptr);
template <class T> friend int GetOneItem(CSecsDataItem &p, T &x, const unsigned int i);
template <class T> friend int SetOneItem(CSecsDataItem &p, const T &x, const unsigned int i);
template <class T> friend int GetSecsItem(CSecsDataItem &p, unsigned int n, T* x);

private:
        char *pItem;                                    //pointer to the data
        int Type;                                               //the type of data
        unsigned int NumberOfItem;              //Total item number
        int lb;                                                 //suggested length byte

public:
        CSecsDataItem()
                :pItem(NULL), NumberOfItem(0), Type(iL), lb(0)
        {}

        CSecsDataItem(const CSecsDataItem &rhs)
                :pItem(NULL)
        {
                *this = rhs;
        }

        CSecsDataItem(const int type, const unsigned int n, const int lengthbyte)
                :Type(type), NumberOfItem(n), pItem(NULL), lb(lengthbyte)
        {
                if (n)
                {
                        switch (Type)
                        {
                                case iL:
                                        pItem = (char *) new vector <CSecsDataItem>(n);
                                        break;

                                case iB: case iA: case iI1: case iU1: case iJ: case iBOO:
                                        pItem = new char [n];
                                        break;
                                
                                case iA2: case iU2: case iI2:
                                        pItem = new char [n * 2];
                                        break;

                                case iU4: case iI4: case iF4:
                                        pItem = new char [n * 4];
                                        break;
                                        
                                case iF8: case iU8: case iI8:
                                        pItem = new char [n * 8];
                                        break;

                                default:
                                        //_RPT0(_CRT_ASSERT, "Bad type");
                                        break;
                        }
                }
        }

        ~CSecsDataItem()
        {
                delete_pItem();
        }


        CSecsDataItem& operator() (const int type, const unsigned int n, const int lengthbyte)
        {
                return SetHeader(type, n, lengthbyte);
        }

        CSecsDataItem& SetHeader(const int type, const unsigned int n, const int lengthbyte)
        {
                delete_pItem();

                Type = type;
                lb = lengthbyte;
                NumberOfItem = 0;

                if (n)
                {
                        switch (Type)
                        {
                                case iL:
                                        pItem = (char *) new vector <CSecsDataItem>(n);
                                        break;

                                case iB: case iA: case iI1: case iU1: case iJ: case iBOO:
                                        pItem = new char [n];
                                        break;
                                
                                case iA2: case iU2: case iI2:
                                        pItem = new char [n * 2];
                                        break;

                                case iU4: case iI4: case iF4:
                                        pItem = new char [n * 4];
                                        break;
                                        
                                case iF8: case iU8: case iI8:
                                        pItem = new char [n * 8];
                                        break;

                                default:
                                        //_RPT0(_CRT_ASSERT, "Bad type");
                                        break;
                        }
                }
                NumberOfItem = n;
                return *this;
        }

        CSecsDataItem& operator[](const unsigned int index)
        {
                if (index >= NumberOfItem || Type != iL)
                        throw(0);
                return (* (vector <CSecsDataItem> *) pItem)[index];
        }

        void SetLengthByte(const unsigned int lengthbyte)
        {
                switch (lengthbyte)
                {
                        case 1:
                        case 2:
                        case 3:
                                lb = lengthbyte;
                                return;

                        default:
                                lb = 0;
                                return;
                }
        }

        void delete_pItem()
        {
                if (pItem)
                {
                        if (Type == iL)
                                delete (vector <CSecsDataItem> *) pItem;
                        else
                                delete [] pItem;
                        pItem = NULL;
                }
        }

        //return pointer to the data 
        char * GetPointer(const unsigned int n)
        {
                switch (Type)
                {
                        case iL:
                                return (char *) &((* (vector <CSecsDataItem> *) pItem)[n]);

                        case iA: case iB: case iBOO: case iJ: case iI1: case iU1:
                                return pItem + n;

                        case iI2: case iU2: case iA2:
                                return pItem + n * 2;

                        case iI4: case iU4: case iF4:
                                return pItem + n * 4;

                        case iI8: case iU8: case iF8:
                                return pItem + n * 8;

                }
                //_RPT0(_CRT_ASSERT, "Bad type");
                return NULL;
        }

        unsigned int GetNumberOfItem()
        {
                return NumberOfItem;
        }

        unsigned int GetBinaryData(void * ptr, const unsigned int ptr_size)
        {
                unsigned int actual_copied;
                switch (Type)
                {
                        case iL:
                                actual_copied = 0;
                                break;

                        case iI1:case iU1:case iBOO:case iB:case iJ:case iA:
                                actual_copied = min(NumberOfItem, ptr_size);
                                break;

                        case iA2:case iU2:case iI2:
                                actual_copied = min(NumberOfItem * 2, ptr_size);
                                break;

                        case iI4:case iF4:case iU4:
                                actual_copied = min(NumberOfItem * 4, ptr_size);
                                break;

                        case iU8:case iF8:case iI8:
                                actual_copied = min(NumberOfItem * 8, ptr_size);
                                break;

                        default:
                                actual_copied = 0;
                                break;
                }
                memcpy(ptr, pItem, actual_copied);
                return actual_copied;
        }

        int TypeToB()
        {
                if (Type == iL)
                        return 1;

                Type = iB;
                return 0;
        }

        int TypeToJ()
        {
                if (Type == iL)
                        return 1;

                Type = iJ;
                return 0;
        }

        int TypeToA()
        {
                if (Type == iL)
                        return 1;

                Type = iA;
                return 0;
        }

        int Insert(const unsigned int x)
        {
                if (x < ++NumberOfItem)
                {
                        switch (Type)
                        {
                                case iL:                
                                        if (pItem)
                                                ((vector <CSecsDataItem> *) pItem)->insert(((vector <CSecsDataItem> *) pItem)->begin() + x, CSecsDataItem());
                                        else
                                                return 1;
                                        break;

                                case iB: case iA: case iI1: case iU1: case iJ: case iBOO:
                                {
                                        char * pTemp = new char[NumberOfItem];
                                        memcpy(pTemp, pItem, x);
                                        memcpy(pTemp + x + 1, pItem + x, NumberOfItem - x);
                                        delete [] pItem;
                                        pItem = pTemp;
                                        break;
                                }

                                case iA2: case iU2: case iI2:
                                {
                                        char * pTemp = new char [NumberOfItem * 2];
                                        memcpy(pTemp, pItem, x * 2);
                                        memcpy(pTemp + (x + 1) * 2, pItem + x * 2, (NumberOfItem - x) * 2);
                                        delete [] pItem;
                                        pItem = pTemp;
                                        break;
                                }

                                case iU4: case iI4: case iF4:
                                {
                                        char * pTemp = new char [NumberOfItem * 4];
                                        memcpy(pTemp, pItem, x * 4);
                                        memcpy(pTemp + (x + 1) * 4, pItem + x * 4, (NumberOfItem - x) * 4);
                                        delete [] pItem;
                                        pItem = pTemp;
                                        break;
                                }
                                                
                                case iF8: case iU8: case iI8:
                                {
                                        char * pTemp = new char [NumberOfItem * 8];
                                        memcpy(pTemp, pItem, x * 8);
                                        memcpy(pTemp + (x + 1) * 8, pItem + x * 8, (NumberOfItem - x) * 8);
                                        delete [] pItem;
                                        pItem = pTemp;
                                        break;
                                }

                                default:
                                        //_RPT0(_CRT_ASSERT, "Bad type");
                                        break;
                        }
                        return 0;
                }
                return 1;
        }

        int Remove(const unsigned int x)
        {
                if (x < NumberOfItem)
                {
                        switch (Type)
                        {
                                case iL:                
                                        if (pItem)
                                        {
                                                ((vector <CSecsDataItem> *) pItem)->erase(((vector <CSecsDataItem> *) pItem)->begin() + x);
                                                --NumberOfItem;
                                        }
                                        else
                                                return 1;
                                        break;

                                case iB: case iA: case iI1: case iU1: case iJ: case iBOO:
                                {
                                        --NumberOfItem;
                                        char * pTemp = new char[NumberOfItem];
                                        memcpy(pTemp, pItem, x);
                                        memcpy(pTemp + x, pItem + x + 1, NumberOfItem - x);
                                        delete [] pItem;
                                        pItem = pTemp;
                                        break;
                                }

                                case iA2: case iU2: case iI2:
                                {
                                        --NumberOfItem;
                                        char * pTemp = new char [NumberOfItem * 2];
                                        memcpy(pTemp, pItem, x * 2);
                                        memcpy(pTemp + x * 2, pItem + (x + 1) * 2, (NumberOfItem - x) * 2);
                                        delete [] pItem;
                                        pItem = pTemp;
                                        break;
                                }

                                case iU4: case iI4: case iF4:
                                {
                                        --NumberOfItem;
                                        char * pTemp = new char [NumberOfItem * 4];
                                        memcpy(pTemp, pItem, x * 4);
                                        memcpy(pTemp + x * 4, pItem + (x + 1) * 4, x * 4);
                                        delete [] pItem;
                                        pItem = pTemp;
                                        break;
                                }
                                                
                                case iF8: case iU8: case iI8:
                                {
                                        --NumberOfItem;
                                        char * pTemp = new char [NumberOfItem * 8];
                                        memcpy(pTemp, pItem, x * 8);
                                        memcpy(pTemp + x * 8, pItem + (x + 1) * 8, x * 8);
                                        delete [] pItem;
                                        pItem = pTemp;
                                        break;
                                }

                                default:
                                        //_RPT0(_CRT_ASSERT, "Bad type");
                                        break;
                        }
                        return 0;
                }
                return 1;
        }
        void Resize(const unsigned int x)
        {
                switch (Type)
                {
                        case iL:
                                if (pItem)
                                        ((vector <CSecsDataItem> *) pItem)->resize(x);
                                else
                                        pItem = (char *) new vector <CSecsDataItem>(x);
                                break;

                        case iB: case iA: case iI1: case iU1: case iJ: case iBOO:
                        {
                                char * pTemp = new char[x];
                                memcpy(pTemp, pItem, min(x, NumberOfItem));
                                delete [] pItem;
                                pItem = pTemp;
                                break;
                        }
                                
                        case iA2: case iU2: case iI2:
                        {
                                char * pTemp = new char [x * 2];
                                memcpy(pTemp, pItem, min(x, NumberOfItem) * 2);
                                delete [] pItem;
                                pItem = pTemp;
                                break;
                        }

                        case iU4: case iI4: case iF4:
                        {
                                char * pTemp = new char [x * 4];
                                memcpy(pTemp, pItem, min(x, NumberOfItem) * 4);
                                delete [] pItem;
                                pItem = pTemp;
                                break;
                        }
                                        
                        case iF8: case iU8: case iI8:
                        {
                                char * pTemp = new char [x * 8];
                                memcpy(pTemp, pItem, min(x, NumberOfItem) * 8);
                                delete [] pItem;
                                pItem = pTemp;
                                break;
                        }

                        default:
                                //_RPT0(_CRT_ASSERT, "Bad type");
                                break;
                }

                NumberOfItem = x;
        }

        void SetData_A(const string &s)
        {
                SetHeader(iA, (unsigned int) s.size(), 0);
                memcpy(pItem, s.c_str(), s.size());
        }
        
        void SetData_A(const char * s, size_t len)
        {
            SetHeader(iA, len, 0);
            memcpy(pItem, s, len);
        }

        void SetData_X(const unsigned int type, const unsigned int nItem, const void * ptr, const size_t ptr_size)
        {
                SetHeader(type, nItem, 0);
                memcpy(pItem, ptr, ptr_size);           //todo, need to put in the correct length
        }

        void ToText(string &s);

        void GetBinaryData(string &s)
        {
                switch (Type)
                {
                        case iL:
                                s.clear();
                                break;

                        case iI1:case iU1:case iBOO:case iB:case iJ:case iA:
                                if (NumberOfItem)
                                        s.assign(pItem, NumberOfItem);
                                else
                                        s.clear();
                                break;

                        case iA2:case iU2:case iI2:
                                if (NumberOfItem)
                                        s.assign(pItem, NumberOfItem * 2);
                                else
                                        s.clear();
                                break;

                        case iI4:case iF4:case iU4:
                                if (NumberOfItem)
                                        s.assign(pItem, NumberOfItem * 4);
                                else
                                        s.clear();
                                break;

                        case iU8:case iF8:case iI8:
                                if (NumberOfItem)
                                        s.assign(pItem, NumberOfItem * 8);
                                else
                                        s.clear();
                                break;

                        default:
                                s.clear();
                                break;
                }
        }

        string GetBinaryData()
        {
                switch (Type)
                {
                        case iL:
                                return string();

                        case iI1:case iU1:case iBOO:case iB:case iJ:case iA:
                                if (NumberOfItem)
                                        return string(pItem, NumberOfItem);
                                else
                                        return string();

                        case iA2:case iU2:case iI2:
                                if (NumberOfItem)
                                        return string(pItem, NumberOfItem * 2);
                                else
                                        return string();

                        case iI4:case iF4:case iU4:
                                if (NumberOfItem)
                                        return string(pItem, NumberOfItem * 4);
                                else
                                        return string();

                        case iU8:case iF8:case iI8:
                                if (NumberOfItem)
                                        return string(pItem, NumberOfItem * 8);
                                else
                                        return string();

                        default:
                                return string();
                }
        }

        int GetType()
        {
                return Type;
        }

        int FillText(string &s, unsigned int depth, unsigned int option);
        int FillBinary(string &cws);
        unsigned int Secs2Parse(const char *secs2, unsigned int &offset, const unsigned int maxlength);
        
        int operator== (const CSecsDataItem &rhs);
        CSecsDataItem& operator=(const CSecsDataItem &rhs)              //deep copy
        {
                unsigned int loop;

                SetHeader(rhs.Type, rhs.NumberOfItem, rhs.lb);
                lb = rhs.lb;

                if (NumberOfItem)
                {
                        switch (Type)
                        {
                                case iL:
                                        for (loop = 0; loop < NumberOfItem; loop++)
                                                (*(vector <CSecsDataItem> *) pItem)[loop] = (*(vector <CSecsDataItem> *) rhs.pItem)[loop];
                                        break;

                                case iI1: case iU1: case iA: case iBOO: case iJ: case iB:
                                        memcpy(pItem, rhs.pItem, NumberOfItem);
                                        break;

                                case iU2: case iA2: case iI2:
                                        memcpy(pItem, rhs.pItem, NumberOfItem * 2);
                                        break;

                                case iI4: case iF4: case iU4:
                                        memcpy(pItem, rhs.pItem, NumberOfItem * 4);
                                        break;

                                case iU8: case iF8: case iI8:
                                        memcpy(pItem, rhs.pItem, NumberOfItem * 8);
                                        break;
                        }
                }
                return *this;
        }
};

class CSecsMessage : public KObject
{

protected:
        
        int Stream;
        int Function;

        int DeviceID;
        int WaitBit;

        string BinaryStream;

        virtual KType_enum GetKType()
        {
                return KTYPE_MESSAGE;
        }
        

public:
        CSecsDataItem *Item;                    //The body in CSecsDataItem format

        CSecsMessage(const unsigned int stream, const unsigned int function, const unsigned int waitbit, const unsigned int deviceid)
                :WaitBit(waitbit), Item(NULL)
        {
                Stream = stream > 127 ? 0 : stream;
                Function = function > 255 ? 0 : function;
                DeviceID = deviceid > 32767 ? 0 : deviceid;
        }
        CSecsMessage()
                :Stream(1),Function(1), WaitBit(1), DeviceID(10), Item(NULL)
        {}

        CSecsMessage(const CSecsMessage &rhs)
                :KObject(rhs)
        {
                Stream = rhs.Stream;
                Function = rhs.Function;
                WaitBit = rhs.WaitBit;
                DeviceID = rhs.DeviceID;
                BinaryStream.assign(rhs.BinaryStream);

                if (rhs.Item)
                {
                        Item = new CSecsDataItem(*rhs.Item);
                }
                else
                        Item = NULL;
        }

        virtual ~CSecsMessage()
        {
                if (Item)
                        delete Item;
        }

    size_t BinarySize()
    {
        return BinaryStream.size();
    }
    
    const string & BStream()
    {
        return BinaryStream;
    }
    
        int InsertItem(const unsigned int pos)
        {
                if (Item)
                        return Item->Insert(pos);
                return 1;
        }

        int RemoveItem(const unsigned int pos)
        {
                if (Item)
                        return Item->Remove(pos);
                return 1;
        }

        void Assign(const CSecsDataItem &rhs)
        {
                if (Item)
                        *Item = rhs;
                else
                        Item = new CSecsDataItem(rhs);
        }

        //s is a SECS II binary stream including header, length is size of s
        //user code must ensure that s > 10
        int Assign(const char *s, const unsigned int length)
        {
                DeviceID = (s[0] << 8 & 0x7fff) + s[1];
                WaitBit = s[2] & 0x80;
                Stream = s[2] & 0x7f;
                Function = (unsigned char) s[3];
                if (Item)
                {
                        delete Item;
                        Item = NULL;
                }
        
                BinaryStream.assign(s + 10, length - 10);
                return ConvertBtoD();
        }

        //return reference of Text
        //safe to manipulate Text
        string& GetText()
        {
                return Description;
        }

        //return const char * of Text
        //do not manipulate the content (i.e. do not change the content)
        const char * GetTextStr()
        {
                return Description.c_str();
        }

        //SetFunctions
        void SetWaitBit(const int waitbit)
        {
                WaitBit = waitbit;
        }

        void SetDeviceID(unsigned int deviceid)
        {
                DeviceID = deviceid;
        }
        
        void SetStream(const unsigned int s)
        {
                Stream = s;
        }

        void SetFunction(const unsigned int f)
        {
                Function = f;
        }

        void SetSFW(const unsigned int s, const unsigned int f, const unsigned int w)
        {
                Stream = s & 0x7F;
                Function = f & 0xFF;
                WaitBit = w;
        }

        //Get functions
        int GetWaitBit()
        {
                return WaitBit;
        }

        int GetStream()
        {
                return Stream;
        }

        int GetFunction()
        {
                return Function;
        }

        int GetDeviceID()
        {
                return DeviceID;
        }

        //return the number of item in SECS item
        //return 0 if there is no SECS item
        unsigned int GetNumberOfItem()
        {
                if (Item)
                        return Item->NumberOfItem;
                else
                        return 0;
        }

        //return the enumerated type of SECS item
        //return -1 of 0xFFFFFFFF if this message has not SECS item
        int GetType()
        {
                if (Item)
                        return Item->Type;
                else
                        return -1;
        }

        //Get the pointer of CSecsMessage::Item
        CSecsDataItem * GetPointer()
        {
                return Item;
        }

        //if Item is pointing to a list item, return pointer to CSecsDataItem
        //if Item is pointing to a SECS item, return pointer to the data itself
        void * GetPointer(const unsigned int n)
        {
                if (Item)
                        return Item->GetPointer(n);
                else
                        return NULL;
        }

        void ClearItem()
        {
                if (Item)
                {
                        delete Item;
                        Item = NULL;    
                }
                BinaryStream.erase();
        }

        //Data convertion functions
        int ConvertDtoB()
        {
                BinaryStream.clear();
                if (Item)
                        return Item->FillBinary(BinaryStream);
                return 0;
        }

        int ConvertBtoD();
        int ConvertDtoT(const unsigned int option);
        int ConvertDtoT(string &t, const unsigned int option, unsigned int depth = 0, unsigned int appendflag = 0)
        {
                if (appendflag)
                        t.append(depth, '\t') << "S" << Stream << "F" << Function;
                else
                        t.assign(depth, '\t') << "S" << Stream << "F" << Function;
                if (WaitBit)
                        t.append(" W", 2);

                if (Item)
                {
                        t.append("\r\n", 2);
                        Item->FillText(t, depth, option | FULL_STOP);
                }
                else
                        t.append(".\r\n", 3);
                return 0;
        }

        CSecsMessage& operator()(const int type, const unsigned int n, const int lengthbyte = 0)
        {
                if (Item == NULL)
                        Item = new CSecsDataItem(type, n, lengthbyte);
                else
                        Item->SetHeader(type, n, lengthbyte);

                return *this;
        }

        CSecsDataItem& operator[](const int x)
        {
                return (*Item)[x];
        }

        int operator==(const CSecsMessage &rhs)
        {
                if (Stream != rhs.Stream || Function != rhs.Function)
                        return 0;

                if (Item)
                {
                        if (rhs.Item)
                                return *Item == *rhs.Item;
                        else
                                return 0;
                }
                else
                {
                        if (rhs.Item)
                                return 0;
                        else
                                return 1;
                }
        }

        CSecsMessage& operator=(const CSecsMessage &rhs)
        {
                Stream = rhs.Stream;
                Function = rhs.Function;
                WaitBit = rhs.WaitBit;
                DeviceID = rhs.DeviceID;
                Description.assign(rhs.Description);
                Name.assign(rhs.Name);

                if (Item)
                        delete Item;

                if (rhs.Item)
                        Item = new CSecsDataItem(* rhs.Item);
                else
                        Item = NULL;
                return *this;
        }
        
        //set Item to point to pItem, and pItem will be set to NULL
        void SetItem(CSecsDataItem * &pItem)
        {
                if (Item)
                        delete Item;
                Item = pItem;
                pItem = NULL;
        }
};

class SECSOut
{
public:
        string Description;             //the SECS 2 description
        string S;                               //the SECS 2 stream (for HSMS : inclusive of the HSMS length)
        unsigned int SyncID;    //the SyncID of this message
        __uint64 U1;
        __uint64 U2;
        unsigned int T3v;
        __uint64 Channel;
        SECS_message_type Type; //message type  (SECS_NORMAL, SECS_BINARY_STREAM, SECS_ASCII_STREAM)
        unsigned short Retry;                           //only used in UDPSECS
        unsigned short CurrentBlock;                    //only used in UDPSECS

        SECSOut(const string &dscrptn, const string &s, const unsigned int sid, const __uint64 u1, const __uint64 u2, const SECS_message_type type = SECS_NORMAL, const unsigned int t3v = 0, const __uint64 channel = 0)
                :Description(dscrptn),
                S(s),
                SyncID(sid),
                U1(u1),
                U2(u2),
                Type(type),
                T3v(t3v),
                Channel(channel),
                Retry(0),
                CurrentBlock(0)
        {}
};

class CSecsTag
{
public:
        unsigned int SystemByte;
        unsigned int ReplyFlag;
        __uint64 User1;
        __uint64 User2;
        __uint64 DeviceID;

        CSecsTag(const unsigned int sb, const __uint64 u1, const __uint64 u2, const __uint64 did)
        :
                SystemByte(sb),
                ReplyFlag(0),
                User1(u1),
                User2(u2),
                DeviceID(did)
        {}

	CSecsTag()
	:
                SystemByte(0),
                ReplyFlag(0),
                User1(0),
                User2(0),
                DeviceID(0)
	{}
};

class CSecsTag2 : public CSecsTag
{
public:
    
    CSecsMessage * Msg;
    
    CSecsTag2(CSecsMessage * msg, const unsigned int sb, const __uint64 u1, const __uint64 u2, const __uint64 did)
    :
        Msg(msg),
        CSecsTag(sb, u1, u2, did)
        {}

    CSecsTag2()
	{}
    
    ~CSecsTag2()
    {
        delete Msg;
    }
};


#endif  /* OBJECTSECS_H */

