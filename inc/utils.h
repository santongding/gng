#pragma once
#include <vector>
#include <type.h>
#include <debug/log.h>
using std::string;
using std::vector;

#include <iostream>
#include <fstream>
void write_binary(const string &path, const bytes_t &data)
{
    std::ofstream ofs(path, std::ios::binary | std::ios::out);
    if (!ofs)
    {
        panic("fail to open file:%s", path.c_str());
    }
    ofs.write(data.c_str(), data.size());
}
std::string read_binary(const string &path)
{
    std::ifstream ifs(path, std::ios::binary | std::ios::in);
    if (!ifs)
    {
        panic("fail to open file:%s", path.c_str());
    }
    return std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
}

bytes_hash_t mod0 = 1349880437;
bytes_hash_t mod1 = 1e9 + 7;
bytes_hash_t base = 19260817;
bytes_hash_t get_hash(const std::string &data)
{
    bytes_hash_t ans0 = mod1, ans1 = mod0;
    for (auto c : data)
    {
        ans0 = (ans0 * base + c) % mod0;
        ans1 = (ans1 * base + c) % mod1;
    }
    return (ans0 << 32) | ans1;
}

void push_back_n(vector<char> &data, char *p, int n)
{
    for (char *i = p; i != p + n; i++)
    {
        data.push_back(*i);
    }
}

template <class T>
void push_back(vector<char> &data, T v)
{
    push_back_n(data, (char *)&v, sizeof(T));
}
template <class T>
static void read_head(T &dst, const vector<char> &data, int &l, int r)
{
    if (l + sizeof(T) > r)
    {
        panic("l:%d, r:%d, siz:%ld type name:%s\n", l, r, sizeof(T), typeid(T).name());
    }
    else
    {
        debug_verbose("l:%d, r:%d, siz:%ld type name:%s", l, r, sizeof(T), typeid(T).name());
        dst = *(T *)&data[l];
        for (int i = l; i < l + sizeof(T); i++)
        {
            debug_verbose("i:%d v:%d", i, data[i]);
        }
        l += sizeof(T);
        return;
    }
}

vector<char> read_head_n(int n, const vector<char> &data, int &l, int r)
{
    assert(n + l <= r);
    l += n;
    return vector<char>(data.begin() + l - n, data.begin() + l);
}