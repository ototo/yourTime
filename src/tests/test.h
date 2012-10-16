/* yourTime
 *
 * Testing related structures and functions.
 *
 * TODO: complete header (license, author, etc)
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <setjmp.h>
#include <signal.h>
#include <execinfo.h>

#include "../error.h"


#define BACKTRACE_SIZE 16

#define START_CATCHING_SIGNALS \
    setup_signal_handlers();

#define STOP_CATCHING_SIGNALS \
    reset_signal_handlers();

#define SIGNAL_MARK \
    __test_signal_line = __LINE__;

#define RECATCH_SIGNALS \
    }\
    CATCH_SIGNALS

#define CATCH_SIGNALS \
    if (sigsetjmp(__test_g_sig_env, 1)) {\
         printf("%s%s:%d: Signal %d:%s caught in %s:%s!\n Backtrace is:\n",\
             (__test_failed ? "" : ANSI_START_FAIL "FAIL" ANSI_STOP_FAIL "\n"),\
            __FILE__, __test_signal_line, __test_g_signo,\
            signame(__test_g_signo), __test_suite, __test_name);\
        void *array[BACKTRACE_SIZE];\
        size_t size;\
        size = backtrace(array, BACKTRACE_SIZE);\
        char **symbols = backtrace_symbols(array, size);\
        for (int idx = 0; idx < size; ++idx) {\
            if (!symbols[idx])\
                break;\
            else\
                printf(" * %s\n", symbols[idx]);\
        }\
        free(symbols);\
        ++__test_failed;\
    }\
    else {

#define START_TEST(suite_name, test_name) \
int test_##test_name() \
{\
    char *__test_suite = #suite_name;\
    char *__test_name = #test_name;\
    int __test_failed = 0;\
    int __test_signal_line = __LINE__;\
    START_CATCHING_SIGNALS\
    CATCH_SIGNALS

#define END_TEST \
    }\
    STOP_CATCHING_SIGNALS\
    return __test_failed;\
}

#ifdef USE_ANSI_CODES
    #define ANSI_START_TEST "\033[2K\r"
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
        printf("%s%s:%d: Test for TRUE(" #expr\
                ") in %s:%s has failed!\n",\
        (__test_failed ? "" : ANSI_START_FAIL "FAIL" ANSI_STOP_FAIL "\n"),\
        __FILE__, __LINE__, __test_suite, __test_name);\
        ++__test_failed;\
    }

#define TEST_TRUE_ITER(expr, current, total) \
    if (!(expr)) {\
        printf("%s%s:%d: Iteration %d/%d: Test for TRUE(" #expr\
                ") in %s:%s has failed!\n",\
        (__test_failed ? "" : ANSI_START_FAIL "FAIL" ANSI_STOP_FAIL "\n"),\
        __FILE__, __LINE__, current, total, __test_suite, __test_name);\
        ++__test_failed;\
    }

#define TEST_FALSE(expr) \
    if ((expr)) {\
        printf("%s%s:%d: Test for FALSE(" #expr\
                ") in %s:%s has failed!\n",\
        (__test_failed ? "" : ANSI_START_FAIL "FAIL" ANSI_STOP_FAIL "\n"),\
        __FILE__, __LINE__, __test_suite, __test_name);\
        ++__test_failed;\
    }

#define TEST_FALSE_ITER(expr, current, total) \
    if ((expr)) {\
        printf("%s%s:%d: Iteration %d/%d: Test for FALSE(" #expr\
                ") in %s:%s has failed!\n",\
        (__test_failed ? "" : ANSI_START_FAIL "FAIL" ANSI_STOP_FAIL "\n"),\
        __FILE__, __LINE__, current, total, __test_suite, __test_name);\
        ++__test_failed;\
    }

#define TEST_EQUAL(expr1, expr2) \
    if ((expr1) != (expr2)) {\
        printf("%s%s:%d: Test for EQUAL(" #expr1 ", " #expr2\
                ") in %s:%s has failed!\n",\
        (__test_failed ? "" : ANSI_START_FAIL "FAIL" ANSI_STOP_FAIL "\n"),\
        __FILE__, __LINE__, __test_suite, __test_name);\
        ++__test_failed;\
    }

#define TEST_EQUAL_ITER(expr1, expr2, current, total) \
    if ((expr1) != (expr2)) {\
        printf("%s%s:%d: Iteration %d/%d: Test for EQUAL(" #expr1 ", " #expr2\
                ") in %s:%s has failed!\n",\
        (__test_failed ? "" : ANSI_START_FAIL "FAIL" ANSI_STOP_FAIL "\n"),\
        __FILE__, __LINE__, current, total, __test_suite, __test_name);\
        ++__test_failed;\
    }

#define TEST_NOT_EQUAL(expr1, expr2) \
    if ((expr1) == (expr2)) {\
        printf("%s%s:%d: Test for NOT EQUAL(" #expr1 ", " #expr2\
                ") in %s:%s has failed!\n",\
        (__test_failed ? "" : ANSI_START_FAIL "FAIL" ANSI_STOP_FAIL "\n"),\
        __FILE__, __LINE__, __test_suite, __test_name);\
        ++__test_failed;\
    }

#define TEST_NOT_EQUAL_ITER(expr1, expr2, current, total) \
    if ((expr1) == (expr2)) {\
        printf("%s%s:%d: Iteration %d/%d: Test for NOT EQUAL(" #expr1 ", " #expr2\
                ") in %s:%s has failed!\n",\
        (__test_failed ? "" : ANSI_START_FAIL "FAIL" ANSI_STOP_FAIL "\n"),\
        __FILE__, __LINE__, current, total, __test_suite, __test_name);\
        ++__test_failed;\
    }

#define TEST_NOT_IMPLEMENTED \
    printf("%s%s:%d: Test not implemented for %s:%s!\n",\
    (__test_failed ? "" : ANSI_START_FAIL "FAIL" ANSI_STOP_FAIL "\n"),\
    __FILE__, __LINE__, __test_suite, __test_name);\
    ++__test_failed;\

#define TEST_INCOMPLETE \
    bool is_test_complete = false;\
    TEST_TRUE(is_test_complete);


extern int __test_g_signo;
extern sigjmp_buf __test_g_sig_env;
char *signame(int signo);
void setup_signal_handlers();
void reset_signal_handlers();
