#pragma once

namespace kc_db
{
    class IVal
    {
    public:
        enum EValType { evtInt = 0, evtFloat, evtString };

    public:
        virtual int GetID(void) = 0;
        virtual IVal& Set(int) = 0;
        virtual IVal& Set(double) = 0;
        virtual IVal& Set(const char*) = 0;
        virtual int AsInt(void) = 0;
        virtual double AsFloat(void) = 0;
        virtual const char* AsStr(void) = 0;
        virtual EValType GetType(void) = 0;
    };

    class IRecodeSet
    {
    public:
        virtual int GetID(void) = 0;
        virtual int FieldCount(void) = 0;
        virtual const char* FieldName(int) = 0;
        virtual int FieldType(int i) = 0;
        virtual IVal& Value(int) = 0;
        virtual IVal& Value(const char*) = 0;
        virtual bool Next(void) = 0;
    };

    class IParms
    {
    public:
        virtual int GetID(void) = 0;
        virtual IVal& NewVal(void) = 0;
        virtual void Add(const char* sName, IVal&) = 0;
        virtual IVal& Get(const char* sName) = 0;
        virtual int GetCount(void) = 0;
        virtual const char* GetParmName(int i) = 0;
    };

    class IDbHelper
    {
    public:
        virtual bool Init(IParms&) = 0;
        virtual IParms& CreateParm(void) = 0;
        virtual int Exec(const char* sSql, IParms* = nullptr) = 0;
        virtual IRecodeSet& Query(const char* sSql, IParms* = nullptr) = 0;
        virtual void Release(IRecodeSet&) = 0;
        virtual void Release(IParms&) = 0;
    };
}
