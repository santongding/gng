#include "bits/stdc++.h"

using namespace std;

std::string gen_tmp_file()
{
    srand(time(0));
    std::stringstream ss;
    ss << "/tmp/gng-tmp-";
    ss << random();
    auto name = ss.str();
    std::ofstream stream(name, ios::out | ios::binary);
    if ((random() % 4) == 0)
    {
    }
    else
    {
        int len = random() % 1024;
        while (len--)
        {
            stream << (char)(rand() % 256);
        }
    }
    return name;
}
std::vector<std::string> gen_tmp_files(int n)
{
    vector<string> ret;
    while (n--)
    {
        auto s = gen_tmp_file();
        assert(s.size() > 0);
        ret.push_back(s);
    }
    return ret;
}
