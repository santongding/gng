#include "data-ops.h"
#include <bits/stdc++.h>
#include "debug/log.h"
#include "utils.h"
int main()
{
    const std::string path = "/tmp/temp_test";

    data::file_single parent;

    parent.set_parent(0);
    parent.CheckInitialized();
    auto b = new data::file_binary();
    std::string content = "1232313212";
    content[3] = 0;
    b->set_content(content);
    parent.set_allocated_binary(b);
    get_file_by_loc_handler_t null_handler = [](file_location loc)
    {
        return (file_single_helper *)NULL;
    };
    auto p_helper = file_single_helper(parent.SerializeAsString(), 1, &null_handler);

    verbose("init parent helper");
    get_file_by_loc_handler_t handler = [&](file_location loc)
    {
        if (loc.commit != 0)
            return &p_helper;
        else
        {
            return (file_single_helper *)NULL;
        }
    };

    p_helper.to_file({path, 1});

    auto son = file_single_helper(file_desc{path, 1}, 1, &handler);
    auto fb = read_binary({path, 1});
    son.to_file({path, 1});
    EQ(fb, read_binary({path, 1}));
}