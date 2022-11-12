#include "commit-ops.h"
#include "commit.pb.h"
#include "debug/log.h"
#include "data-ops.h"

class commit_helper::commit_helper_impl
{
public:
    bool _is_committed;
    data::commit _commit;
    const get_commit_by_loc_handler_t *_commit_handler = nullptr;
    std::map<file_handle_t, std::unique_ptr<file_single_helper>> _files;
    commit_helper *get_parent()
    {
        if (_commit.parent())
        {
            return get_commit(_commit.parent());
        }
        else
        {
            return nullptr;
        }
    }
    void do_checkout()
    {
        while (!_commit.files().empty())
        {
            (const_cast<vfs *>(get_vfs()))->insert_path(_commit.files().begin()->first);
            if (get_file_in_current(get_vfs()->path2handle(_commit.files().begin()->first)) == nullptr)
            {
                panic("fail to init file helper");
            }
        }
        for (auto &f : _files)
        {
            file_desc desc = {"", f.first};
            f.second->to_file(desc);
            debug_verbose("vfs:%p", _vfs);
            get_vfs()->write(desc.handle, desc.data);
        }
    }

    const vfs *get_vfs()
    {
        NE(_vfs, nullptr);
        return _vfs;
    }

    void set_vfs(const vfs *vfs)
    {
        EQ(_vfs, nullptr);
        _vfs = vfs;
    }

    void commit_file(const file_handle_t handle)
    {
        EQ(_is_committed, false);
        EQ(_files.find(handle), _files.end());
        commit_handle_t file_parent_commit = 0;
        debug_verbose("commit parent:%llu", _commit.parent());
        if (get_parent())
        {
            auto p_f = get_parent()->_impl->get_file_in_current(handle);

            if (p_f == nullptr)
            {
                file_parent_commit = 0;
            }
            else if (p_f->get_parent_commit_handle())
            {
                file_parent_commit = p_f->get_parent_commit_handle();
            }
            else
            {
                file_parent_commit = _commit.parent();
            }
        }
        debug_verbose("file parent:%llu", file_parent_commit);
        file_desc f{get_vfs()->read(handle), handle};

        _files.insert({handle, std::unique_ptr<file_single_helper>(new file_single_helper(f, file_parent_commit, &_file_handler))});
    }

private:
    const vfs *_vfs = nullptr;

    const get_file_by_loc_handler_t _file_handler = [&](file_location loc)
    {
        debug_verbose("file handler invoked");
        NE(loc.commit, 0);
        return this->get_file_by_loc(loc.commit, loc.file);
    };

    file_single_helper *get_file_by_loc(const commit_handle_t commit, file_handle_t file) // commit == 0 means find in current commit
    {
        debug_verbose("parent:%llu, commit:%llu, f:%llu", _commit.parent(), commit, file);
        if (commit == 0)
        {
            return get_file_in_current(file);
        }
        else
        {
            // EQ(_is_committed, false);
            auto target_commit = get_commit(commit);
            NE(target_commit, nullptr);
            auto ret = target_commit->_impl->get_file_by_loc(0, file);
            NE(ret, nullptr);
            return ret;
        }
    }

    file_single_helper *get_file_in_current(file_handle_t file)
    {
        EQ(_is_committed, true);
        file_path_t p = get_vfs()->handle2path(file);
        if (_files.find(file) == _files.end())
        {
            auto fs = _commit.mutable_files()->find(p);
            if (fs == _commit.mutable_files()->end())
            {
                return nullptr;
            }
            fs->second.CheckInitialized();
            // debug_verbose("start init helper");

            _files.insert({file, std::make_unique<file_single_helper>(fs->second.SerializeAsString(), file, &_file_handler)});
            _commit.mutable_files()->erase(fs);
        }
        else
        {
            EQ(_commit.files().find(p), _commit.files().end());
        }

        NE(_files.find(file), _files.end());
        return _files.find(file)->second.get();
    }
    commit_helper *get_commit(commit_handle_t handle)
    {
        NE(_commit_handler, nullptr);

        return (*_commit_handler)(handle);
    }
};

commit_helper::~commit_helper() = default;

commit_helper::commit_helper(bytes_t data, const get_commit_by_loc_handler_t *handler, const vfs *vfs) : _impl(std::make_unique<commit_helper_impl>())
{
    // debug_verbose("commit init by data");
    _impl->_is_committed = true;
    _impl->_commit_handler = handler;
    _impl->set_vfs(vfs);
    if (!_impl->_commit.ParseFromString(data))
    {
        panic("fail to parse data");
    }
}

commit_helper::commit_helper(commit_handle_t parent, const get_commit_by_loc_handler_t *handler, const vfs *vfs) : _impl(std::make_unique<commit_helper_impl>())
{
    _impl->_is_committed = false;
    _impl->_commit_handler = handler;
    _impl->_commit.set_parent(parent);
    _impl->set_vfs(vfs);
    for (auto &f : _impl->get_vfs()->get_all_file_handles())
    {
        _impl->commit_file(f);
    }
    _impl->_is_committed = true;
}

commit_handle_t commit_helper::get_parent()
{
    return _impl->_commit.parent();
}

bytes_t commit_helper::commit_to_bytes()
{
    EQ(_impl->_is_committed, true);
    while (!_impl->_files.empty())
    {
        auto it = _impl->_files.begin();
        auto obj = data::file_single();
        if (!obj.ParseFromString(it->second->to_bytes()))
        {
            panic("fail to pares");
        }
        file_path_t p = _impl->get_vfs()->handle2path(it->first);
        _impl->_commit.mutable_files()->insert({p, obj});
        _impl->_files.erase(it);
    }
    return _impl->_commit.SerializeAsString();
}

void commit_helper::checkout()
{
    EQ(_impl->_is_committed, true);

    _impl->do_checkout();
}