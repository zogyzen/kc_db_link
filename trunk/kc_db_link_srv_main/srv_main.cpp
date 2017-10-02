#include "srv_main.h"

////////////////////////////////////////////////////////////////////////////////
// CKCSrvMain类
CKCSrvMain::CKCSrvMain(const IBundle& bundle)
    : m_context(bundle.getContext()), m_bundle(bundle), m_exit(false)
    // 读配置文件中的参数
    , m_MsgInName(m_context.GetCfgInfo("Config.Parameters.message_queue_in_name", "value", "kc_db_message_queue_in_v10"))
    , m_MsgOutName(m_context.GetCfgInfo("Config.Parameters.message_queue_out_name", "value", "kc_db_message_queue_out_v10"))
    , m_MsgSize(lexical_cast<unsigned>(string(m_context.GetCfgInfo("Config.Parameters.message_queue_size", "value", "100"))))
    , m_MemName(m_context.GetCfgInfo("Config.Parameters.shared_memory_name", "value", "kc_db_shared_memory_v10"))
    , m_MemSize(lexical_cast<unsigned>(string(m_context.GetCfgInfo("Config.Parameters.shared_memory_size", "value", "8388608"))))
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
        cout << "Start Message Queue" << endl;
        m_context.WriteLogInfo(m_context.getHint("Message_queue_start"), __FUNCTION__, c_KCSrvMainSrvGUID);
        IServiceReferenceEx &wk = dynamic_cast<IBundleContextEx&>(m_context).getServiceRef(c_KCSrvWorkSrvGUID);
        // 创建
        message_queue::remove(m_MsgInName.c_str());
        message_queue::remove(m_MsgOutName.c_str());
        shared_memory_object::remove(m_MemName.c_str());
        message_queue mqIn(create_only, m_MsgInName.c_str(), m_MsgSize, sizeof(int));
        message_queue mqOut(create_only, m_MsgOutName.c_str(), m_MsgSize, sizeof(int));
        managed_shared_memory mhm(create_only, m_MemName.c_str(), m_MemSize);
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
                int iLen = *(int*)pRequest;
                string sRequest;
                sRequest.append((char*)pRequest + sizeof(int), (char*)pRequest + iLen);
                mhm.deallocate(pRequest);
                // 处理请求
                wk.getServiceSafe<IKCSrvWork>().request(sRequest.c_str(), iLen, *this);
            }
        }
        // 退出
        message_queue::remove(m_MsgInName.c_str());
        message_queue::remove(m_MsgOutName.c_str());
        shared_memory_object::remove(m_MemName.c_str());
        m_context.WriteLogInfo(m_context.getHint("Message_queue_end"), __FUNCTION__, c_KCSrvMainSrvGUID);
    }
    catch(std::exception &ex)
    {
        cout << "Message Queue Exception - " << (string("[") + typeid(ex).name() + "] " + ex.what()) << endl;
        m_context.WriteLogFatal(m_context.getHint("Message_queue_exception"), __FUNCTION__, (string("[") + typeid(ex).name() + "] " + ex.what()).c_str());
    }
}

// 响应结果
void CKCSrvMain::respond(const char* res, int len)
{
    cout << res << endl;
    try
    {
        message_queue mqOut(open_only, m_MsgOutName.c_str());
        managed_shared_memory mhm(open_only, m_MemName.c_str());
        void* pRes = mhm.allocate(len + sizeof(int));
        memcpy(pRes, &len, sizeof(len));
        memcpy((char*)pRes + sizeof(len), res, len);
        int numHandle = mhm.get_handle_from_address(pRes);
        mqOut.send(&numHandle, sizeof(numHandle), 0);
    }
    catch (std::exception &ex)
    {
        m_context.WriteLogFatal(m_context.getHint("Respond_exception"), __FUNCTION__, (string("[") + typeid(ex).name() + "] " + ex.what() + "\n" + res).c_str());
    }
}
