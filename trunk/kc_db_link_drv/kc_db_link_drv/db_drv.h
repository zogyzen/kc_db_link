#pragma once

#include "stddrv.h"

// 数据库接口驱动
class CKCSrvDbDrv : public IKCSrvDbDrv
{
public:
    CKCSrvDbDrv(const char* cfg);
    ~CKCSrvDbDrv(void);

    // 启动和结束
    void start(void) override;
    void end(void) override;
    // 请求
    void request(const char*, int len, IKCSrvDbRespond& res) override;

protected:
    void startMQ(void);

private:
    atomic_bool m_isRunning;
    atomic_int m_index;
    boost::thread *m_thrd = nullptr;
    // 配置
    string m_MsgName = "kc_db_message_queue_v10";
    unsigned m_MsgSize = 100;
    string m_MemName = "kc_db_shared_memory_v10";
    bool m_netEnable = false;
    string m_srvNetIP = "127.0.0.1";
    int m_srvNetPort = 20179;
};
