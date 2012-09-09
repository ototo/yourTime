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


#define BACKTRACE_SIZE 16

#define CATCH_SIGNALS \
    setup_signal_handlers();\
    if (sigsetjmp(g_sig_env, 1)) {\
         printf("%s%s:%d: Signal %d:%s caught in %s:%s!\n Backtrace is:\n",\
             (failed ? "" : ANSI_START_FAIL "FAIL" ANSI_STOP_FAIL "\n"),\
            __FILE__, __LINE__, g_signo, signame(g_signo), suite, name);\
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
        ++failed;\
    }

#define STOP_CATCHING_SIGNALS \
    reset_signal_handlers();

#define START_TEST(suite_name, test_name) \
int test_##test_name() \
{\
    char *suite = #suite_name;\
    char *name = #test_name;\
    int failed = 0;\
    CATCH_SIGNALS\
    else {

#define END_TEST \
    }\
    STOP_CATCHING_SIGNALS\
    return failed;\
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

#define TEST_NOT_IMPLEMENTED \
    printf("%s%s:%d: Test not implemented for %s:%s!\n",\
    (failed ? "" : ANSI_START_FAIL "FAIL" ANSI_STOP_FAIL "\n"),\
    __FILE__, __LINE__, suite, name);\
    ++failed;\


extern int g_signo;
extern sigjmp_buf g_sig_env;
char *signame(int signo);
void setup_signal_handlers();
void reset_signal_handlers();
