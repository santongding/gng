#include <cstdio>
#include <bits/stdc++.h>
#include "args.hxx"
#include "config.h"
#include "fs-utils.h"
#include "file-lock.h"
#include "commands-impl.h"

void Init(const std::string &progname, std::vector<std::string>::const_iterator beginargs, std::vector<std::string>::const_iterator endargs);
void Commit(const std::string &progname, std::vector<std::string>::const_iterator beginargs, std::vector<std::string>::const_iterator endargs);
void Checkout(const std::string &progname, std::vector<std::string>::const_iterator beginargs, std::vector<std::string>::const_iterator endargs);
void List(const std::string &progname, std::vector<std::string>::const_iterator beginargs, std::vector<std::string>::const_iterator endargs);

using commandtype = std::function<void(const std::string &, std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator)>;
bool verbose_value;

int main(int argc, char **argv)
{

    std::unordered_map<std::string, commandtype> map{
        {"init", Init},
        {"commit", Commit},
        {"checkout", Checkout},
        {"list", List}};

    const std::vector<std::string> args(argv + 1, argv + argc);

    std::stringstream useable_commands;
    useable_commands << "Valid commands are {";
    assert(map.size());
    for (auto &it : map)
    {
        useable_commands << it.first << ", ";
    }
    auto useable_commands_str = useable_commands.str();
    useable_commands_str[useable_commands_str.size() - 2] = '}';
    args::ArgumentParser parser("This is a git-like program", useable_commands_str.c_str());
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    parser.Prog(argv[0]);
    parser.ProglinePostfix("{command options}");
    args::Flag version(parser, "version", "Show the version of this program", {"version"});
    args::Flag verbose(parser, "verbose", "Output verbose log", {'v', "verbose"});

    // args::ValueFlag<std::string> htmlpath(parser, "html-path", "Specify the html path", {"html-path"});
    args::MapPositional<std::string, commandtype> command(parser, "command", "Command to execute", map);
    command.KickOut(true);
    // exclusive_lock_file lock("gng-private-file-lock");
    exclusive_lock_file _lock("/tmp/gng-private-file-lock");

    try
    {
        auto next = parser.ParseArgs(args);
        std::cout << std::boolalpha;
        //  std::cout << "html-path called: " << bool{htmlpath} << ", value: " << args::get(htmlpath) << std::endl;
        if (command)
        {
            std::cout << "Before command options:" << std::endl;
            std::cout << "Verbose: " << bool{verbose} << std::endl;

            verbose_value = bool{verbose};

            args::get(command)(argv[0], next, std::end(args));
        }
        else
        {
            if (version)
            {
                std::cout << "gng(Gng Not Git) version 1.0.0" << std::endl;
            }
            else
            {
                std::cout << parser;
                return 0;
            }
        }
    }
    catch (args::Help)
    {
        std::cout << parser;
        return 0;
    }
    catch (args::Error e)
    {
        std::cout << "parse args err" << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    return 0;
}

void Init(const std::string &progname, std::vector<std::string>::const_iterator beginargs, std::vector<std::string>::const_iterator endargs)
{
    std::cout << "Execute init" << std::endl;
    args::ArgumentParser parser("This command is used to init .gng as repository");
    parser.Prog(progname + " init");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Flag not_compress(parser, "not-compress", "Do not compress the data", {"not-compress"});
    args::Flag not_encrypt(parser, "not-encrypt", "Do not encrypt the data", {"not-encrypt"});
    args::Flag not_store_meta(parser, "not-store-meta", "Do not store metadata", {"not-store-meta"});
    args::ValueFlag<std::string> key(parser, "key", "Key to encrypt data. This value will be ignored if disable encryption", {'k', "key"});

    try
    {
        parser.ParseArgs(beginargs, endargs);

        if (dir_exists(".gng"))
        {
            std::cerr << "directory \".gng\" exists, please remove it before init" << std::endl;
            return;
        }
        else
        {
            EQ(mk_dir(".gng"), true);
        }
        EQ(init(not_compress, not_encrypt, not_store_meta, verbose_value, args::get(key)), true);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return;
    }
}

void Commit(const std::string &progname, std::vector<std::string>::const_iterator beginargs, std::vector<std::string>::const_iterator endargs)
{
    std::cout << "Execute commit" << std::endl;
    args::ArgumentParser parser("This command will store all non-ignored files to a commit whose parent commit is current one");
    parser.Prog(progname + " commit");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<std::string> key(parser, "key", "Key to encrypt data. This value will be ignored if disable encryption", {'k', "key"});

    try
    {
        parser.ParseArgs(beginargs, endargs);
        auto config = init_from_local(args::get(key));
        commit_impl(config);
        write_back_config(config);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return;
    }
}

void Checkout(const std::string &progname, std::vector<std::string>::const_iterator beginargs, std::vector<std::string>::const_iterator endargs)
{
    std::cout << "Execute checkout" << std::endl;
    args::ArgumentParser parser("This command will restore files in certain commit and setup head commit. Default use soft checkout, which will do nothing to the filesystem");
    parser.Prog(progname + " checkout");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Positional<std::string> cmt(parser, "commit", "The commit to checkout", ".");
    // args::Positional<std::string> cmt(parser, "commit", "commit to checkout", {'c', "commit"});
    args::Flag hard_checkout(parser, "hard-checkout", "Keep files in your filesystem same with the commit", {"hard"});
    args::Flag mixed_checkout(parser, "mixed-checkout", "Restore all files in the commit and do not delete uncontrolled flies", {"mixed"});

    args::ValueFlag<std::string> key(parser, "key", "Key to encrypt data. This value will be ignored if disable encryption", {'k', "key"});

    try
    {
        parser.ParseArgs(beginargs, endargs);
        std::cout << std::boolalpha;
        if (bool{hard_checkout} && bool{mixed_checkout})
        {
            std::cerr << "please select one option" << std::endl;
            return;
        }
        if (!bool{cmt})
        {
            std::cerr << "commit not specified" << std::endl;
            return;
        }
        auto config = init_from_local(args::get(key));
        checkout_op op = SOFT;
        if (bool{hard_checkout})
            op = HARD;
        if (bool{mixed_checkout})
            op = MIXED;
        commit_handle_t c;
        try
        {
            c = stoi(args::get(cmt));
        }
        catch (...)
        {
            std::cerr << "commit should be number" << std::endl;
            return;
        }
        checkout_impl(config, c, op);
        write_back_config(config);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return;
    }
}

void List(const std::string &progname, std::vector<std::string>::const_iterator beginargs, std::vector<std::string>::const_iterator endargs)
{
    std::cout << "Execute list" << std::endl;
    args::ArgumentParser parser("This command is used to list commits");
    parser.Prog(progname + " list");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<std::uint64_t> listed_commit(parser, "listed-commit", "List the commit and all its ancestors. If not presented, will list all commits", {'l', "listed-commit"});
    args::ValueFlag<std::string> key(parser, "key", "Key to encrypt data. This value will be ignored if disable encryption", {'k', "key"});

    // args::Flag bare(parser, "bare", "create a bare repository", {"bare"});
    // args::Flag quiet(parser, "quiet", "be quiet", {'q', "quiet"});
    // args::Positional<std::string> directory(parser, "directory", "The directory to create in", ".");
    try
    {
        parser.ParseArgs(beginargs, endargs);
        auto config = init_from_local(args::get(key));
        std::cout << std::boolalpha;
        std::cout << "listed-commit: " << bool{listed_commit} << ", value: " << args::get(listed_commit) << std::endl;
        list_impl(config, args::get(listed_commit));
    }
    catch (args::Help)
    {
        std::cout << parser;
        return;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return;
    }
}