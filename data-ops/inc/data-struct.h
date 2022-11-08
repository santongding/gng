#include <type.h>
#include "data-ops.h"
#pragma once

#define MAX_LINES_IN_SEG (1 << 7)
class data_obj
{
};

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
    file_binary(const std::vector<char> &data);
    std::vector<char> data;

private:
    std::vector<char> to_bytes_content() override;
    void to_file_content(FILE *fd, file_location parent, const get_file_by_loc_handler_t &handler) override;
    file_type type_identity() override
    {
        return BINARY;
    }
    std::vector<char> _data;
};
class file_text : public file_single
{
public:
    file_text(const std::vector<char> &data);

private:
    std::vector<char> to_bytes_content() override;
    void to_file_content(FILE *fd, file_location parent, const get_file_by_loc_handler_t &handler) override;
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
