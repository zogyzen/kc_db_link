#pragma once

#include "stddrv.h"

class CJsonObj;
class CJsonManager : public IJsonManager
{
    friend class CJsonObj;

public:
    CJsonManager(void);
    virtual ~CJsonManager(void);

    IJsonObj& create(void) override;
    void free(IJsonObj&) override;
};

class CJsonObj : public IJsonObj
{
public:
    friend class CJsonManager;
    CJsonObj(CJsonManager&, CJsonObj* = nullptr, string = "");
    ~CJsonObj(void) override;

    IJsonManager& manager(void) override;

    const char* toStr(void) const override;
    void fromStr(const char*) override;

    const char* getVal(void) const override;
    void setVal(const char*) override;

    bool existChild(const char*) const override;
    IJsonObj& childObj(const char*) override;
    IJsonObj& addChild(const char*, IJsonObj&) override;

    IJsonObj& operator[](const char*) override;
    IJsonObj& operator=(const char*) override;
    IJsonObj& operator=(int) override;
    operator const char*(void) const override;

private:
    CJsonManager &m_man;
    CJsonObj* const m_own = nullptr;
    vector<std::shared_ptr<CJsonObj>> m_subs;
    string m_name;
    property_tree::ptree m_pt;
};
extern "C"
{
    IJsonManager& GetJsonInf(const char*);
    void freeJsonInf(void);
}
