#include "srv_main.h"

// 消息队列
const char* c_message_queue_in_name = "kc_db_message_queue_in_v10";
const char* c_message_queue_out_name = "kc_db_message_queue_out_v10";
const unsigned c_message_queue_size = 100;
// 共享内存
const char* c_shared_memory_name = "kc_db_shared_memory_v10";
const unsigned c_shared_memory_size = 1024 * 1024 * 8;

////////////////////////////////////////////////////////////////////////////////
// CKCSrvMain类
CKCSrvMain::CKCSrvMain(const IBundle& bundle)
    : m_context(bundle.getContext()), m_bundle(bundle)
{
}

CKCSrvMain::~CKCSrvMain()
{
}

// 得到服务特征码
const char* CKCSrvMain::getGUID(void) const
{
    return c_KCSrvMainSrvGUID;
}

// 对应的模块
const IBundle& CKCSrvMain::getBundle(void) const
{
    return m_bundle;
}

// 启动主程序
void CKCSrvMain::run(void)
{
    try
    {
       // 创建
       message_queue::remove(c_message_queue_in_name);
       message_queue::remove(c_message_queue_out_name);
       shared_memory_object::remove(c_shared_memory_name);
       message_queue mqIn(create_only, c_message_queue_in_name, c_message_queue_size, sizeof(int));
       message_queue mqOut(create_only, c_message_queue_out_name, c_message_queue_size, sizeof(int));
       managed_shared_memory mhm(create_only, c_shared_memory_name, c_shared_memory_size);
       // 启动消息队列
       for(bool bExit = false; !bExit; )
       {
           unsigned int priority;
           message_queue::size_type recvd_size;
           int number;
           mqIn.receive(&number, sizeof(number), recvd_size, priority);
       }
       // 退出
       message_queue::remove(c_message_queue_in_name);
       message_queue::remove(c_message_queue_out_name);
       shared_memory_object::remove(c_shared_memory_name);
    }
    catch(interprocess_exception &ex)
    {
    }
}

