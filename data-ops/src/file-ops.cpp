#include "data-struct.h"
#include "debug/log.h"
#include "assert.h"
#include "commit-ops.h"
using std::vector;
vector<char> file_single::to_bytes()
{
    vector<char> ret_data = encode_content();
    ret_data.push_back(type_identity());
    return ret_data;
}

file_single *file_single::from_bytes(const std::vector<char> &data)
{

    file_type type = (file_type)data[data.size() - 1];

    verbose("type:%d", type);
    switch (type)
    {
    case BINARY:
        return dynamic_cast<file_single *>(new file_binary(data));
        break;
    case TEXT:
        return dynamic_cast<file_single *>(new file_text(data));
    default:
        panic("wrong file type:%d\n", type);
        break;
    }
}

file_binary::file_binary(const vector<char> &data) : file_single()
{
    _data = data;
    _data.pop_back();
}

vector<char> file_binary::encode_content()
{
    return _data;
}
static void push_back(vector<char> &data, char *p, int n)
{
    for (char *i = p; i != p + n; i++)
    {
        data.push_back(*i);
    }
}

template <class T>
static void push_back(vector<char> &data, T v)
{
    push_back(data, (char *)&v, sizeof(T));
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

static vector<char> read_head_n(int n, const vector<char> &data, int &l, int r)
{
    assert(n + l <= r);
    l += n;
    return vector<char>(data.begin() + l - n, data.begin() + l);
}
segment decode_from_bytes(const vector<char> &data, int &l, int r)
{
    char hd;
    read_head(hd, data, l, r);
    segment ret;
    read_head(ret.line_num, data, l, r);
    verbose("is_link:%d line num:%d\n", (int)hd, ret.line_num);
    if (hd)
    {
        ret.is_link = true;
        read_head(ret.link_line, data, l, r);
    }
    else
    {
        ret.is_link = false;
        int num;
        for (int i = 0; i < ret.line_num; i++)
        {
            read_head<int>(num, data, l, r);
            vector<char> chars = read_head_n(num, data, l, r);
            chars.push_back(0);
            ret.texts.push_back(std::string(&chars[0]));
        }
    }
    verbose("decoded seg: is_link:%d, line_num:%d, l:%d, r:%d", ret.is_link, ret.line_num, l, r);
    return ret;
}

file_text::file_text(const vector<char> &data) : file_single()
{

    int n = data.size() - 1;
    int bg = 0;

    while (bg < n)
    {
        segments.push_back(decode_from_bytes(data, bg, n));
    }
}

vector<char> file_text::encode_content()
{
    vector<char> ret;
    verbose("seg num:%ld",segments.size());
    for (auto &seg : segments)
    {
        ret.push_back(seg.is_link ? 1 : 0);
        push_back(ret, seg.line_num);

        if (!seg.is_link)
        {
            assert(seg.line_num == seg.texts.size());
            for (auto &s : seg.texts)
            {
                push_back(ret, (int)s.length());
                push_back(ret, &s[0], s.length());
            }
        }
        else
        {
            push_back(ret, seg.link_line);
        }
    }
    for (int i = 0; i < ret.size(); i++)
    {
        verbose("encode i:%d v:%d", i, ret[i]);
    }
    return ret;
}