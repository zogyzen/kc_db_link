#pragma once

#include "stdw.h"

namespace KC
{
    class CKCSrvWork : public IKCSrvWork
    {
    public:
        CKCSrvWork(const IBundle& bundle);
        virtual CALL_TYPE ~CKCSrvWork();

        // 得到服务特征码
        const char* CALL_TYPE getGUID(void) const override;
        // 对应的模块
        const IBundle& CALL_TYPE getBundle(void) const override;

        // 处理请求
        void request(const char*, IRespondBackCall&) override;

    protected:
        // 处理请求
        void dealRequest(string sReq);

    private:
        // 插件及上下文
        IBundleContext& m_context;
        const IBundle& m_bundle;
    };
}
