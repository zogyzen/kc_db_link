#include "srv_work.h"

////////////////////////////////////////////////////////////////////////////////
// CKCSrvWork类
CKCSrvWork::CKCSrvWork(const IBundle& bundle)
    : m_context(bundle.getContext()), m_bundle(bundle)
{
}

CKCSrvWork::~CKCSrvWork()
{
}

// 得到服务特征码
const char* CKCSrvWork::getGUID(void) const
{
    return c_KCSrvWorkSrvGUID;
}

// 对应的模块
const IBundle& CKCSrvWork::getBundle(void) const
{
    return m_bundle;
}

// 处理请求
void CKCSrvWork::request(const char* name, const char* req, int len, IRespondBackCall& res)
{
    m_context.WriteLogDebug((m_context.getHint("Message_work_") + string(name)).c_str(), __FUNCTION__, req);
    try
    {
        res.respond(name, req, len);
    }
    catch(std::exception &ex)
    {
    }
}

// 处理请求
void CKCSrvWork::dealRequest(string sReq)
{

}
