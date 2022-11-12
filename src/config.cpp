#include "config.pb.h"
#include "config.h"
#include "fs-utils.h"
#include "unistd.h"
#include "bytes-utils.h"
#include "fcntl.h"

void log_success_init(const config_t &ret)
{

    std::cout << std::boolalpha;
    std::cout << "current branch: " << ret.commit << std::endl;
    std::cout << "not-compress: " << !(ret.store_op & COMPRESSED) << std::endl;
    std::cout << "not-encrypt: " << !(ret.store_op & ENCRYPTED) << std::endl;
    std::cout << "not-store-meta: " << !ret.enable_meta << std::endl;
    std::cout << "init config success!" << std::endl;
    if (!ret.verbose)
    {
        std::cout.setstate(std::ios_base::badbit);
    }
}
const char local_config_path[] = ".gng/config";
const static bytes_t magic_code = "snkdfnsakdfnjkasfkdfjnkadnsnakjandaj";

static config::config local_config;
void write_back_config(const config_t &config)
{
    local_config.set_commit_handle(config.commit);
    local_config.CheckInitialized();
    write_binary(local_config_path, local_config.SerializeAsString());
}
static void init_ignore(config_t &config)
{
    config.ignores.push_back(gen_regex(".gng/*"));
    if (file_exists(".gngignore"))
    {
        std::ifstream infile(".gngignore");
        std::string line;
        while (std::getline(infile, line))
        {
            config.ignores.push_back(gen_regex(line));
        }
    }
}

static data_store_op build_op(bool compress, bool encrypt)
{
    return (data_store_op)((compress) | (encrypt << 1));
}

config_t init_from_local(bytes_t key)
{
    auto config_bytes = read_binary(local_config_path);
    local_config.ParseFromString(config_bytes);
    local_config.CheckInitialized();

    config_t ret = {local_config.commit_handle(), build_op(local_config.enable_compress(), local_config.enable_encrypt()), local_config.enable_meta(), local_config.verbose()};
    if (ret.store_op & ENCRYPTED)
    {
        if (key == "")
        {
            std::cerr << "Key not provided!" << std::endl;
            exit(1);
        }
        EQ(local_config.has_magic_code(), true);
        if (decrypt(local_config.magic_code(), key) != magic_code)
        {
            std::cerr << "Key not right!" << std::endl;
            exit(1);
        }
    }
    ret.key = key;
    init_ignore(ret);
    log_success_init(ret);

    return ret;
}

bool init(bool not_compress, bool not_encrypt, bool not_meta, bool verbose, bytes_t key)
{
    const config_t &ret{0, build_op(!not_compress, !not_encrypt), verbose};
    config::config local;
    local.set_commit_handle(0);
    local.set_enable_compress(!not_compress);
    local.set_enable_encrypt(!not_encrypt);
    local.set_enable_meta(!not_meta);
    local.set_verbose(verbose);
    if (!not_encrypt)
    {
        if (key == "")
        {
            std::cerr << "Key not provided!" << std::endl;
            exit(1);
        }

        local.set_magic_code(encrypt(magic_code, key));
    }
    write_binary(".gng/config", local.SerializeAsString());
    log_success_init(ret);
    return true;
}