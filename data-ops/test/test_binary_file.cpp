#include "data-ops.h"
#include <bits/stdc++.h>
#include "debug/log.h"

int main()
{
    std::string path = "/tmp/temp_test";
    std::vector<char> d = {'c', 'b', 0, 1};
    auto fb = file_single::from_bytes(d);
    auto fd = fopen(path.c_str(),"w");
    if(fd == NULL){
        panic("fail to open from path:%s",path.c_str());
    }
    get_file_by_loc_handler_t handler = [](file_location loc){
        return (file_single *)NULL;
    };
    fb->to_file(fd, {}, handler);

    fclose(fd);
    delete fb;

    fd = fopen(path.c_str(),"r");
        if(fd == NULL){
        panic("fail to open from path:%s",path.c_str());
    }
    fb = file_single::from_file(fd, {}, handler);
    auto nd = fb->to_bytes();
    verbose("siz:%ld %ld",d.size(), nd.size());
    assert(d.size() == nd.size());
    for (int i = 0; i < d.size(); i++)
    {
        verbose("v:%d %d",(int)d[i], (int)nd[i]);
        assert(d[i] == nd[i]);
    }
    fclose(fd);
    delete fb;

}