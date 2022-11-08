#pragma once
#include <string>
#include <vector>
#include <type.h>

class file_single;
typedef std::function<file_single *(file_location)> get_file_by_loc_handler_t;

class file_single
{
public:
    std::vector<char> to_bytes();

    static file_single *from_bytes(const std::vector<char> &data);

    void to_file(FILE *fd, file_location parent, const get_file_by_loc_handler_t &handler);
    static file_single *from_file(FILE *fd, file_location parent, const get_file_by_loc_handler_t &handler);
    virtual ~file_single(){};

protected:
    file_single(){};
    virtual std::vector<char> to_bytes_content() = 0;
    virtual void to_file_content(FILE *fd, file_location parent, const get_file_by_loc_handler_t &handler) = 0;
    virtual file_type type_identity() = 0;
};
