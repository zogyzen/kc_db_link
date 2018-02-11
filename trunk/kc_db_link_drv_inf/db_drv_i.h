#pragma once

#include "kc_json_i.h"

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
        // Json
        virtual IJsonManager& GetJsonInf(const char*) = 0;
        virtual void freeJsonInf(void) = 0;
        // 启动和结束
        virtual bool start(void) = 0;
        virtual void end(void) = 0;
        // 请求
        virtual bool request(const char*, int len, IKCSrvDbRespond& res) = 0;
        // 停止服务器
        virtual void stopSrv(void) = 0;
        // 得到上次的错误
        virtual const char* getLastError(void) = 0;
    };
}
