#pragma once

#include "stddb.h"

namespace KC
{
    class CKCSrvDb : public IKCSrvDb
    {
    public:
        CKCSrvDb(const IBundle& bundle);
        virtual CALL_TYPE ~CKCSrvDb();

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
    };
}
