#pragma once

#include "framework_ex/service_ex_i.h"
#include "framework_ex/framework_exception.h"

namespace KC
{
    // 主程序接口
    class IKCSrvMain : public IServiceEx
    {
    public:
        // 启动主程序
        virtual void run(void) = 0;

    protected:
        virtual CALL_TYPE ~IKCSrvMain() = default;
    };

    constexpr const char c_KCSrvMainSrvGUID[] = "IKCSrvMain_B5E289BB-ADB8-92F8-CF31-BD411D2F52BE";

    class TKCSrvMainException : public TFWSrvException
    {
    public:
            TKCSrvMainException(int id, string place, string msg, string name)
                            : TFWSrvException(id, place, msg, name, c_KCSrvMainSrvGUID) {}
            TKCSrvMainException(int id, string place, string msg, IKCSrvMain& srv)
                            : TFWSrvException(id, place, msg, srv) {}
    };
}
