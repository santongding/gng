#pragma once
#include <cstdio>
#include <execinfo.h>
#include <cstring>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define debug(fmt, ...) fprintf(stderr, "[debug %s:%d]" fmt "\n", __FILENAME__, __LINE__, ##__VA_ARGS__)
#define panic(fmt, ...) (fprintf(stderr, "[error %s:%d]" fmt "\n", __FILENAME__, __LINE__, ##__VA_ARGS__), assert(0))
#define debug_verbose(fmt, ...) fprintf(stderr, "[verbose %s:%d]" fmt "\n", __FILENAME__, __LINE__, ##__VA_ARGS__)

enum cmp_op
{
    EQ = 0,
    NE = 1,
    LG = 2,
    LGE = 3,
};

#define cmp1(op, x, y)                                              \
    do                                                              \
    {                                                               \
        bool exp;                                                   \
        if (op < 2)                                                 \
            exp = int((x) != (y)) ^ op;                             \
        else                                                        \
            panic("wrong cmp op:%d", op);                           \
        if (exp)                                                    \
        {                                                           \
            panic("[cmp op:%d]fail to cmp %s with %s", op, #x, #y); \
        }                                                           \
    } while (0);

#define cmp2(op, x, y)                                              \
    do                                                              \
    {                                                               \
        bool exp;                                                   \
        if (op == 2)                                                \
            exp = !((x) > (y));                                     \
        else if (op == 3)                                           \
            exp = !((x) >= (y));                                    \
        else                                                        \
            panic("wrong cmp op:%d", op);                           \
        if (exp)                                                    \
        {                                                           \
            panic("[cmp op:%d]fail to cmp %s with %s", op, #x, #y); \
        }                                                           \
    } while (0);

#define EQ(x, y) cmp1(EQ, (x), (y))
#define NE(x, y) cmp1(NE, (x), (y))
#define LG(x, y) cmp2(LG, (x), (y))
#define LGE(x, y) cmp2(LGE, (x), (y))