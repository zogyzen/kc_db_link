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
        // 初始化
        virtual void init(IKCSrvDbRespond& res) = 0;
        // 请求
        virtual void request(const char*, int len) = 0;
    };
}
