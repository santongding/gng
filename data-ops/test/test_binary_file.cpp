#include "data-ops.h"
#include <bits/stdc++.h>
#include "debug/log.h"

int main()
{
    std::string path = "/tmp/temp_test";
    std::vector<char> d = {'c', 'b', 0, 1};
    file_single * fb0;
    get_file_by_loc_handler_t handler = [&](file_location loc)
    {
        if (loc.commit == 0)
        {
            return (file_single *)NULL;
        }
        return (file_single *)fb0;
    };
    auto fb = file_single::from_bytes(d, { handler,{}});
    auto fd = fopen(path.c_str(), "w");
    if (fd == NULL)
    {
        panic("fail to open from path:%s", path.c_str());
    }
    fb0 = fb;

    fb = file_single::from_bytes({1}, {handler,{1}});
    fb->to_file(fd);

    fclose(fd);
    delete fb;

    fd = fopen(path.c_str(), "r");
    if (fd == NULL)
    {
        panic("fail to open from path:%s", path.c_str());
    }
    fb = file_single::from_file(fd,{handler,{}});
    auto nd = fb->to_bytes();
    verbose("nd siz:%ld\n", nd.size());
    assert(nd.size() == 1);
    delete fb0;
    fb0 = NULL;
    delete fb;
    fb = file_single::from_file(fd, { handler,{}});
    assert(d.size() == nd.size());
    for (int i = 0; i < d.size(); i++)
    {
        verbose("v:%d %d", (int)d[i], (int)nd[i]);
        assert(d[i] == nd[i]);
    }
    fclose(fd);
    delete fb;
}