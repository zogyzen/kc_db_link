#include "db_drv.h"

////////////////////////////////////////////////////////////////////////////////
// CKCSrvDbDrv类
CKCSrvDbDrv::CKCSrvDbDrv(const char* cfg)
{
    if (boost::filesystem::exists(cfg))
    {
        boost::property_tree::ptree pt;
        read_xml(cfg, pt);
        if (pt.get_child_optional("Config.Parameters.message_queue_in_name"))
            m_MsgInName = pt.get<string>("Config.Parameters.message_queue_in_name.<xmlattr>.value");
        if (pt.get_child_optional("Config.Parameters.message_queue_out_name"))
            m_MsgOutName = pt.get<string>("Config.Parameters.message_queue_out_name.<xmlattr>.value");
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

// 启动
void CKCSrvDbDrv::start(IKCSrvDbRespond& res)
{
    if (!m_netEnable)
    {
        message_queue mqOut(open_only, m_MsgOutName.c_str());
        managed_shared_memory mhm(open_only, m_MemName.c_str());
    }
    m_isRunning = true;
}

// 结束
void CKCSrvDbDrv::end(void)
{
    m_isRunning = false;
}

// 请求
void CKCSrvDbDrv::request(const char* req, int len)
{
    try
    {
        if (!m_netEnable)
        {
            message_queue mqIn(open_only, m_MsgInName.c_str());
            managed_shared_memory mhm(open_only, m_MemName.c_str());
            void* pReq = mhm.allocate(len + sizeof(int));
            memcpy(pReq, &len, sizeof(len));
            memcpy((char*)pReq + sizeof(len), req, len);
            int numHandle = mhm.get_handle_from_address(pReq);
            mqIn.send(&numHandle, sizeof(numHandle), 0);
        }
    }
    catch (std::exception &ex)
    {
    }
}
