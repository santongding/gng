#include "config.pb.h"
#include "config.h"
#include "utils.h"
config_t init_from_local()
{
    auto config_bytes = read_binary(".gng/config");
    config::config local;
    local.ParseFromString(config_bytes);
    local.CheckInitialized();
    return {local.commit_handle(), local.enable_compress(), local.enable_encrypt(), local.verbose()};
}

config_t init(bool not_compress, bool not_encrypt, bool verbose)
{
    config_t ret{0, !not_compress, !not_encrypt, verbose};
    config::config local;
    local.set_commit_handle(0);
    local.set_enable_compress(!not_compress);
    local.set_enable_encrypt(!not_encrypt);
    local.set_verbose(verbose);
    write_binary(".gng/config", local.SerializeAsString());
    return ret;
}