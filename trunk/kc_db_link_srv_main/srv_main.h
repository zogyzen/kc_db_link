#pragma once

#include "stdm.h"

namespace KC
{
    class CKCSrvMain : public IKCSrvMain
    {
    public:
        CKCSrvMain(const IBundle& bundle);
        virtual CALL_TYPE ~CKCSrvMain();

        // 得到服务特征码
        const char* CALL_TYPE getGUID(void) const override;
        // 对应的模块
        const IBundle& CALL_TYPE getBundle(void) const override;

        // 启动主程序
        void run(void) override;

    protected:
        // 处理请求
        void dealRequest(string sReq);

    private:
        // 插件及上下文
        IBundleContext& m_context;
        const IBundle& m_bundle;
        // 其他参数
        atomic_bool m_exit;
        const char* m_MsgInName = nullptr;
        const char* m_MsgOutName = nullptr;
        unsigned m_MsgSize = 100;
        const char* m_MemName = nullptr;
        unsigned m_MemSize = 1024 * 1024 * 8;
    };
}
