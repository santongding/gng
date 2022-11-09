#include "data-struct.h"
#include "debug/log.h"
#include "assert.h"
#include "utils.h"
using std::vector;
void file_single::to_file(FILE *fd)
{
    to_file_content(fd);
}
bool file_single::is_same_with_committed()
{
    if (true)
    {
        return is_same_content_with_committed();
    }
    else
    {
    }
    return false;
}

file_single *file_single::from_file(FILE *fd, loc_enclave loc)
{
    static char buf[256];
    vector<char> data;
    int n;
    file_type type = TEXT;
    while ((n = fread(buf, sizeof(char), 256, fd)) > 0)
    {
        verbose("read %d bits", n);
        if (type == TEXT)
            for (int i = 0; i < n; i++)
            {
                if (!(buf[i] >= ASCII_BG && buf[i] < ASCII_ED))
                {
                    type = BINARY;
                }
            }
        push_back_n(data, buf, n);
    }

    auto hash = get_hash(data);
    verbose("type:%d", type);
    switch (type)
    {
    case BINARY:

        return dynamic_cast<file_single *>(new file_binary(data, loc));
        break;
    case TEXT:
        panic("wrong file type:%d\n", type);
    default:
        panic("wrong file type:%d\n", type);
        break;
    }
}
vector<char> file_single::to_bytes()
{
    vector<char> ret_data = to_bytes_content();
    verbose("siz:%ld\n", ret_data.size());
    ret_data.push_back(type_identity());
    return ret_data;
}

file_single *file_single::from_bytes(std::vector<char> data, loc_enclave loc)
{

    file_type type = (file_type)data[data.size() - 1];
    data.pop_back();

    verbose("type:%d", type);
    switch (type)
    {
    case BINARY:
        return dynamic_cast<file_single *>(new file_binary(data, loc));
        break;
    case TEXT:
        return dynamic_cast<file_single *>(new file_text(data, loc));
        break;
    default:
        panic("wrong file type:%d\n", type);
        break;
    }
}

file_binary::file_binary(const std::vector<char> &data, loc_enclave loc) : file_binary(loc)
{
    _data = data;
}

vector<char> file_binary::to_bytes_content()
{
    return _data;
}

static segment decode_from_bytes(const vector<char> &data, int &l, int r)
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

file_text::file_text(const vector<char> &data, loc_enclave loc) : file_text(loc)
{

    int n = data.size();
    int bg = 0;

    while (bg < n)
    {
        segments.push_back(decode_from_bytes(data, bg, n));
    }
}

vector<char> file_text::to_bytes_content()
{
    vector<char> ret;
    verbose("seg num:%ld", segments.size());
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
                push_back_n(ret, &s[0], s.length());
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
void file_binary::to_file_content(FILE *fd)
{
    verbose("start to file content, parent commit:%llu", _loc.parent.commit);

    fwrite(&_data[0], sizeof(char), _data.size(), fd);
}
void file_text::to_file_content(FILE *fd)
{
}

bool file_binary::is_same_content_with_committed()
{


    if (has_parent())
    {
        auto p = dynamic_cast<typeof(this)>(get_parent());

        if (get_hash(_data) == get_hash(p->_data))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool file_text::is_same_content_with_committed()
{
    return false;
}