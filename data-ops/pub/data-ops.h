#pragma once

#include "file.pb.h"
#include <type.h>
class file_single_helper;
typedef std::function<file_single_helper *(file_location)> get_file_by_loc_handler_t;

class file_single_helper
{
public:
    // static file_single_helper *from_file(FILE *fd); // from local disk to structured object

    // bytes_hash get_completed_content_hash(file_location parent, const get_file_by_loc_handler_t &handler); // return serializable bytes's hash value, without metadata.

    // void to_file(FILE *fd);       // from structured object to local disk
    virtual ~file_single_helper();
    // virtual file_type type_identity() = 0; // binary or text

    // bool is_same_with_committed(); // return if file not changed since last commit

    file_single_helper(bytes data, file_handle file_handle,const get_file_by_loc_handler_t *handler);                                                 // from bytes to structured object
    file_single_helper(const file_desc& fd,commit_handle commit_handle, const get_file_by_loc_handler_t *handler); // from file to structured object

    // bytes to_bytes(); // from structured object to serializable bytes

    void to_file(const file_desc& fd); // from structured object to binary file

    /*file_single_helper(FILE *fd, data::file_location *parent)
    {
        static uint8_t buf[256];
        size_t n;
        while ((n = fread(buf, 1, 256, fd)) > 0)
        {
            _file.ParsePartialFromArray(buf, n);
        }
    }*/

    commit_handle get_parent_commit_handle();

    const data::file_single get_proto_object();

private:
    struct file_single_helper_impl;
    std::unique_ptr<file_single_helper_impl> _impl;
    /*
        bool has_parent()
        {
            return _loc.handler(_loc.parent) != NULL;
        }
        file_single *get_parent()
        {
            auto p = _loc.handler(_loc.parent);
            assert(p->type_identity() == type_identity());
            return p;
        }
        // bytes_hash _content_hash;
        virtual bool is_same_content_with_committed() = 0;
        virtual bytes to_bytes_content() = 0;
        virtual void to_file_content(FILE *fd) = 0;*/
};
