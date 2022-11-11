#pragma once

#include "type.h"
class commit_helper;
typedef std::function<commit_helper *(commit_handle_t)> get_commit_by_loc_handler_t;
class commit_helper
{
public:
    virtual ~commit_helper();
    commit_helper(commit_handle_t parent, const get_commit_by_loc_handler_t *handler);
    commit_helper(bytes_t data, const get_commit_by_loc_handler_t *handler);
    void commit_file(const file_desc& fd);
    bytes_t commit();

private:
    class commit_helper_impl;
    std::unique_ptr<commit_helper_impl> _impl;
};
