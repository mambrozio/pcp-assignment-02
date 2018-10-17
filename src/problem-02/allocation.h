#if !defined(allocation_h)
#define allocation_h

#include <stdio.h>
#include <stdlib.h>

#define MEMORY_ERROR \
    { fprintf(stderr, "error: not enough memory\n"); \
    exit(1); } \

#define MALLOC(x, t) \
    x = (t*)malloc(sizeof(t)); \
    if (!x) MEMORY_ERROR; \

#define MALLOC_ARRAY(x, t, n) \
    x = (t*)malloc(n * sizeof(t)); \
    if (!x) MEMORY_ERROR; \

#endif
