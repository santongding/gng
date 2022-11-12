#pragma once

#include "commit-ops.h"
enum command_op
{
    List,
    Commit,
    Checkout, 
};

class commit_mgr
{
public:
    virtual ~commit_mgr();
    commit_mgr(const file_path_t &path_prefix, command_op op);

    void do_commit_file(const file_desc & fd); // commit one file, but not store
    void finish_commit(); // store all files to path_prefix<newest_commit>
    
    std::vector<file_handle_t> get_all_files_handle(); // return all files' handle in the checkout commit
    void do_check_out(file_desc & fd); // check out the file, write content to fd
    bool is_all_files_checkout(); // return if all files are checkout

    void do_list(commit_handle_t commit);
private:
    class commit_mgr_impl;
    std::unique_ptr<commit_mgr_impl> _impl;
};
