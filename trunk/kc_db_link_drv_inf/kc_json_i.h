#pragma once

namespace KC
{
    // json interface
    class IJsonManager;
    class IJsonObj
    {
    public:
        virtual ~IJsonObj(void) = default;
        virtual IJsonManager& manager(void) = 0;

        virtual const char* toStr(void) const = 0;
        virtual void fromStr(const char*) = 0;

        virtual const char* getVal(void) const = 0;
        virtual void setVal(const char*) = 0;

        virtual bool existChild(const char*) const = 0;
        virtual IJsonObj& childObj(const char*) = 0;
        virtual IJsonObj& addChild(const char*, IJsonObj&) = 0;

        virtual IJsonObj& operator[](const char*) = 0;
        virtual IJsonObj& operator=(const char*) = 0;
        virtual IJsonObj& operator=(int) = 0;
        virtual operator const char*(void) const = 0;
    };
    class IJsonManager
    {
    public:
        virtual IJsonObj& create(void) = 0;
        virtual void free(IJsonObj&) = 0;
    };

    struct TJsonPack
    {
        TJsonPack(IJsonObj& jsn) : m_manager(jsn.manager()), m_json(jsn)
        {
        }
        TJsonPack(const TJsonPack& c) : m_manager(c.m_manager), m_json(c.m_json)
        {
        }
        ~TJsonPack(void)
        {
            m_manager.free(m_json);
        }
        IJsonObj& json(void)
        {
            return m_json;
        }

    private:
        IJsonManager& m_manager;
        IJsonObj& m_json;
    };
}
