#pragma once

#include "stddrv.h"

// 数据库接口驱动
class CKCSrvDbDrv : public IKCSrvDbDrv
{
public:
    CKCSrvDbDrv(const char* cfg, IKCSrvDbRespond& res);

    // 初始化
    void init(IKCSrvDbRespond& res) override;
    // 请求
    void request(const char*, int len) override;

private:
    string m_MsgInName = "kc_db_message_queue_in_v10";
    string m_MsgOutName = "kc_db_message_queue_out_v10";
    string m_MemName = "kc_db_shared_memory_v10";
    bool m_netEnable = false;
    string m_srvNetIP = "127.0.0.1";
    int m_srvNetPort = 20179;
};
