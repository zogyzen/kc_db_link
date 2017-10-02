#pragma once

namespace KC
{
    // 数据库接口驱动
    class IKCSrvDbDrv
    {
    public:
        // 启动
        virtual void run(void) = 0;

    protected:
        virtual CALL_TYPE ~IKCSrvDb() = default;
    };
}
