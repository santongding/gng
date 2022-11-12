#pragma once

// file system operations
#include "type.h"
#include <string>
#include <vector>
#include "debug/log.h"
#include <filesystem>
#include "regex"
#include <fstream>
using std::string;
inline void write_binary(const string &path, const bytes_t &data)
{
    std::ofstream ofs(path, std::ios::binary | std::ios::out);
    if (!ofs)
    {
        panic("fail to open file:%s", path.c_str());
    }
    ofs.write(data.c_str(), data.size());
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

inline void normalize_path(file_path_t &path, const file_path_t &base)
{
    auto relative = (std::string)std::filesystem::relative(path, base);
    if (relative.size() == 1 || relative[0] != '.' && relative[1] != '.')
    {
        path = relative;
    }
    else
    {
        panic("path:%s is out of base:%s", path.c_str(), base.c_str());
    }
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
            ret.push_back(p);
        }
    }
    return ret;
}