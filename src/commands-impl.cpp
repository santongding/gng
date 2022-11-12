#include "commands-impl.h"
#include "commit-mgr.h"
#include "iostream"
#include "vfs.h"
const file_path_t meta_prefix = ".gng/data-meta-";
const file_path_t data_prefix = ".gng/data-";
class data_vfs : public vfs
{
public:
    data_vfs(const file_path_t &root_dir, const ignore_pattern_t &ignores, data_store_op op, const bytes_t &k) : vfs(root_dir, ignores, op, k){};

protected:
    void write_impl(file_path_t path, const bytes_t &data) const override
    {
        write_binary(path, data);
    }
    bytes_t read_impl(file_path_t path) const override
    {

        return read_binary(path);
    }
};
void list_impl(const config_t &config, commit_handle_t handle)
{
    std::cout.clear(); // temp open stdout

    file_path_t root = ".";
    data_vfs vfs(root, config.ignores, config.store_op, config.key);
    auto mgr = commit_mgr(data_prefix, &vfs);
    mgr.do_list(handle);
    std::cout.setstate(std::ios_base::badbit);
}

void commit_impl(config_t &config)
{
    file_path_t root = ".";
    data_vfs vfs(root, config.ignores, config.store_op, config.key);
    auto mgr = commit_mgr(data_prefix, &vfs);
    auto new_commit = mgr.do_commit(config.commit);
    config.commit = new_commit;
    if (!config.enable_meta)
    {
        return;
    }
}

void checkout_impl(config_t &config, commit_handle_t commit, checkout_op op)
{
    file_path_t root = ".";
    data_vfs vfs(root, config.ignores, config.store_op, config.key);

    auto mgr = commit_mgr(data_prefix, &vfs);
    config.commit = commit;
    if (op == HARD)
    {
        for (auto f : vfs.get_all_file_handles())
        {
            del_file(vfs.handle2path(f));
        }
    }
    if (commit)
        mgr.do_checkout(commit, op);

    if (!config.enable_meta)
    {
        return;
    }
}
