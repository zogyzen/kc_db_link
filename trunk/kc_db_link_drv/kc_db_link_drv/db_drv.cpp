#include "db_drv.h"

const int c_EachMessageQueueSize = 30;

////////////////////////////////////////////////////////////////////////////////
// CKCSrvDbDrv类
CKCSrvDbDrv::CKCSrvDbDrv(const char* cfg) : m_isRunning(false), m_index(0)
{
    if (boost::filesystem::exists(cfg))
    {
        boost::property_tree::ptree pt;
        read_xml(cfg, pt);
        if (pt.get_child_optional("Config.Parameters.message_queue_name"))
            m_MsgName = pt.get<string>("Config.Parameters.message_queue_name.<xmlattr>.value");
        if (pt.get_child_optional("Config.Parameters.message_queue_size"))
            m_MsgSize = lexical_cast<unsigned>(pt.get<string>("Config.Parameters.message_queue_size.<xmlattr>.value"));
        if (pt.get_child_optional("Config.Parameters.message_queue_size"))
            m_MemName = pt.get<string>("Config.Parameters.message_queue_size.<xmlattr>.value");
        if (pt.get_child_optional("Config.Parameters.srv_net_enable"))
            m_netEnable = pt.get<string>("Config.Parameters.srv_net_enable.<xmlattr>.value") == "1";
        if (pt.get_child_optional("Config.Parameters.srv_net_ip"))
            m_srvNetIP = pt.get<string>("Config.Parameters.srv_net_ip.<xmlattr>.value");
        if (pt.get_child_optional("Config.Parameters.srv_net_port"))
            m_srvNetPort = lexical_cast<int>(pt.get<string>("Config.Parameters.srv_net_port.<xmlattr>.value"));
    }
}

CKCSrvDbDrv::~CKCSrvDbDrv(void)
{
    this->end();
}

void CKCSrvDbDrv::startMQ(void)
{
    try
    {
        string sPidName = m_MsgName + "_p" + lexical_cast<string>(getpid());
        message_queue::remove(sPidName.c_str());
        message_queue mq(create_only, sPidName.c_str(), m_MsgSize, c_EachMessageQueueSize);
        managed_shared_memory mhm(open_only, m_MemName.c_str());
        do
        {
            unsigned int priority = 0;
            message_queue::size_type recvd_size = 0;
            char aRec[c_EachMessageQueueSize + 1] = { 0 };
            // 取消息
            mq.receive(aRec, c_EachMessageQueueSize, recvd_size, priority);
            string sRec(aRec);
            cout << "receive message - " << sRec << endl;
            // 得到消息请求内容
            pair<char*, unsigned> prReq = mhm.find<char>(aRec);
            string sRespond;
            sRespond.append(prReq.first, prReq.first + prReq.second);
            mhm.destroy<char>(aRec);
            // 响应回调
        }
        while (m_isRunning);
    }
    catch (std::exception &ex)
    {
        m_isRunning = false;
    }
}

// 启动
void CKCSrvDbDrv::start(void)
{
    if (!m_netEnable)
        m_thrd = new boost::thread(&CKCSrvDbDrv::startMQ, this);
    m_isRunning = true;
}

// 结束
void CKCSrvDbDrv::end(void)
{
    m_isRunning = false;
    if (nullptr != m_thrd)
    {
        m_thrd->interrupt();
        delete m_thrd;
        m_thrd = nullptr;
    }
    if (!m_netEnable)
        message_queue::remove((m_MsgName + "_" + lexical_cast<string>(getpid())).c_str());
}

// 请求
void CKCSrvDbDrv::request(const char* req, int len, IKCSrvDbRespond& res)
{
    try
    {
        if (!m_isRunning)
            throw "not running";
        if (!m_netEnable)
        {
            string sName = "p" + lexical_cast<string>(getpid()) + "_" + lexical_cast<string>(++m_index);
            message_queue mq(open_only, m_MsgName.c_str());
            managed_shared_memory mhm(open_only, m_MemName.c_str());
            char* pReq = mhm.construct<char>(sName.c_str())[len](0);
            memcpy(pReq, req, len);
            mq.send(sName.c_str(), c_EachMessageQueueSize, 0);
        }
    }
    catch (std::exception &ex)
    {
    }
}
