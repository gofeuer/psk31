#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>

#define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "ASSERTION FAILED: %s\n", message); \
            exit(1); \
        } \
    } while (0)

#endif // TEST_H
