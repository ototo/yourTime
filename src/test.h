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

#ifdef USE_ANSI_CODES
    #define ANSI_START_FAIL "\033[u\033[37;41m"
    #define ANSI_STOP_FAIL "\033[0m"
    #define ANSI_START_OK "\033[u\033[30;42m"
    #define ANSI_STOP_OK "\033[0m"
    #define ANSI_STORE "\033[s"
    #define ANSI_RESTORE "\033[u"
    #define ANSI_START_BAR_OK "\033[32m"
    #define ANSI_STOP_BAR_OK "\033[0m"
    #define ANSI_START_BAR_FAIL "\033[31m"
    #define ANSI_STOP_BAR_FAIL "\033[0m"
    #define ANSI_START_STATS_OK "\033[32m"
    #define ANSI_STOP_STATS_OK "\033[0m"
    #define ANSI_START_STATS_FAIL "\033[31m"
    #define ANSI_STOP_STATS_FAIL "\033[0m"
    #define BAR_CHAR_OK "\xE2\x96\xA0"
    #define BAR_CHAR_FAIL "\xE2\x96\xA0"
#else
    #define ANSI_START_FAIL ""
    #define ANSI_STOP_FAIL ""
    #define ANSI_START_OK ""
    #define ANSI_STOP_OK ""
    #define ANSI_STORE ""
    #define ANSI_RESTORE ""
    #define ANSI_START_BAR_OK ""
    #define ANSI_STOP_BAR_OK ""
    #define ANSI_START_BAR_FAIL ""
    #define ANSI_STOP_BAR_FAIL ""
    #define ANSI_START_STATS_OK ""
    #define ANSI_STOP_STATS_OK ""
    #define ANSI_START_STATS_FAIL ""
    #define ANSI_STOP_STATS_FAIL ""
    #define BAR_CHAR_OK "+"
    #define BAR_CHAR_FAIL "-"
#endif

#define TEST_TRUE(expr) \
    if (!(expr)) {\
        printf("%s%s:%d: Test for TRUE in %s:%s has failed!\n",\
        (failed ? "" : ANSI_START_FAIL "FAIL" ANSI_STOP_FAIL "\n"),\
        __FILE__, __LINE__, suite, name);\
        ++failed;\
    }

#define TEST_FALSE(expr) \
    if ((expr)) {\
        printf("%s%s:%d: Test for FALSE in %s:%s has failed!\n",\
        (failed ? "" : ANSI_START_FAIL "FAIL" ANSI_STOP_FAIL "\n"),\
        __FILE__, __LINE__, suite, name);\
        ++failed;\
    }

#define TEST_EQUAL(expr1, expr2) \
    if ((expr1) != (expr2)) {\
        printf("%s%s:%d: Test for EQUAL in %s:%s has failed!\n",\
        (failed ? "" : ANSI_START_FAIL "FAIL" ANSI_STOP_FAIL "\n"),\
        __FILE__, __LINE__, suite, name);\
        ++failed;\
    }
