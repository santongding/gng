#pragma once
#include <stdint.h>
#include <string>

typedef uint64_t commit_handle;
typedef uint64_t file_handle;
typedef uint64_t line_index;
typedef uint64_t bytes_hash;
typedef std::string bytes;
typedef struct {const std::string path;const file_handle handle;} file_desc;

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
    commit_handle commit;
    file_handle file;
};
