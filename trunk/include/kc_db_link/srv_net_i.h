#pragma once

#include "framework_ex/service_ex_i.h"
#include "framework_ex/framework_exception.h"

namespace KC
{
    // ÍøÂç½Ó¿Ú
    class IKCSrvNet : public IServiceEx
    {
    public:
        // Æô¶¯
        virtual void run(void) = 0;

    protected:
        virtual CALL_TYPE ~IKCSrvNet() = default;
    };

    constexpr const char c_KCSrvNetSrvGUID[] = "IKCSrvNet_B5E289BB-ADB8-92F8-CF31-BD411D2F52BD";

    class TKCSrvNetException : public TFWSrvException
    {
    public:
            TKCSrvNetException(int id, string place, string msg, string name)
                            : TFWSrvException(id, place, msg, name, c_KCSrvNetSrvGUID) {}
            TKCSrvNetException(int id, string place, string msg, IKCSrvNet& srv)
                            : TFWSrvException(id, place, msg, srv) {}
    };
}
