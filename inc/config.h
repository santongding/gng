#pragma once
#include "type.h"
struct config_t
{
    commit_handle_t commit;
    data_store_op store_op;
    const bool enable_meta;
    const bool verbose;
    ignore_pattern_t ignores;
    bytes_t key;
};

bool init(bool not_compress, bool not_encrypt, bool not_meta, bool verbose, bytes_t);

config_t init_from_local(bytes_t key);

void write_back_config(const config_t &config);