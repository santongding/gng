#include "config.pb.h"
#include "config.h"
#include "fs-utils.h"
#include "unistd.h"
#include "fcntl.h"

void log_success_init(const config_t &ret)
{

    if (!ret.verbose)
    {
        std::cout.setstate(std::ios_base::badbit);
    }
    std::cout << std::boolalpha;
    std::cout << "current branch: " << ret.commit << std::endl;
    std::cout << "not-compress: " << !ret.enable_compress << std::endl;
    std::cout << "not-encrypt: " << !ret.enable_encrypt << std::endl;
    std::cout << "not-store-meta: " << !ret.enable_meta << std::endl;
    std::cout << "init config success!" << std::endl;
}
config_t init_from_local()
{
    auto config_bytes = read_binary(".gng/config");
    config::config local;
    local.ParseFromString(config_bytes);
    local.CheckInitialized();
    config_t ret = {local.commit_handle(), local.enable_compress(), local.enable_encrypt(), local.enable_meta(), local.verbose()};
    log_success_init(ret);
    return ret;
}

bool init(bool not_compress, bool not_encrypt, bool not_meta, bool verbose)
{
    config_t ret{0, !not_compress, !not_encrypt, verbose};
    config::config local;
    local.set_commit_handle(0);
    local.set_enable_compress(!not_compress);
    local.set_enable_encrypt(!not_encrypt);
    local.set_enable_meta(!not_meta);
    local.set_verbose(verbose);
    write_binary(".gng/config", local.SerializeAsString());
    log_success_init(ret);
    return true;
}