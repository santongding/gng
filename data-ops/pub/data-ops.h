#pragma once
#include <string>
#include <vector>
#include <type.h>

class file_single;
typedef std::function<file_single *(file_location)> get_file_by_loc_handler_t;
typedef struct
{
    const get_file_by_loc_handler_t &handler;
    const file_location parent;
} loc_enclave;

class file_single
{
public:
    static file_single *from_bytes(std::vector<char> data, loc_enclave loc); // from bytes to structured object

    static file_single *from_file(FILE *fd, loc_enclave loc); // from local disk to structured object

    // bytes_hash get_completed_content_hash(file_location parent, const get_file_by_loc_handler_t &handler); // return serializable bytes's hash value, without metadata.

    std::vector<char> to_bytes(); // from structured object to serializable bytes
    void to_file(FILE *fd);       // from structured object to local disk
    virtual ~file_single(){};
    virtual file_type type_identity() = 0; // binary or text

    bool is_same_with_committed(); // return if file not changed since last commit

protected:
    const loc_enclave _loc;

    file_single(loc_enclave loc) : _loc(loc){};

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
    virtual std::vector<char> to_bytes_content() = 0;
    virtual void to_file_content(FILE *fd) = 0;
};
