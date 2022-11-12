#include "commit-mgr.h"
#include "commit.pb.h"
#include "debug/log.h"
#include "fs-utils.h"
#include "sstream"

class commit_mgr::commit_mgr_impl
{
private:
    const vfs *_vfs = nullptr;
    commit_handle_t _max_handle;

public:
    commit_mgr_impl(const file_path_t &path_prefix, const vfs *vfs) : _path_prefix(path_prefix), _vfs(vfs)
    {
        _max_handle = 0;
        for (commit_handle_t i = 1;; i++)
        {
            file_path_t p = _path_prefix + commit_handle_to_str(i);
            if (file_exists(p))
            {
                _max_handle = i;
            }
            else
            {
                break;
            }
        }
    };
    commit_handle_t commit_max_handle()
    {
        return _max_handle;
    }
    void do_checkout(commit_handle_t commit) // mixed checkout
    {
        auto helper = get_commit_helper(commit);
        helper->checkout();
    }
    commit_handle_t do_commit(commit_handle_t commit)
    {
        debug_verbose("do commit, parent:%llu", commit);
        if (commit)
        {
            NE(get_commit_helper(commit), nullptr);
        }
        _commits[commit_max_handle() + 1] = std::make_unique<commit_helper>(commit, &_get_commit_handler, _vfs);
        _max_handle++;
        file_path_t p = _path_prefix + commit_handle_to_str(commit_max_handle());
        _vfs->store_committed(p, get_commit_helper(commit_max_handle())->commit_to_bytes());
        return commit_max_handle();
    }

    void do_list(commit_handle_t commit)
    {
        debug_verbose("start log list");
        if (commit > commit_max_handle())
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
        std::cout << std::setw(16) << "handle" << std::setw(16) << "parent" << std::endl;
        if (!commit)
        {
            for (int i = commit_max_handle(); i >= 1; i--)
            {
                if (i > 1)
                {
                    NE(get_commit_helper(i)->get_parent(), 0);
                }
                else
                {
                    EQ(get_commit_helper(i)->get_parent(), 0);
                }
                std::cout << std::setw(16) << i << std::setw(16) << get_commit_helper(i)->get_parent() << std::endl;
            }
        }
        else
        {

            do
            {
                std::cout << std::setw(16) << commit << std::setw(16) << get_commit_helper(commit)->get_parent() << std::endl;
                commit = get_commit_helper(commit)->get_parent();
            } while (commit);
        }
        std::cout << "listing current not ignored files:" << std::endl;
        std::cout << std::setw(16) << "file handle" << std::setw(16) << "path" << std::endl;
        auto handles = _vfs->get_all_file_handles();
        for (auto i = 0; i < handles.size(); i++)
        {
            std::cout << std::setw(16) << std::hex << handles[i] << std::setw(16) << _vfs->handle2path(handles[i]) << std::endl;
        }
    }

private:
    const file_path_t _path_prefix;
    // const command_op _op;
    std::map<commit_handle_t, std::unique_ptr<commit_helper>> _commits;
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
        if (handle > commit_max_handle())
        {
            panic("commit handle:%llu not found", handle);
        }
        else
        {
            if (_commits.find(handle) == _commits.end())
            {
                file_path_t p = _path_prefix + commit_handle_to_str(handle);
                EQ(file_exists(p), true);
                auto content = _vfs->load_committed(p);
                _commits[handle] = std::make_unique<commit_helper>(content, &_get_commit_handler, _vfs);
            }
            NE(_commits.find(handle), _commits.end());
            return _commits[handle].get();
        }
    }
};

commit_mgr::~commit_mgr() = default;

commit_mgr::commit_mgr(const file_path_t &path_prefix, const vfs *vfs) : _impl(std::make_unique<commit_mgr_impl>(path_prefix, vfs))
{
}

void commit_mgr::do_list(commit_handle_t commit)
{
    _impl->do_list(commit);
}

commit_handle_t commit_mgr::do_commit(commit_handle_t current)
{
    return _impl->do_commit(current);
}

void commit_mgr::do_checkout(commit_handle_t commit, checkout_op op)
{
    if (commit > _impl->commit_max_handle())
    {
        panic("handle:%llu not found", commit);
    }
    if (op != SOFT)
    {
        _impl->do_checkout(commit);
    }
}