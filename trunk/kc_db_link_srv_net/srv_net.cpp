#include "srv_net.h"

////////////////////////////////////////////////////////////////////////////////
// CKCSrvNet类
CKCSrvNet::CKCSrvNet(const IBundle& bundle)
    : m_context(bundle.getContext()), m_bundle(bundle), m_exit(false)
    // 读配置文件中的参数
    , m_port(lexical_cast<unsigned>(string(m_context.GetCfgInfo("Config.Parameters.srv_net_port", "value", "20179"))))
{
}

CKCSrvNet::~CKCSrvNet()
{
}

// 得到服务特征码
const char* CKCSrvNet::getGUID(void) const
{
    return c_KCSrvNetSrvGUID;
}

// 对应的模块
const IBundle& CKCSrvNet::getBundle(void) const
{
    return m_bundle;
}

// 启动
void CKCSrvNet::run(void)
{
    try
    {
        cout << "Start Net DB" << endl;
    }
    catch(std::exception &ex)
    {
    }
}

// 响应结果
void CKCSrvNet::respond(const char*, int)
{

}
