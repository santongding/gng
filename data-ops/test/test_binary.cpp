#include "data-ops.h"
#include <bits/stdc++.h>
#include "debug/log.h"
#include "file.pb.h"
int main()
{

    get_file_by_loc_handler_t handler = [](file_location loc)
    {
        return nullptr;
    };

    data::file_single f;

    f.set_parent(0);
    f.CheckInitialized();
    auto b = new data::file_binary();
    std::string content = "1232313212";
    content[3] = 0;
    b->set_content(content);
    f.set_allocated_binary(b);
    auto fb = file_single_helper(f.SerializeAsString(), 1, &handler);
    NE(f.SerializeAsString(), "");
    EQ(f.SerializeAsString(), fb.get_proto_object().SerializeAsString());
}