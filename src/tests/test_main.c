/* yourTime
 *
 * Main test entry file.
 *
 * TODO: complete header (license, author, etc)
 */

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>

#include "test.h"
#include "../version.h"

#include "test_forward_declarations.inc"

typedef int (*TestFunction)(void);

struct _TestRecord
{
    char            *suite_name;
    char            *test_name;
    TestFunction    function;
};

typedef struct _TestRecord TestRecord;


/* stuff needed to catch signals generated while running tests */
sigjmp_buf g_sig_env;
int g_signo;

char *signame(int signo)
{
    switch (signo) {
    case 1:
        return "SIGHUP";
    case 2:
        return "SIGINT";
    case 8:
        return "SIGFPE";
    case 11:
        return "SIGSEGV";
    case 13:
        return "SIGPIPE";
    case 15:
        return "SIGTERM";
    default:
        return "";
    }
}

static void catch_signal(int signo)
{
    g_signo = signo;
    siglongjmp(g_sig_env, signo);
    return;
}

void setup_signal_handlers()
{
    g_signo = 0;
    sigset(SIGINT, catch_signal);
    sigset(SIGFPE, catch_signal);
    sigset(SIGSEGV, catch_signal);
}

void reset_signal_handlers()
{
    sigset(SIGSEGV, SIG_DFL);
    sigset(SIGFPE, SIG_DFL);
    sigset(SIGINT, SIG_DFL);
}


char *get_bar_string(int total, int failed)
{
    static char bar[sizeof(ANSI_START_BAR_OK) + sizeof(ANSI_STOP_BAR_OK)
        + sizeof(ANSI_START_BAR_FAIL) + sizeof(ANSI_STOP_BAR_FAIL)
        + sizeof(BAR_CHAR_OK) * 10 + sizeof(BAR_CHAR_FAIL) * 10] = "";

    float perc_ok = (float)(total - failed) / (float)total;
    float perc_fail = 1.0 - perc_ok;
    int ok_part, fail_part;
    if (perc_ok < perc_fail) {
        fail_part = (int)round(perc_fail * 10.0);
        if (perc_ok > 0 && fail_part == 10)
            --fail_part;
        ok_part = 10 - fail_part;
    }
    else {
        ok_part = (int)nearbyintf(perc_ok * 10.0);
        if (perc_fail > 0 && ok_part == 10)
            --ok_part;
        fail_part = 10 - ok_part;
    }

#ifdef USE_ANSI_CODES
    strcat(bar, ANSI_START_BAR_OK);
#endif
    for (int i = 0; i < ok_part; ++i)
        strcat(bar, BAR_CHAR_OK);
#ifdef USE_ANSI_CODES
    strcat(bar, ANSI_STOP_BAR_OK);
#endif

#ifdef USE_ANSI_CODES
    strcat(bar, ANSI_START_BAR_FAIL);
#endif
    for (int i = ok_part; i < 10; ++i)
        strcat(bar, BAR_CHAR_FAIL);
#ifdef USE_ANSI_CODES
    strcat(bar, ANSI_STOP_BAR_FAIL);
#endif

    return bar;
}


int main(int argc, char *argv[])
{
    TestRecord tests[] = {
        #include "tests.inc"
        { NULL, NULL, NULL }
    };

    printf("Running tests for %s v%s [%s/%s]:\n\n",
            YOURTIME_FULL_NAME, YOURTIME_VERSION,
            YOURTIME_REVISION, YOURTIME_BRANCH);

    int rc = 0;
    int total = 0;
    int failed = 0;

    struct timeval tv_start, tv_stop;

    gettimeofday(&tv_start, NULL);

    for(TestRecord *test = tests; test->function; ++test) {
        printf(
#ifdef USE_ANSI_CODES
            ANSI_START_TEST "[" ANSI_STORE "....] %s:%s "
#else
            "- %s:%s ... "
#endif
            ,test->suite_name, test->test_name);
        rc = test->function();
        fflush(stdout);
        if (rc) {
            ++failed;
        }
        else
            printf(
#ifdef USE_ANSI_CODES
                ANSI_RESTORE ANSI_START_OK " ok " ANSI_STOP_OK "\r"
#else
                "ok\n"
#endif
                );
        fflush(stdout);
        ++total;
    }

    gettimeofday(&tv_stop, NULL);

    float elapsed =
        ((float)tv_stop.tv_sec + (float)tv_stop.tv_usec / 1000000.0) -
        ((float)tv_start.tv_sec + (float)tv_start.tv_usec / 1000000.0);

    printf(
#ifdef USE_ANSI_CODES
            ANSI_START_TEST
#endif
            "\nDone: %d tests [%s %s%d%s / %s%d%s] in %0.6f seconds.\n",
            total, get_bar_string(total, failed),
            ANSI_START_STATS_OK, total - failed, ANSI_STOP_STATS_OK,
            ANSI_START_STATS_FAIL, failed, ANSI_STOP_STATS_FAIL,
            elapsed);

    return failed ? 1 : 0;
}
