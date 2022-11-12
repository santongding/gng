#pragma once

#include "type.h"
#include "vfs.h"

class commit_helper;

using get_commit_by_loc_handler_t = std::function<commit_helper *(commit_handle_t)>;

class commit_helper
{
public:
    virtual ~commit_helper();
    commit_helper(commit_handle_t parent, const get_commit_by_loc_handler_t *handler, const vfs *vfs);
    commit_helper(bytes_t data, const get_commit_by_loc_handler_t *handler, const vfs *vfs);
    commit_handle_t get_parent();
    bytes_t commit_to_bytes();
    void checkout();

private:
    class commit_helper_impl;
    std::unique_ptr<commit_helper_impl> _impl;
};
