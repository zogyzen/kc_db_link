#include "framework_ex/bundle_context_helper.h"
#include "kc_db_link/srv_main_i.h"
using namespace KC;

// 主引导程序
int main(int/* argc */, char *argv[])
{
    filesystem::path mainDir(argv[0]);
    BundleContextHelper contHelp(mainDir.branch_path().string().c_str());
    if (contHelp.isSuccess())
    {
        IBundleContextEx& cont = contHelp.getContext();
        cont.getService<IKCSrvMain>(c_KCSrvMainSrvGUID).run();
    }
    else cout << "Load framework fail." << endl;
    return 0;
}
