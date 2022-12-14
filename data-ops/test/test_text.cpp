#include "data-ops.h"
#include <bits/stdc++.h>
#include "debug/log.h"

int main()
{
    bytes d = {
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
    get_file_by_loc_handler_t handler = [](file_location loc)
    {
        return (file_single *)NULL;
    };
    for (int i = 0; i < 10; i++)
    {

        auto ft = file_single::from_bytes(d, {{}, handler});
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