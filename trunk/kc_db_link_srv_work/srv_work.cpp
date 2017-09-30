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
void CKCSrvWork::request(const char* req, IRespondBackCall& res)
{
    try
    {
        res.respond(req);
    }
    catch(std::exception &ex)
    {
    }
}

// 处理请求
void CKCSrvWork::dealRequest(string sReq)
{

}
