#include "data-ops.h"
#include "file.pb.h"
#include "utils.h"
#include "debug/log.h"
class file_single_helper::file_single_helper_impl
{
public:
    data::file_single _file;
    const get_file_by_loc_handler_t *_handler = nullptr;
    file_handle _file_handle;

    const bytes &get_content()
    {
        if (get_parent() == NULL)
        {
            EQ(_file.has_binary(), true);
            return _file.binary().content();
        }
        else
        {
            return get_parent()->_impl->get_content();
        }
    }

private:
    file_single_helper *get_parent()
    {
        NE(_file_handle, 0);
        NE(_handler, nullptr);
        verbose("file has parent:%d handle:%d", _file.has_parent(), _file_handle);

        if (_file.has_parent() == false)
        {
            return nullptr;
        }

        verbose("handler name:%s", _handler->target_type().name());
        return (*_handler)(file_location{_file.parent(), _file_handle});
    }
};
file_single_helper::~file_single_helper() = default;
commit_handle file_single_helper::get_parent_commit_handle()
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
file_single_helper::file_single_helper(bytes data, file_handle file_handle, const get_file_by_loc_handler_t *handler) : _impl(std::make_unique<file_single_helper::file_single_helper_impl>()) // from bytes to structured object
{
    _impl->_handler = handler;
    _impl->_file_handle = file_handle;
    // verbose("init by data, handler:%p", &handler);
    if (!_impl->_file.ParseFromString(data))
    {
        panic("fail to parse");
    }
    verbose("success init by data");
    EQ(_impl->_file.has_binary() ^ _impl->_file.has_parent(), true);
    // _impl->set_content_hash();
}

file_single_helper::file_single_helper(const file_desc &fd, commit_handle commit_handle, const get_file_by_loc_handler_t *handler) : _impl(std::make_unique<file_single_helper::file_single_helper_impl>()) // from bytes to structured object
{

    verbose("init by fd");
    _impl->_handler = handler;

    _impl->_file_handle = fd.handle;
    auto data = read_binary(fd);
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

void file_single_helper::to_file(const file_desc &fd)
{
    write_binary(fd, _impl->get_content());
}

const data::file_single file_single_helper::get_proto_object()
{
    return _impl->_file;
}