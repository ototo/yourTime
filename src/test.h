/* yourTime
 *
 * Testing related structures and functions.
 *
 * TODO: complete header (license, author, etc)
 */

#pragma once

#include <stdio.h>
#include <stdbool.h>


#define START_TEST(suite_name, test_name) \
int test_##test_name() \
{\
    char *suite = #suite_name;\
    char *name = #test_name;\
    int failed = 0;

#define END_TEST \
    return failed; \
}

#define TEST_TRUE(expr) \
    if (!(expr)) {\
        printf("%s%s:%d: Test for TRUE in %s:%s has failed!\n",\
        (failed ? "" : "\033[u\033[37;41mFAIL\033[0m\n"), __FILE__, __LINE__,\
        suite, name);\
        ++failed;\
    }

#define TEST_FALSE(expr) \
    if ((expr)) {\
        printf("%s%s:%d: Test for FALSE in %s:%s has failed!\n",\
        (failed ? "" : "\033[u\033[37;41mFAIL\033[0m\n"), __FILE__, __LINE__,\
        suite, name);\
        ++failed;\
    }

#define TEST_EQUAL(expr1, expr2) \
    if ((expr1) != (expr2)) {\
        printf("%s%s:%d: Test for EQUAL in %s:%s has failed!\n",\
        (failed ? "" : "\033[u\033[317;41m\n"), __FILE__, __LINE__,\
        suite, name);\
        ++failed;\
    }
