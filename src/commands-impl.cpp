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

class meta_vfs : public vfs
{
public:
    meta_vfs(const file_path_t &root_dir, const ignore_pattern_t &ignores, data_store_op op, const bytes_t &k) : vfs(root_dir, ignores, op, k){};

protected:
    void write_impl(file_path_t path, const bytes_t &data) const override
    {
        meta::meta meta;
        meta.ParseFromString(data);
        meta.CheckInitialized();
        write_meta(path, meta);
    }
    bytes_t read_impl(file_path_t path) const override
    {
        auto meta = read_meta(path);
        return meta.SerializeAsString();
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
    data_vfs d(root, config.ignores, config.store_op, config.key);

    auto mgr = commit_mgr(data_prefix, &d);
    auto new_commit = mgr.do_commit(config.commit);

    if (config.enable_meta)
    {
        meta_vfs m(root, config.ignores, config.store_op, config.key);
        auto m_mgr = commit_mgr(meta_prefix, &m);
        EQ(new_commit, m_mgr.do_commit(config.commit));
    }
    config.commit = new_commit;
}

void checkout_impl(config_t &config, commit_handle_t commit, checkout_op op)
{
    file_path_t root = ".";
    data_vfs d(root, config.ignores, config.store_op, config.key);

    if (op == HARD)
    {
        for (auto f : d.get_all_file_handles())
        {
            del_file(d.handle2path(f));
        }
    }

    if (commit)
    {

        auto mgr = commit_mgr(data_prefix, &d);
        mgr.do_checkout(commit, op);

        if (config.enable_meta)
        {
            meta_vfs m(root, config.ignores, config.store_op, config.key);
            auto m_mgr = commit_mgr(meta_prefix, &m);
            m_mgr.do_checkout(commit, op);
        }
    }
    config.commit = commit;
}
