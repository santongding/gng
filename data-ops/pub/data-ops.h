#pragma once

#include <type.h>
class file_single_helper;
typedef std::function<file_single_helper *(file_location)> get_file_by_loc_handler_t;

class file_single_helper
{
public:
    virtual ~file_single_helper();

    file_single_helper(bytes_t data, file_handle_t file_handle, const get_file_by_loc_handler_t *handler);              // from bytes to structured object
    file_single_helper(const file_desc &fd, commit_handle_t commit_handle, const get_file_by_loc_handler_t *handler); // from file to structured object

    void to_file(file_desc &fd)const; // from structured object to binary file

    commit_handle_t get_parent_commit_handle() const; // needed to determine if is a link

    bytes_t to_bytes() const; // t from structured object to bytes

private:
    struct file_single_helper_impl;
    std::unique_ptr<file_single_helper_impl> _impl;
};
