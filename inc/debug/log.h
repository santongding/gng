#pragma once
#include <cstdio>

#define debug(fmt, ...) fprintf(stderr, "[debug %s:%d]" fmt "\n", __FILE__,__LINE__,__VA_ARGS__)
#define panic(fmt, ...) (fprintf(stderr, "[error%s:%d]" fmt "\n", __FILE__,__LINE__,__VA_ARGS__), assert(0))
#define verbose(fmt, ...) fprintf(stderr, "[verbose %s:%d]" fmt "\n", __FILE__,__LINE__,__VA_ARGS__)