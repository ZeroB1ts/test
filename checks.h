#ifndef CHECKS_H
#define CHECKS_H

#include "ProcData.h"

#define TEST(name) printf("\n\033[1mTEST: %s\033[0m\n", name)
#define ASSERT(cond) do { \
    if (!(cond)) { \
        printf("  [FAIL] %s:%d (%s)\n", __FILE__, __LINE__, #cond); \
        return FAILURE; \
    } else { \
        printf("  [OK] %s\n", #cond); \
    } \
} while(0)

#define ASSERT_EQUAL(a, b) ASSERT((a) == (b))
#define ASSERT_FLOAT_EQUAL(a, b) ASSERT(fabs((a) - (b)) < 1e-6)
#define ASSERT_STR_EQUAL(a, b) do { \
    const char* _a = (a); \
    const char* _b = (b); \
    if (_a == NULL || _b == NULL) { \
        printf("[ERROR] NULL pointer in ASSERT_STR_EQUAL\n"); \
        ASSERT(0); \
    } \
    int result = strcmp(_a, _b); \
    if (result != 0) { \
        printf("  [FAIL] Expected: '%s', Actual: '%s'\n", _b, _a); \
        ASSERT(0); \
    } \
} while(0)

StatData create_entry(long, int, float, int, int);
void free_hash_table(HashEntry **);

#endif