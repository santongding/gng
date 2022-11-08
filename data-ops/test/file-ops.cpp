#include "data-ops.h"
#include <bits/stdc++.h>
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
    std::cout<<std::endl;

}
void test_text()
{
    std::vector<char> d = {
        0,
        1, 0, 0, 0,
        2, 0, 0, 0,
        'a', 'b',
        1,
        1, 0, 0, 0,
        23, 0, 0, 0, 0, 0, 0, 0,
        2};
    auto origin = d;
    for (int i = 0; i < 10; i++)
    {

        auto ft = file_single::from_bytes(d);
        d = ft->to_bytes();
    }
    // std::cout << int(d[d.size()-1]);
    assert(d.size() == origin.size());
    for (int i = 0; i < d.size(); i++)
    {
        d[i] == origin[i];
    }
}
int main()
{
    test_binary();
    test_text();
    exit(1);
    std::cout<<std::endl;
}