#pragma once
#include <vector>
using std::vector;
bytes_hash mod0 = 1349880437;
bytes_hash mod1 = 1e9 + 7;
bytes_hash base = 19260817;
bytes_hash get_hash(vector<char> &data)
{
    bytes_hash ans0 = 0, ans1 = 0;
    ;
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
        verbose("l:%d, r:%d, siz:%ld type name:%s", l, r, sizeof(T), typeid(T).name());
        dst = *(T *)&data[l];
        for (int i = l; i < l + sizeof(T); i++)
        {
            verbose("i:%d v:%d", i, data[i]);
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