#include "data-ops.h"
#include <bits/stdc++.h>
#include "debug/log.h"

void test_binary()
{
    std::vector<char> d = {'c', 'b', 0, 1};
    auto fb = file_single::from_bytes(d);
    auto encoded = fb->to_bytes();
    std::cout << encoded.size();
    for (auto x : encoded)
    {
        std::cout << int(x);
    }
    std::cout << std::endl;
    delete fb;
}
void test_text()
{
    std::vector<char> d = {
        0,
        2, 0, 0, 0,
        2, 0, 0, 0,
        'a', 'b',
        3, 0, 0, 0,
        'a', 'b', 'c',
        1,
        1, 0, 0, 0,
        23, 0, 0, 0, 0, 0, 0, 0,
        2};
    auto origin = d;
    for (int i = 0; i < 10; i++)
    {

        auto ft = file_single::from_bytes(d);
        d = ft->to_bytes();
        delete ft;
    }
    // std::cout << int(d[d.size()-1]);
    assert(d.size() == origin.size());
    for (int i = 0; i < d.size(); i++)
    {
        assert(d[i] == origin[i]);
    }
}
void test_binary_file()
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
int main()
{
    test_binary();
    test_binary_file();
    test_text();
    std::cout << std::endl;
}