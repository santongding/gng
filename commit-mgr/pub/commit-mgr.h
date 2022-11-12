#pragma once

#include "commit-ops.h"
enum command_op
{
    Checkout,
};

class commit_mgr
{
public:
    virtual ~commit_mgr();
    commit_mgr(const file_path_t &path_prefix, const vfs *vfs);

    commit_handle_t do_commit(commit_handle_t current); // store all files to path_prefix<newest_current>

    void do_checkout(commit_handle_t commit, checkout_op op);         

    void do_list(commit_handle_t commit);

private:
    class commit_mgr_impl;
    std::unique_ptr<commit_mgr_impl> _impl;
};
