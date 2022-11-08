#pragma once
#include <vector>
using std::vector;
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
template<class T>
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