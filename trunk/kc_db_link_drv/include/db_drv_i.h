#pragma once

namespace KC
{
    // 数据库接口驱动应答
    class IKCSrvDbRespond
    {
    public:
        // 应答
        virtual void respond(const char*, int len) = 0;
    };

    // 数据库接口驱动
    class IKCSrvDbDrv
    {
    public:
        // 启动和结束
        virtual void start(IKCSrvDbRespond& res) = 0;
        virtual void end(void) = 0;
        // 请求
        virtual void request(const char*, int len) = 0;
    };
}
