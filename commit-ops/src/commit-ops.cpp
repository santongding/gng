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
    std::map<file_handle, std::unique_ptr<file_single_helper>> _files;

    const get_file_by_loc_handler_t _file_handler = [&](file_location loc)
    {
        verbose("file handler invoked");
        NE(loc.commit, 0);
        return this->get_file_by_loc(loc.commit, loc.file);
    };

    file_single_helper *get_file_by_loc(const commit_handle commit, file_handle file) // commit == 0 means find in current commit
    {
        verbose("parent:%d, commit:%d, f:%d", _commit.parent(), commit, file);
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
    file_single_helper *get_file_in_current(file_handle file)
    {
        EQ(_is_committed, true);
        if (_files.find(file) == _files.end())
        {
            auto fs = _commit.mutable_files()->find(file);
            if (fs == _commit.mutable_files()->end())
            {
                return nullptr;
            }
            fs->second.CheckInitialized();
            verbose("start init helper");

            _files.insert({file, std::make_unique<file_single_helper>(fs->second.SerializeAsString(), file, &_file_handler)});
            _commit.mutable_files()->erase(fs);
        }
        else
        {
            EQ(_commit.files().find(file), _commit.files().end());
        }

        NE(_files.find(file), _files.end());
        return _files.find(file)->second.get();
    }

private:
    commit_helper *get_commit(commit_handle handle)
    {
        NE(_commit_handler, nullptr);

        return (*_commit_handler)(handle);
    }
};
commit_helper::~commit_helper() = default;

commit_helper::commit_helper(bytes data, const get_commit_by_loc_handler_t *handler) : _impl(std::make_unique<commit_helper_impl>())
{
    verbose("commit init by data");
    _impl->_is_committed = true;
    _impl->_commit_handler = handler;
    if (!_impl->_commit.ParseFromString(data))
    {
        panic("fail to parse data");
    }
}

commit_helper::commit_helper(commit_handle parent, const get_commit_by_loc_handler_t *handler) : _impl(std::make_unique<commit_helper_impl>())
{
    _impl->_is_committed = false;
    _impl->_commit_handler = handler;
    _impl->_commit.set_parent(parent);
}

void commit_helper::commit_file(const file_desc &fd)
{
    EQ(_impl->_is_committed, false);
    EQ(_impl->_files.find(fd.handle), _impl->_files.end());
    commit_handle file_parent_commit = 0;
    verbose("commit parent:%d", _impl->_commit.parent());
    if (_impl->get_parent())
    {
        auto p_f = _impl->get_parent()->_impl->get_file_in_current(fd.handle);
        
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
            file_parent_commit = _impl->_commit.parent();
        }
    }
    verbose("file parent:%llu", file_parent_commit);

    _impl->_files.insert({fd.handle, std::unique_ptr<file_single_helper>(new file_single_helper(fd, file_parent_commit, &_impl->_file_handler))});
}

bytes commit_helper::commit()
{
    EQ(_impl->_is_committed, false);
    _impl->_is_committed = true;
    while (!_impl->_files.empty())
    {
        auto it = _impl->_files.begin();
        _impl->_commit.mutable_files()->insert({it->first, it->second->get_proto_object()});
        _impl->_files.erase(it);
    }
    return _impl->_commit.SerializeAsString();
}