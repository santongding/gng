#pragma once
#include <stdint.h>
#include <string>
#include <regex>

typedef uint64_t commit_handle_t;
typedef uint64_t file_handle_t;
typedef uint64_t bytes_hash_t;
typedef std::string bytes_t;

typedef std::vector<std::regex> ignore_pattern_t;
typedef std::string file_path_t;
typedef struct
{
    bytes_t data;
    const file_handle_t handle;
} file_desc;

#define NONE_HANDLE (-1)

#define ASCII_BG 1
#define ASCII_ED 127

enum file_type
{
    BINARY = 1,
    TEXT = 2
};

struct file_location
{
    commit_handle_t commit;
    file_handle_t file;
};


enum checkout_op{
    SOFT,
    HARD,
    MIXED,
};


enum data_store_op{
    COMPRESSED = 1,
    ENCRYPTED = 2,
};