#include "commands-impl.h"
#include "commit-mgr.h"
#include "iostream"
const file_path_t meta_prefix = ".gng/data-meta-";
const file_path_t data_prefix = ".gng/data-";

void list_impl(config_t config, commit_handle_t handle)
{
    std::cout.clear(); // temp open stdout
    auto mgr = commit_mgr(data_prefix, List);
    mgr.do_list(handle);
    std::cout.setstate(std::ios_base::badbit);
}

void commit_impl(config_t config)
{

    if (!config.enable_meta)
    {
        return;
    }
}

void checkout_impl(config_t config, commit_handle_t commit)
{
    if (!config.enable_meta)
    {
        return;
    }
}