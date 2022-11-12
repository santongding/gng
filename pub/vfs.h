#pragma once
#include "type.h"
#include "unordered_map"
#include "debug/log.h"
#include "fs-utils.h"
#include "utils.h"
#include "bytes-utils.h"

class vfs
{
protected:
    virtual void write_impl(file_path_t path, const bytes_t &data) const = 0;
    virtual bytes_t read_impl(file_path_t path) const = 0;

private:
    bool enable_compress;
    bool enable_encrypt;
    const bytes_t key;

public:
    void store_committed(file_path_t path, const bytes_t &data) const
    {
        EQ(is_path_under_base(path, _root_dir), true);
        bytes_t stored = data;
        if (enable_compress)
        {
            stored = compress_data(stored);
        }
        if (enable_encrypt)
        {
            stored = encrypt(stored, key);
        }
        write_binary(path, stored);
        debug_verbose("commit store to:%s, stored bits:%lu", path.c_str(), stored.size());
    }
    bytes_t load_committed(file_path_t path) const
    {
        auto ret = read_binary(path);
        debug_verbose("load commit:%s, load bits:%lu", path.c_str(), ret.size());
        EQ(is_path_under_base(path, _root_dir), true);

        if (enable_encrypt)
        {
            ret = decrypt(ret, key);
        }
        if (enable_compress)
        {
            ret = decompress_data(ret);
        }
        debug_verbose("decompressed bits:%lu", ret.size());
        return ret;
    }
    vfs(const file_path_t &root_dir, const ignore_pattern_t &ignores, data_store_op store_option, const bytes_t &k) : _root_dir(root_dir), key(k)
    {
        enable_compress = store_option & COMPRESSED;
        enable_encrypt = store_option & ENCRYPTED;
        auto paths = get_files_relative_path_under_dir(root_dir, ignores);
        for (auto x : paths)
        {
            _handle2path_map[path2handle(x)] = x;
            debug_verbose("path:%s hash:%lu", x.c_str(), path2handle(x));
        }
    }
    void insert_path(const file_path_t &path)
    {
        _handle2path_map[path2handle(path)] = path;
    }

    std::vector<file_handle_t> get_all_file_handles() const
    {
        std::vector<file_handle_t> ret;
        ret.reserve(_handle2path_map.size());
        for (auto &x : _handle2path_map)
        {
            ret.push_back(x.first);
        }
        EQ(ret.size(), _handle2path_map.size());
        return ret;
    }
    void write(const file_handle_t handle, const bytes_t &data) const
    {
        write_impl(handle2path(handle), data);
    }
    bytes_t read(const file_handle_t handle) const
    {
        return read_impl(handle2path(handle));
    }
    virtual ~vfs() = default;
    std::unordered_map<file_handle_t, file_path_t> _handle2path_map;

    const file_handle_t path2handle(const file_path_t &path) const
    {

        EQ(is_path_under_base(path, _root_dir), true);
        auto ret = get_hash(path);
        return ret;
    }

    const file_path_t &handle2path(file_handle_t handle) const
    {
        NE(_handle2path_map.find(handle), _handle2path_map.end());
        return _handle2path_map.find(handle)->second;
    }

private:
    const file_path_t _root_dir;
};
