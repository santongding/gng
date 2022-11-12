#pragma once

// file system operations
#include "type.h"
#include <string>
#include <vector>
#include "debug/log.h"
#include <filesystem>
#include "regex"
#include <fstream>
#include "meta.pb.h"
using std::string;

using ft = std::filesystem::file_time_type;
template <typename TP>
inline std::time_t to_time_t(TP tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    return system_clock::to_time_t(sctp);
}

inline void write_meta(const string &path, const meta::meta &meta)
{

    // auto t = std::chrono::system_clock::from_time_t(meta.last_write_timestamp());
    // std::filesystem::last_write_time(path, ft(t));
    auto s = std::filesystem::status(path);
    s.permissions((std::filesystem::perms)meta.permission());
    s.type((std::filesystem::file_type)meta.file_type());
}

inline meta::meta read_meta(const string &path)
{
    auto s = std::filesystem::status(path);
    meta::meta ret;
    ret.set_permission((uint64_t)s.permissions());
    ret.set_file_type((uint64_t)s.type());
    // auto ftime = std::filesystem::last_write_time(path);
    // std::time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
    // ret.set_last_write_timestamp(to_time_t(ftime));
    ret.CheckInitialized();
    return ret;
}

inline void write_binary(const string &path, const bytes_t &data)
{
    std::ofstream ofs(path, std::ios::binary | std::ios::out);
    if (!ofs)
    {
        panic("fail to open file:%s", path.c_str());
    }
    ofs.write(data.c_str(), data.size());
    EQ(std::filesystem::exists(path), true);
}
inline std::string read_binary(const string &path)
{
    std::ifstream ifs(path, std::ios::binary | std::ios::in);
    if (!ifs)
    {
        panic("fail to open file:%s", path.c_str());
    }
    return std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
}

inline bool dir_exists(std::string path)
{
    return std::filesystem::is_directory(path);
}
inline bool file_exists(std::string path)
{
    return std::filesystem::is_regular_file(path);
}

inline void del_file(const string &path)
{
    EQ(file_exists(path), true);
    if (!std::filesystem::remove(path))
    {
        panic("fail to del file:%s", path.c_str());
    }
}
inline bool mk_dir(std::string path)
{
    return std::filesystem::create_directory(path);
}

inline file_path_t get_cwd()
{
    return std::filesystem::current_path();
}
inline std::regex gen_regex(std::string arg)
{
    for (auto i = arg.find('*'); i != std::string::npos; i = arg.find('*', i + 2))
    {
        arg.insert(i, 1, '.');
    }

    return std::regex(arg);
}
inline bool is_path_under_base(const file_path_t &path, const file_path_t &base)
{
    auto relative = (std::string)std::filesystem::relative(path, base);
    // debug("path:%s, base:%s relative:%s", path.c_str(), base.c_str(), relative.c_str());

    if (relative.size() == 2 && relative[0] == '.' && relative[1] == '.')
        return false;
    if (relative.size() < 3 || !(relative[0] == '.' && relative[1] == '.' && relative[2] == '/'))
    {
        return true;
    }
    else
    {
        return false;
    }
}

inline void normalize_path(file_path_t &path, const file_path_t &base)
{
    EQ(is_path_under_base(path, base), true);
    path = (std::string)std::filesystem::relative(path, base);
}

inline bool is_path_ignored(const file_path_t &path, const ignore_pattern_t &ignore)
{
    for (auto &re : ignore)
    {
        if (std::regex_match(path, re))
        {
            return true;
        }
    }
    return false;
}

inline std::vector<file_path_t> get_files_relative_path_under_dir(const file_path_t &path, const ignore_pattern_t &ignore)
{
    std::vector<file_path_t> ret;
    for (std::filesystem::recursive_directory_iterator i(path), end; i != end; ++i)
    {
        if (!std::filesystem::is_directory(i->path()))
        {
            file_path_t p = i->path();
            normalize_path(p, path);
            if (is_path_ignored(p, ignore))
                continue;
            // debug_verbose("file found:%s", i->path().c_str());
            ret.push_back(p);
        }
    }
    return ret;
}