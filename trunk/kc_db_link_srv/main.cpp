#include "framework_ex/bundle_context_helper.h"
using namespace KC;

int main(int argc, char *argv[])
{
    filesystem::path mainDir(argv[0]);
    BundleContextHelper contHelp(mainDir.branch_path().string().c_str());
    if (contHelp.isSuccess())
    {
        IBundleContext& cont = contHelp.getContext();
    }
    else cout << "Load framework fail." << endl;
    return 0;
}
