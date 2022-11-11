#pragma once

#include "type.h"
class commit_helper;
typedef std::function<commit_helper *(commit_handle)> get_commit_by_loc_handler_t;
class commit_helper
{
public:
    virtual ~commit_helper();
    commit_helper(commit_handle parent, const get_commit_by_loc_handler_t *handler);
    commit_helper(bytes data, const get_commit_by_loc_handler_t *handler);
    void commit_file(const file_desc& fd);
    bytes commit();

private:
    class commit_helper_impl;
    std::unique_ptr<commit_helper_impl> _impl;
};
