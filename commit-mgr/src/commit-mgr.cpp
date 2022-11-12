#include "commit-mgr.h"
#include "commit.pb.h"
#include "debug/log.h"
#include "fs-utils.h"
#include "sstream"
class commit_mgr::commit_mgr_impl
{
public:
    commit_mgr_impl(const file_path_t &path_prefix) : _path_prefix(path_prefix)
    {
        for (commit_handle_t i = 1;; i++)
        {
            file_path_t p = _path_prefix + commit_handle_to_str(i);
            if (file_exists(p))
            {
                auto content = read_binary(p);
                _commits.push_back(std::make_unique<commit_helper>(content, &_get_commit_handler));
            }
            else
            {
                break;
            }
        }
        _max_commit_handle = _commits.size();
    };

    void do_list(commit_handle_t commit)
    {
        debug_verbose("start log list");
        if (commit > _max_commit_handle)
        {
            std::cout << "commit handle not found" << std::endl;
            return;
        }
        if (!commit)
        {

            std::cout << "listing all commits:" << std::endl;
        }
        else
        {
            std::cout << "listing commit " << commit_handle_to_str(commit) << " :" << std::endl;
        }
        std::cout << std::setw(10) << "handle" << std::setw(10) << "parent" << std::endl;
        if (!commit)
        {
            for (int i = _max_commit_handle; i >= 1; i--)
            {
                if (i > 1)
                {
                    NE(get_commit_helper(i)->get_parent(), 0);
                }
                else
                {
                    EQ(get_commit_helper(i)->get_parent(), 0);
                }
                std::cout << std::setw(10) << i << std::setw(10) << get_commit_helper(i)->get_parent() << std::endl;
            }
        }
        else
        {

            do
            {
                std::cout << std::setw(10) << commit << std::setw(10) << get_commit_helper(commit)->get_parent() << std::endl;
                commit = get_commit_helper(commit)->get_parent();
            } while (commit);
        }
    }

private:
    const file_path_t _path_prefix;
    // const command_op _op;
    std::vector<std::unique_ptr<commit_helper>> _commits;
    uint64_t _max_commit_handle;
    const get_commit_by_loc_handler_t _get_commit_handler = [&](commit_handle_t loc)
    {
        return this->get_commit_helper(loc);
    };
    std::string commit_handle_to_str(commit_handle_t i)
    {
        std::stringstream ss;
        ss << std::setw(8) << std::setfill('0') << i;
        return ss.str();
    }
    commit_helper *get_commit_helper(commit_handle_t handle)
    {
        if (handle > _commits.size())
        {
            panic("commit handle:%llu not found", handle);
        }
        else
        {
            return _commits[handle - 1].get();
        }
    }
};
commit_mgr::~commit_mgr() = default;

commit_mgr::commit_mgr(const file_path_t &path_prefix, command_op op) : _impl(std::make_unique<commit_mgr_impl>(path_prefix))
{
}

void commit_mgr::do_list(commit_handle_t commit)
{
    _impl->do_list(commit);
}