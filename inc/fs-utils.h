#pragma once

// file system operations
#include "type.h"
#include <string>
#include <vector>
#include "debug/log.h"
#include <filesystem>
#include "regex"
bool file_or_dir_exists(std::string path)
{
    return std::filesystem::exists(path);
}

void mk_dir(std::string path){
    std::filesystem::create_directory(path);
}

file_path_t get_cwd()
{
    return std::filesystem::current_path();
}
std::regex gen_regex(std::string arg)
{
    for (auto i = arg.find('*'); i != std::string::npos; i = arg.find('*', i + 2))
    {
        arg.insert(i, 1, '.');
    }

    return std::regex(arg);
}

void normalize_path(file_path_t &path, const file_path_t &base)
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

bool is_path_ignored(const file_path_t &path, const ignore_pattern_t &ignore)
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

std::vector<file_path_t> get_files_relative_path_under_dir(const file_path_t &path, const ignore_pattern_t &ignore)
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