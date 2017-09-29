#include "srv_main.h"

////////////////////////////////////////////////////////////////////////////////
// CKCSrvMain类
CKCSrvMain::CKCSrvMain(const IBundle& bundle)
    : m_context(bundle.getContext()), m_bundle(bundle)
{
}

CKCSrvMain::~CKCSrvMain()
{
}

// 得到服务特征码
const char* CKCSrvMain::getGUID(void) const
{
    return c_KCSrvMainSrvGUID;
}

// 对应的模块
const IBundle& CKCSrvMain::getBundle(void) const
{
    return m_bundle;
}

// 启动主程序
void CKCSrvMain::run(void)
{
}

