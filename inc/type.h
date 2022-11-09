#pragma once
#include <stdint.h>

typedef uint64_t commit_handle;
typedef uint64_t file_handle;
typedef uint64_t line_index;
typedef uint64_t bytes_hash;

#define ASCII_BG 1
#define ASCII_ED 127


enum file_type
{
    BINARY = 1,
    TEXT = 2
};

struct file_location
{
    commit_handle commit;
    file_handle file;
};
