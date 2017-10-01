#pragma once

#include "stdn.h"

namespace KC
{
    class CKCSrvNet : public IKCSrvNet, public IRespondBackCall
    {
    public:
        CKCSrvNet(const IBundle& bundle);
        virtual CALL_TYPE ~CKCSrvNet();

        // 得到服务特征码
        const char* CALL_TYPE getGUID(void) const override;
        // 对应的模块
        const IBundle& CALL_TYPE getBundle(void) const override;

        // 启动主程序
        void run(void) override;

        // 响应结果
        void respond(const char*, int) override;

    private:
        // 插件及上下文
        IBundleContext& m_context;
        const IBundle& m_bundle;
        // 其他参数
        atomic_bool m_exit;
        unsigned m_port = 20179;
    };
}
