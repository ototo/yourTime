/* yourTime
 *
 * String test set file.
 *
 * TODO: complete header (license, author, etc)
 */

#define _XOPEN_SOURCE 600

#include <string.h>

#include "test.h"
#include "../string.h"


/* test data */
static const char const *test_chars = "test chars";

/* Test for allocation routine.
 *
 */

START_TEST(string, string_allocate)

    int rc = string_allocate(NULL, 0);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK
    rc = string_allocate(NULL, 1);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK
    String str;
    rc = string_allocate(&str, 256);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(str.recycler, free);
    TEST_EQUAL(str.refcount, 1);
    TEST_NOT_EQUAL(str.chars, NULL);

    SIGNAL_MARK
    rc = string_release(&str);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(str.refcount, 0);
    TEST_EQUAL(str.chars, NULL);

END_TEST


/* Test for static allocation routine.
 *
 */

START_TEST(string, string_allocate_static)

    int rc = string_allocate_static(NULL, "test string");
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK
    String str;
    rc = string_allocate_static(&str, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK
    rc = string_allocate_static(&str, test_chars);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(str.recycler, string_zero_recycler);
    TEST_EQUAL(str.refcount, 1);
    TEST_NOT_EQUAL(str.chars, NULL);
    TEST_EQUAL(0, strcmp(str.chars, test_chars));

    SIGNAL_MARK
    rc = string_release(&str);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(str.refcount, 0);
    TEST_EQUAL(str.chars, NULL);

END_TEST


/* Test for dynamic allocation routine.
 *
 */

START_TEST(string, string_allocate_dynamic)

    int rc = string_allocate_dynamic(NULL, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK
    rc = string_allocate_dynamic(NULL, test_chars);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK
    String str;
    rc = string_allocate_dynamic(&str, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK
    rc = string_allocate_dynamic(&str, test_chars);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(str.recycler, free);
    TEST_EQUAL(str.refcount, 1);
    TEST_NOT_EQUAL(str.chars, NULL);
    TEST_TRUE(strcmp(test_chars, str.chars) == 0);

    SIGNAL_MARK
    rc = string_release(&str);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(str.refcount, 0);
    TEST_EQUAL(str.chars, NULL);

END_TEST


/* Test for deallocation routines.
 *
 */

START_TEST(string, string_release)

    String str;
    int rc = string_allocate_static(&str, test_chars);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(str.recycler, string_zero_recycler);
    TEST_EQUAL(str.refcount, 1);
    TEST_NOT_EQUAL(str.chars, NULL);
    TEST_EQUAL(0, strcmp(str.chars, test_chars));

    // pretend there are 2 users of the string
    str.refcount = 2;

    SIGNAL_MARK
    rc = string_release(&str);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(str.refcount, 1);
    TEST_NOT_EQUAL(str.chars, NULL);

    SIGNAL_MARK
    rc = string_release(&str);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(str.refcount, 0);
    TEST_EQUAL(str.chars, NULL);

    SIGNAL_MARK
    rc = string_release(&str);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);
    TEST_EQUAL(str.refcount, 0);
    TEST_EQUAL(str.chars, NULL);

END_TEST


/* Test for copy routine.
 *
 */

START_TEST(string, string_copy)

    String from_empty, from_static, from_dynamic, to;
    memset(&from_empty, 0, sizeof(from_empty));
    memset(&from_static, 0, sizeof(from_static));
    memset(&from_dynamic, 0, sizeof(from_dynamic));
    memset(&to, 0, sizeof(to));

    int rc = string_allocate_static(&from_static, test_chars);
    TEST_EQUAL(rc, RC_OK);

    rc = string_allocate_dynamic(&from_dynamic, test_chars);
    TEST_EQUAL(rc, RC_OK);

    /* check for missing target */
    SIGNAL_MARK
    rc = string_copy(NULL, &from_empty);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    /* check for missing source */
    SIGNAL_MARK
    rc = string_copy(&to, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    /* check for static source */
    SIGNAL_MARK
    rc = string_copy(&to, &from_static);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(to.refcount, 1);
    TEST_NOT_EQUAL(to.chars, NULL);
    TEST_EQUAL(to.recycler, free);

    /* check for non-empty target with static allocation */
    SIGNAL_MARK
    rc = string_copy(&to, &from_empty);
    TEST_EQUAL(rc, RC_OK);
    char *ptr = to.chars;
    rc = string_copy(&to, &from_empty);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(ptr, to.chars);

    /* check for non-empty target with dynamic allocation */
    SIGNAL_MARK
    rc = string_copy(&to, &from_dynamic);
    TEST_EQUAL(rc, RC_OK);
    ptr = to.chars;

    rc = string_copy(&to, &from_empty);
    TEST_EQUAL(rc, RC_OK);
    TEST_NOT_EQUAL(ptr, to.chars);

END_TEST


/* Test for hold routine.
 *
 */

START_TEST(string, string_hold)

    int rc = string_hold(NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    String str;
    memset(&str, 0, sizeof(str));
    rc = string_hold(&str);
    TEST_EQUAL(rc, RC_E_INVALID_STATE);

    /* pretend we have chars, but zero refcount */
    str.chars = (char *)test_chars;
    rc = string_hold(&str);
    TEST_EQUAL(rc, RC_E_INVALID_STATE);

    /* now add real string and recycler */
    str.chars = malloc(strlen(test_chars)+1);
    str.refcount = 1;
    str.recycler = free;
    rc = string_hold(&str);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(str.refcount, 2);

    /* check for counter saturation */
    str.refcount = STRING_REFCOUNT_MAX;
    rc = string_hold(&str);
    TEST_EQUAL(rc, RC_OK_NO_ACTION);
    TEST_EQUAL(str.refcount, STRING_REFCOUNT_MAX);

    /* release it */
    str.refcount = 1;
    rc = string_release(&str);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(str.refcount, 0);
    TEST_EQUAL(str.chars, NULL);

END_TEST

