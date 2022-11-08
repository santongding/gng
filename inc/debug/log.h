#pragma once
#include <cstdio>

#define debug(fmt, ...) fprintf(stderr, "[debug]" fmt "\n", __VA_ARGS__)
#define panic(fmt, ...) (fprintf(stderr, "[error]" fmt "\n", __VA_ARGS__), exit(1))
#define verbose(fmt, ...) fprintf(stderr, "[verbose]" fmt "\n", __VA_ARGS__)