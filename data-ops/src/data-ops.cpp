#include "data-ops.h"
#include "file.pb.h"
#include "utils.h"
#include "debug/log.h"
class file_single_helper::file_single_helper_impl
{
public:
    data::file_single _file;
    const get_file_by_loc_handler_t *_handler = nullptr;
    file_handle_t _file_handle;

    const bytes_t &get_content()
    {
        debug_verbose("parent:%p", get_parent());
        if (get_parent() == nullptr)
        {
            EQ(_file.has_binary(), true);
            debug_verbose("has binary");
            return _file.binary().content();
        }
        else
        {
            debug_verbose("got parent");
            debug_verbose("parent has parent:%d", get_parent()->_impl->_file.has_parent());
            return get_parent()->_impl->get_content();
        }
    }

private:
    file_single_helper *get_parent()
    {
        NE(_file_handle, 0);
        NE(_handler, nullptr);
        debug_verbose("file has parent:%d handle:%llu", _file.has_parent(), _file_handle);

        if (_file.has_parent() == false || _file.parent() == 0)
        {
            return nullptr;
        }

        debug_verbose("handler name:%s", _handler->target_type().name());
        return (*_handler)(file_location{_file.parent(), _file_handle});
    }
};
file_single_helper::~file_single_helper() = default;
commit_handle_t file_single_helper::get_parent_commit_handle() const
{
    if (_impl->_file.has_parent())
    {
        NE(_impl->_file.parent(), 0);
        return _impl->_file.parent();
    }
    else
    {
        return 0;
    }
}
file_single_helper::file_single_helper(bytes_t data, file_handle_t file_handle, const get_file_by_loc_handler_t *handler) : _impl(std::make_unique<file_single_helper::file_single_helper_impl>()) // from bytes to structured object
{
    _impl->_handler = handler;
    _impl->_file_handle = file_handle;
    // debug_verbose("init by data, handler:%p", &handler);
    if (!_impl->_file.ParseFromString(data))
    {
        panic("fail to parse");
    }
    debug_verbose("success init by data");
    EQ(_impl->_file.has_binary() ^ _impl->_file.has_parent(), true);
    // _impl->set_content_hash();
}

file_single_helper::file_single_helper(const file_desc &fd, commit_handle_t commit_handle, const get_file_by_loc_handler_t *handler) : _impl(std::make_unique<file_single_helper::file_single_helper_impl>()) // from bytes to structured object
{

    debug_verbose("init by fd");
    _impl->_handler = handler;

    _impl->_file_handle = fd.handle;
    auto &data = fd.data;
    if (commit_handle && get_hash((*_impl->_handler)({commit_handle, fd.handle})->_impl->get_content()) == get_hash(data))
    {
        _impl->_file.set_parent(commit_handle);
    }
    else
    {
        auto b = new data::file_binary();
        b->set_content(data);
        _impl->_file.set_allocated_binary(b);
    }
    EQ(_impl->_file.has_binary() ^ _impl->_file.has_parent(), true);
}

void file_single_helper::to_file(file_desc &fd) const
{
    EQ(fd.data, "");
    EQ(_impl->_file.has_binary() ^ _impl->_file.has_parent(), true);
    fd.data = _impl->get_content();
    debug_verbose("content got");
}

bytes_t file_single_helper::to_bytes() const
{
    _impl->_file.CheckInitialized();
    EQ(_impl->_file.has_binary() ^ _impl->_file.has_parent(), true);
    return _impl->_file.SerializeAsString();
}