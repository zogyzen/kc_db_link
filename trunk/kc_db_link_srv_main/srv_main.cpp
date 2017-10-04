#include "srv_main.h"

const int c_EachMessageQueueSize = 30;

////////////////////////////////////////////////////////////////////////////////
// CKCSrvMain类
CKCSrvMain::CKCSrvMain(const IBundle& bundle)
    : m_context(bundle.getContext()), m_bundle(bundle), m_exit(false)
    // 读配置文件中的参数
    , m_MsgName(m_context.GetCfgInfo("Config.Parameters.message_queue_name", "value", "kc_db_message_queue_v10"))
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
        message_queue::remove(m_MsgName.c_str());
        shared_memory_object::remove(m_MemName.c_str());
        message_queue mq(create_only, m_MsgName.c_str(), m_MsgSize, c_EachMessageQueueSize);
        managed_shared_memory mhm(create_only, m_MemName.c_str(), m_MemSize);
        // 启动消息队列
        while (!m_exit)
        {
            unsigned int priority = 0;
            message_queue::size_type recvd_size = 0;
            char aRec[c_EachMessageQueueSize + 1] = { 0 };
            // 取消息
            mq.receive(aRec, c_EachMessageQueueSize, recvd_size, priority);
            string sRec(aRec);
            cout << "receive message - " << sRec << endl;
            if ("@exit" == sRec) m_exit = true;
            else
            {
                // 得到消息请求内容
                pair<char*, unsigned> prReq = mhm.find<char>(aRec);
                string sRequest;
                sRequest.append(prReq.first, prReq.first + prReq.second);
                mhm.destroy<char>(aRec);
                // 处理请求
                wk.getServiceSafe<IKCSrvWork>().request(aRec, sRequest.c_str(), sRequest.length(), *this);
            }
        }
        // 退出
        message_queue::remove(m_MsgName.c_str());
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
void CKCSrvMain::respond(const char* name, const char* res, int len)
{
    cout << name << " - " << res << endl;
    try
    {
        vector<string> vecPid;
        string sName(name);
        algorithm::split(vecPid, sName, algorithm::is_any_of("_"));
        message_queue mq(open_only, (m_MsgName + "_" + vecPid[0]).c_str());
        managed_shared_memory mhm(open_only, m_MemName.c_str());
        char* pRes = mhm.construct<char>(name)[len](0);
        memcpy(pRes, res, len);
        mq.send(name, c_EachMessageQueueSize, 0);
    }
    catch (std::exception &ex)
    {
        m_context.WriteLogFatal(m_context.getHint("Respond_exception"), __FUNCTION__, (string("[") + typeid(ex).name() + "] " + ex.what() + "\n" + res).c_str());
    }
}
