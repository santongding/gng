#pragma once
#include "type.h"
#include "utils.h"
#include "queue"
#include "compress.pb.h"

struct compress_node
{
    uint16_t ls_index = 0;
    uint16_t rs_index = 0;
};

inline bytes_t compress_data(const bytes_t &data)
{
    std::vector<compress_node> nodes(256);
    EQ(nodes.size(), 256);
    uint16_t allocated_cnt = 1;
    std::map<uint8_t, int> cnt;
    for (int i = 0; i < 256; i++)
        cnt[i] = 0;
    for (auto c : data)
    {
        cnt[c]--;
    }
    debug_verbose("finish cnt");
    std::priority_queue<std::pair<int, uint16_t>> q;
    for (auto it : cnt)
    {
        q.push({it.second, it.first + 256});
        assert(it.first < 256);
    }

    while (q.size() > 1)
    {
        assert(allocated_cnt < 256);
        auto a = q.top();
        q.pop();
        auto b = q.top();
        q.pop();
        auto c = std::pair<int, uint16_t>{a.first + b.first, allocated_cnt++};
        nodes[c.second] = {a.second, b.second};

        q.push(c);
    }

    debug_verbose("finish build");
    assert(allocated_cnt == 256);

    std::function<void(uint16_t, std::string)> dfs;
    std::map<char, std::string> value_map;
    dfs = [&](uint16_t i, std::string s)
    {
        if (i >= 256)
        {
            assert(i < 512);
            value_map[i - 256] = s;
            // debug_verbose("char %d value:%s", i - 256, s.c_str());
        }
        else
        {
            // debug_verbose("p:%u l:%u, r:%u", i, nodes[i].ls_index, nodes[i].rs_index);
            assert(i);
            dfs(nodes[i].ls_index, s + "0");
            dfs(nodes[i].rs_index, s + "1");
        }
    };
    dfs(allocated_cnt - 1, "");
    debug_verbose("finish dfs");
    bytes_t ret;
    assert(value_map.size() == 256);
    uint64_t bit_cnt = 0;
    for (auto c : data)
    {
        for (auto b : value_map[c])
        {
            if (bit_cnt / 8 >= ret.size())
            {
                ret.push_back(0);
            }
            if (b == '1')
                ret[bit_cnt / 8] |= 1 << (bit_cnt % 8);
            bit_cnt++;
        }
    }
    debug_verbose("finish compress");
    compress::compress cdata;
    cdata.set_content(ret);
    cdata.set_content_bit_count(bit_cnt);
    cdata.set_tree_pointers(&nodes[0], sizeof(compress_node) * nodes.size());
    cdata.CheckInitialized();

    return cdata.SerializeAsString();
}
inline bytes_t decompress_data(const bytes_t &data)
{
    compress::compress cdata;
    cdata.ParseFromString(data);
    assert(cdata.tree_pointers().size() == 256 * 4);
    compress_node *nodes = (compress_node *)&cdata.tree_pointers()[0];

    bytes_t ret;
    const uint16_t root = 255;
    uint16_t now = root;
    for (uint64_t i = 0; i < cdata.content_bit_count(); i++)
    {
        // debug_verbose("now:%d",now);
        bool v = cdata.content()[i / 8] & (1 << (i % 8));
        if (v)
        {
            now = nodes[now].rs_index;
        }
        else
        {
            now = nodes[now].ls_index;
        }
        // debug_verbose("now:%d",now);
        assert(now > 0 && now < 512);
        if (now >= 256)
        {
            ret.push_back((char)(now - 256));
            now = root;
        }
    }
    assert(now == root);
    return ret;
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