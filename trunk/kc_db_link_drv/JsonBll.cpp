#include "JsonBll.h"

static std::shared_ptr<CJsonManager> g_jsonMan;
extern "C"
{
    IJsonManager& GetJsonInf(const char*)
    {
        if (nullptr == g_jsonMan)
            g_jsonMan = std::shared_ptr<CJsonManager>(new CJsonManager);
        return *g_jsonMan;
    }
    void freeJsonInf(void)
    {
        g_jsonMan.reset();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CJsonManager
CJsonManager::CJsonManager()
{
}
CJsonManager::~CJsonManager()
{
}


IJsonObj& CJsonManager::create(void)
{
    CJsonObj *pJson = new CJsonObj(*this);
    return *pJson;
}
void CJsonManager::free(IJsonObj& jsn)
{
    delete &jsn;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CJsonObj
CJsonObj::CJsonObj(CJsonManager &m, CJsonObj *v, string n)
    : m_man(m), m_own(v), m_name(n)
{
}

CJsonObj::~CJsonObj()
{
}

IJsonManager& CJsonObj::manager(void)
{
    return m_man;
}

const char* CJsonObj::toStr(void) const
{
    static thread_local stringstream ss;
    property_tree::write_json(ss, m_pt);
    return ss.str().c_str();
}
void CJsonObj::fromStr(const char* str)
{
    stringstream ss;
    ss << str;
    property_tree::read_json(ss, m_pt);
}

const char* CJsonObj::getVal(void) const
{
    static thread_local string str;
    str = m_pt.get_value<string>();
    return str.c_str();
}
void CJsonObj::setVal(const char* v)
{
    m_pt.put_value(v);
    if (nullptr != m_own)
    {
        if (m_own->m_pt.get_child_optional(m_name))
            m_own->m_pt.put_child(m_name, m_pt);
        else
            m_own->m_pt.add_child(m_name, m_pt);
    }
}

bool CJsonObj::existChild(const char* n) const
{
    return !!m_pt.get_child_optional(n);
}
IJsonObj& CJsonObj::childObj(const char* n)
{
    CJsonObj* sub = new CJsonObj(m_man, this, n);
    if (m_pt.get_child_optional(n)) sub->m_pt = m_pt.get_child(n);
    return *sub;
}
IJsonObj& CJsonObj::addChild(const char* n, IJsonObj& subOld)
{
    CJsonObj* sub = new CJsonObj(m_man, this, n);
    sub->m_pt = dynamic_cast<CJsonObj&>(subOld).m_pt;
    m_pt.add_child(n, sub->m_pt);
    return *sub;
}

inline IJsonObj& CJsonObj::operator[](const char* n)
{
    return this->childObj(n);
}
inline IJsonObj& CJsonObj::operator=(const char* v)
{
    this->setVal(v);
    return *this;
}
inline IJsonObj& CJsonObj::operator=(int v)
{
    this->setVal(lexical_cast<string>(v).c_str());
    return *this;
}
inline CJsonObj::operator const char*(void) const
{
    return this->getVal();
}
