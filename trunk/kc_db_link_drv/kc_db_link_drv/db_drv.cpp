#include "db_drv.h"

const int c_EachMessageQueueSize = 30;

atomic_bool CKCSrvDbDrv::m_isRunning(false);
thread_local string CKCSrvDbDrv::m_lastError;

extern "C"
{
    static CKCSrvDbDrv* g_dbdrv = nullptr;
    KC::IKCSrvDbDrv& InitDbDrv(const char* path)
    {
        if (nullptr == g_dbdrv) g_dbdrv = new CKCSrvDbDrv(path);
        return *g_dbdrv;
    }
    void UninitDbDrv(void)
    {
        delete g_dbdrv;
        g_dbdrv = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CKCSrvDbDrv类
CKCSrvDbDrv::CKCSrvDbDrv(const char* path) : m_index(0)
{
    string sCfg = path;
    sCfg += "/config.xml";
    if (boost::filesystem::exists(sCfg))
    {
        boost::property_tree::ptree pt;
        read_xml(sCfg, pt);
        if (pt.get_child_optional("Config.Parameters.message_queue_name"))
            m_MsgName = pt.get<string>("Config.Parameters.message_queue_name.<xmlattr>.value");
        if (pt.get_child_optional("Config.Parameters.message_queue_size"))
            m_MsgSize = lexical_cast<unsigned>(pt.get<string>("Config.Parameters.message_queue_size.<xmlattr>.value"));
        if (pt.get_child_optional("Config.Parameters.shared_memory_name"))
            m_MemName = pt.get<string>("Config.Parameters.shared_memory_name.<xmlattr>.value");
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
            IKCSrvDbRespond* pRes = nullptr;
            {
                boost::unique_lock<boost::mutex> lck(m_resMtx);
                auto iter = m_resList.find(sRec);
                if (m_resList.end() != iter)
                {
                    pRes = &iter->second.res;
                    m_resList.erase(iter);
                }
            }
            if (nullptr != pRes)
            {
                auto fres = [=](void)
                {
                    try
                    {
                        pRes->respond(sRespond.c_str(), sRespond.length());
                    }
                    catch (...)
                    {
                    }
                };
                volatile boost::thread tBack(fres);
            }
        }
        while (m_isRunning);
    }
    catch (std::exception &ex)
    {
        m_isRunning = false;
    }
}

// 启动
bool CKCSrvDbDrv::start(void)
{
    if (m_isRunning)
    {
        m_lastError = "已经启动了一个驱动实例";
        return false;
    }
    try
    {
        if (!m_netEnable)
            m_thrd = new boost::thread(&CKCSrvDbDrv::startMQ, this);
        m_isRunning = true;
    }
    catch (std::exception &ex)
    {
        m_isRunning = false;
        m_lastError = ex.what();
    }
    return m_isRunning;
}

// 结束
void CKCSrvDbDrv::end(void)
{
    try
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
    catch (std::exception &ex)
    {
        m_lastError = ex.what();
    }
}

// 请求
bool CKCSrvDbDrv::request(const char* req, int len, IKCSrvDbRespond& res)
{
    if (!m_isRunning)
    {
        m_lastError = "驱动实例没启动";
        return false;
    }
    bool bResult = false;
    try
    {
        string sName = "p" + lexical_cast<string>(getpid()) + "_" + lexical_cast<string>(++m_index);
        {
            boost::unique_lock<boost::mutex> lck(m_resMtx);
            m_resList.insert(make_pair(sName, TRespondInf(res)));
        }
        if (!m_netEnable)
        {
            message_queue mq(open_only, m_MsgName.c_str());
            managed_shared_memory mhm(open_only, m_MemName.c_str());
            char* pReq = mhm.construct<char>(sName.c_str())[len](0);
            memcpy(pReq, req, len);
            mq.send(sName.c_str(), c_EachMessageQueueSize, 0);
        }
        bResult = true;
    }
    catch (std::exception &ex)
    {
        bResult = false;
        m_lastError = ex.what();
    }
    return bResult;
}

// 得到上次的错误
const char* CKCSrvDbDrv::getLastError(void)
{
    return m_lastError.c_str();
}
