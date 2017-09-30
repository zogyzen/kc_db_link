#include "srv_main.h"

////////////////////////////////////////////////////////////////////////////////
// CKCSrvMain类
CKCSrvMain::CKCSrvMain(const IBundle& bundle)
    : m_context(bundle.getContext()), m_bundle(bundle), m_exit(false)
    // 读配置文件中的参数
    , m_MsgInName(m_context.GetCfgInfo("message_queue_in_name", "value", "kc_db_message_queue_in_v10"))
    , m_MsgOutName(m_context.GetCfgInfo("message_queue_out_name", "value", "kc_db_message_queue_out_v10"))
    , m_MsgSize(lexical_cast<unsigned>(string(m_context.GetCfgInfo("message_queue_size", "value", "100"))))
    , m_MemName(m_context.GetCfgInfo("shared_memory_name", "value", "kc_db_shared_memory_v10"))
    , m_MemSize(lexical_cast<unsigned>(string(m_context.GetCfgInfo("shared_memory_size", "value", "8388608"))))
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
    try
    {
        // 创建
        message_queue::remove(m_MsgInName);
        message_queue::remove(m_MsgOutName);
        shared_memory_object::remove(m_MemName);
        message_queue mqIn(create_only, m_MsgInName, m_MsgSize, sizeof(int));
        message_queue mqOut(create_only, m_MsgOutName, m_MsgSize, sizeof(int));
        managed_shared_memory mhm(create_only, m_MemName, m_MemSize);
        // 启动消息队列
        while (!m_exit)
        {
            unsigned int priority = 0;
            message_queue::size_type recvd_size = 0;
            int num = 0;
            // 取消息
            mqIn.receive(&num, sizeof(num), recvd_size, priority);
            cout << "receive message - " << num << endl;
            if (num <= 0) m_exit = true;
            else
            {
                // 得到消息请求内容
                void* pRequest = mhm.get_address_from_handle(num);
                string sRequest = (char*)pRequest;
                mhm.deallocate(pRequest);
                // 处理请求
                dealRequest(sRequest);
            }
        }
        // 退出
        message_queue::remove(m_MsgInName);
        message_queue::remove(m_MsgOutName);
        shared_memory_object::remove(m_MemName);
    }
    catch(interprocess_exception &ex)
    {
    }
}

// 处理请求
void CKCSrvMain::dealRequest(string sReq)
{

}
