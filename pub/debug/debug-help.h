#include "bits/stdc++.h"
#include <unistd.h>
using namespace std;

std::string gen_tmp_file()
{

    std::stringstream ss;

    if ((rand() % 4) == 0)
    {
        return "";
    }
    else
    {
        int len = rand() % 1024;
        while (len--)
        {
            ss << (char)(rand() % 256);
        }
    }
    return ss.str();
}
static uint64_t cnt = 0;
std::vector<std::string> gen_tmp_files(int n)
{
    srand(time(0) + 1349880437 + cnt++);
    vector<string> ret;
    while (n--)
    {
        auto s = gen_tmp_file();
        ret.push_back(s);
    }
    return ret;
}
