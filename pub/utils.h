#pragma once
#include <vector>
#include <type.h>
using std::string;
using std::vector;




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
