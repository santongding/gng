#pragma once
#include "type.h"
#include "utils.h"

inline bytes_t compress(const bytes_t &data)
{
    return data;
}
inline bytes_t decompress(const bytes_t &data)
{
    return data;
}

inline bytes_t to_block_aligned(const bytes_t &data)
{
    bytes_t ret = data;
    int num = 8 - (data.size() % 8);
    for (int i = 0; i < num; i++)
    {
        ret.push_back(num);
    }
    return ret;
}
inline bytes_t to_block_unaligned(const bytes_t &data)
{
    assert((data.size() % 8) == 0);
    assert(data.size());
    return data.substr(0, data.size() - data[data.size() - 1]);
}

inline bytes_t encrypt(const bytes_t &data, const bytes_t &key)
{
    uint64_t v = get_hash(key);
    bytes_t ret = to_block_aligned(data);
    size_t block_siz = ret.size() / 8;
    uint64_t *bg = (uint64_t *)&ret[0];
    srandom(v);
    for (int i = 0; i < block_siz; i++)
    {
        bg[i] ^= v ^ random();
    }
    return ret;
}
inline bytes_t decrypt(const bytes_t &data, const bytes_t &key)
{
    uint64_t v = get_hash(key);
    bytes_t ret = data;
    size_t block_siz = ret.size() / 8;
    uint64_t *bg = (uint64_t *)&ret[0];
    srandom(v);
    for (int i = 0; i < block_siz; i++)
    {
        bg[i] ^= v ^ random();
    }
    return to_block_unaligned(ret);
}