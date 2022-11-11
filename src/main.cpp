#include <cstdio>
#include <bits/stdc++.h>
#include "file.pb.h"
#include "utils.h"
#include "data-ops.h"
char d[] = {0, 'c', 'b', 0};
int main()
{
    /*
    auto fb = file_single_helper(stringd);
    auto encoded = fb.to_bytes();
    std::cout << encoded.size();
    for (auto x : encoded)
    {
        std::cout << int(x);
    }
    std::cout << std::endl;*/
    data::file_binary f;
    std::string s("DSAADSSDSAADSSDSAADSSS");
    s[3] = 0;

    f.set_content(s);
    auto bs = f.SerializeAsString();
    printf("siz:%ld len:%ld cap:%ld\n",s.size(),s.length(),s.capacity());

    fwrite(&bs[0], 1, bs.size(),fopen("./tmp","wb"));
}