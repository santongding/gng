#pragma once
#include <string>
#include <vector>
#include <type.h>

class file_single
{
public:
    std::vector<char> to_bytes();
    
    static file_single *from_bytes(const std::vector<char> &data);

    // void to_file(std::string path, file_location loc);
    //static from_file(std::string path, file_location loc);

protected:
    file_single(){};
    virtual std::vector<char> encode_content() = 0;
    virtual file_type type_identity() = 0;
};
