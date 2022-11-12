#include "type.h"
#include "unordered_map"
#include "debug/log.h"
#include "fs-utils.h"
#include "utils.h"

class vfs
{
protected:
    virtual void write_impl(file_path_t path) = 0;
    virtual bytes_t read_impl(file_path_t path) = 0;

public:
    std::vector<file_handle_t> get_all_file_handles() const
    {
        std::vector<file_handle_t> ret;
        ret.reserve(_handle2path_map.size());
        for (auto &x : _handle2path_map)
        {
            ret.push_back(x);
        }
        EQ(ret.size(), _handle2path_map.size());
        return ret;
    }

    void write(const file_desc &file_desc)
    {
        write_impl(handle2path(file_desc));
    }
    bytes_t read(const file_desc &file_desc)
    {
        return read_impl(handle2path(file_desc.handle));
    }
    vfs(file_path_t root_dir, ignore_pattern_t ignores) : _root_dir(root_dir)
    {
        auto paths = get_files_relative_path_under_dir(root_dir, ignores);
        for (auto x : paths)
        {
            _handle2path_map[path2handle(x)] = x;
        }
    }
    ~vfs();
    std::unordered_map<file_handle_t, file_path_t> _handle2path_map;

private:
    const file_path_t _root_dir;
    const file_path_t &handle2path(file_handle_t handle) const
    {
        NE(_handle2path_map.count(handle), 0);
        return _handle2path_map[handle];
    }

    const file_handle_t path2handle(file_path_t path) const
    {

        normalize_path(path, _root_dir);
        auto ret = get_hash(path);
        return ret;
    }
};

vfs::~vfs()
{
}
