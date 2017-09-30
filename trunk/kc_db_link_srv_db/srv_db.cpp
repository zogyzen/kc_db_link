#include "srv_db.h"

////////////////////////////////////////////////////////////////////////////////
// CKCSrvDb类
CKCSrvDb::CKCSrvDb(const IBundle& bundle)
    : m_context(bundle.getContext()), m_bundle(bundle)
{
}

CKCSrvDb::~CKCSrvDb()
{
}

// 得到服务特征码
const char* CKCSrvDb::getGUID(void) const
{
    return c_KCSrvDbSrvGUID;
}

// 对应的模块
const IBundle& CKCSrvDb::getBundle(void) const
{
    return m_bundle;
}

// 启动
void CKCSrvDb::run(void)
{
    try
    {
    }
    catch(interprocess_exception &ex)
    {
    }
}

// 处理请求
void CKCSrvDb::dealRequest(string sReq)
{

}
