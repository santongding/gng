#include "type.h"
struct config_t{
    const commit_handle_t commit;
    const bool enable_compress;
    const bool enable_encrypt;
    const bool verbose;
};

config_t init(bool not_compress, bool not_encrypt, bool verbose);

config_t init_from_local();