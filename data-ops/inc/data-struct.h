#include <type.h>
#include "data-ops.h"
#pragma once

#define MAX_LINES_IN_SEG (1 << 7)

struct segment
{
    bool is_link;
    int line_num;
    line_index link_line;
    std::vector<std::string> texts;
};

class file_binary : public file_single
{
public:
    file_binary(const bytes &data, loc_enclave loc);
    bytes data;

private:
    file_binary(loc_enclave loc) : file_single(loc){};
    bool is_same_content_with_committed()override;
    bytes to_bytes_content()override;
    void to_file_content(FILE *fd)override;
    file_type type_identity() override
    {
        return BINARY;
    }
    bytes _data;
};
class file_text : public file_single
{
public:
    file_text(const bytes &data, loc_enclave loc);

private:
    file_text(loc_enclave loc) : file_single(loc){};
    bool is_same_content_with_committed()override;
    bytes to_bytes_content()override;
    void to_file_content(FILE *fd) override;
    file_type type_identity() override
    {
        return TEXT;
    }
    std::vector<segment> segments;
};
struct data_backup
{
    std::vector<file_single *> files;
};
