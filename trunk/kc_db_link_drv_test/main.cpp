#include <string>
#include <iostream>
using namespace std;

#include <boost/dll.hpp>
using namespace boost;

#ifdef WIN32    // Windows环境
    const char* c_dll_extname = ".dll";
#else           // linux环境
    const char* c_dll_extname = ".so";
#endif

#include "db_drv_i.h"
using namespace KC;

// 测试程序
int main(int/* argc */, char *argv[])
{
    filesystem::path mainDir(argv[0]);
    string sDir = mainDir.branch_path().string();
    string sDllPath = sDir + "/kc_db_link_drv" + c_dll_extname;
    dll::shared_library m_lib;
    system::error_code ec;
    m_lib.load(sDllPath, ec);
    if (ec) cout << "Load db driver fail - " << ec.message() << " - " << sDllPath << endl;
    if (m_lib.is_loaded())
    {
        if (m_lib.has("InitDbDrv"))
        {
            auto _InitDbDrv = m_lib.get<IKCSrvDbDrv&(const char*)>("InitDbDrv");
            IKCSrvDbDrv& drv = _InitDbDrv(sDir.c_str());
            if (!drv.start()) cout << drv.getLastError() << endl;
            else
            {
                class CResp : public IKCSrvDbRespond
                {
                public:
                    // 应答
                    void respond(const char* res, int len) override
                    {
                        cout << "result - " << res << endl;
                    }
                };
                CResp res;
                string sReq;
                do
                {
                    cin >> sReq;
                    drv.request(sReq.c_str(), sReq.length(), res);
                } while ("exit" != sReq && "stop" != sReq);
                if ("stop" == sReq) drv.stopSrv();
                drv.end();
            }
        }
        else cout << "Load db driver fail, not exists function InitDbDrv" << endl;
        if (m_lib.has("UninitDbDrv"))
        {
            auto _UninitDbDrv = m_lib.get<void(void)>("UninitDbDrv");
            _UninitDbDrv();
        }
    }
    return 0;
}
