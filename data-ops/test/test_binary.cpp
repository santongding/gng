#include "data-ops.h"
#include <bits/stdc++.h>
#include "debug/log.h"

int main()
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