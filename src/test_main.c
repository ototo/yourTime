/* yourTime
 *
 * Main test entry file.
 *
 * TODO: complete header (license, author, etc)
 */

#include <stdio.h>

#include "test.h"
#include "version.h"

#include "test_forward_declarations.inc"

typedef int (*TestFunction)(void);

struct _TestRecord
{
    char            *suite_name;
    char            *test_name;
    TestFunction    function;
};

typedef struct _TestRecord TestRecord;


int main(int argc, char *argv[])
{
    TestRecord tests[] = {
        #include "tests.inc"
        { NULL, NULL, NULL }
    };

    printf("Running tests for %s v%s:\n", YOURTIME_FULL_NAME,
                                            YOURTIME_VERSION);

    int rc = 0;
    int total = 0;
    int failed = 0;
    for(TestRecord *test = tests; test->function; ++test) {
        printf("- %s:%s: ", test->suite_name, test->test_name);
        rc = test->function();
        if (rc) {
            ++failed;
        }
        else
            printf("OK\n");
        ++total;
    }
    printf("Done running tests (%d total, %d failed).\n", total, failed);

    return 0;
}
