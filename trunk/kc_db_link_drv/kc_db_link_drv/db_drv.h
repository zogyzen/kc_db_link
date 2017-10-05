#pragma once

#include "stddrv.h"

// 数据库接口驱动
class CKCSrvDbDrv : public IKCSrvDbDrv
{
public:
    CKCSrvDbDrv(const char* path);
    virtual ~CKCSrvDbDrv(void);

    // 启动和结束
    bool start(void) override;
    void end(void) override;
    // 请求
    bool request(const char*, int len, IKCSrvDbRespond& res) override;
    // 得到上次的错误
    const char* getLastError(void) override;

protected:
    void startMQ(void);

private:
    // 驱动是否启动
    static atomic_bool m_isRunning;
    // 上一次的错误
    static thread_local string m_lastError;
    // 消息的序号
    atomic_int m_index;
    // 响应消息循环的线程
    boost::thread *m_thrd = nullptr;
    // 响应列表
    boost::mutex m_resMtx;
    struct TRespondInf
    {
        time_t tm = time(nullptr);
        IKCSrvDbRespond& res;
        TRespondInf(IKCSrvDbRespond& r) : res(r) {}
        TRespondInf(const TRespondInf& c) : tm(c.tm), res(c.res) {}
    };
    map<string, TRespondInf> m_resList;
    // 配置
    string m_MsgName = "kc_db_message_queue_v10";
    unsigned m_MsgSize = 100;
    string m_MemName = "kc_db_shared_memory_v10";
    bool m_netEnable = false;
    string m_srvNetIP = "127.0.0.1";
    int m_srvNetPort = 20179;
};
