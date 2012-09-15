/* yourTime
 *
 * String test set file.
 *
 * TODO: complete header (license, author, etc)
 */

#define _XOPEN_SOURCE 600

#include "test.h"
#include "../string.h"


/* Test for allocation routine.
 *
 */

START_TEST(string, string_allocate)

    int rc = string_allocate(0, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = string_allocate(1, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    String *str = 0x12345678;

    rc = string_allocate(0, str);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);
    TEST_EQUAL(str, 0x12345678);

    str = NULL;
    rc = string_allocate(256, str);
    TEST_EQUAL(rc, RC_OK);
    TEST_NOT_EQUAL(str, NULL);
    TEST_EQUAL(str->recycler, free);
    TEST_EQUAL(str->refcount, 1);
    TEST_NOT_EQUAL(str->chars, NULL);

    rc = string_release(str);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(str->refcount, 0);
    TEST_EQUAL(str->chars, NULL);

END_TEST


/* Test for static allocation routine.
 *
 */

START_TEST(string, string_allocate_static)

    TEST_NOT_IMPLEMENTED;

END_TEST


/* Test for deallocation routines.
 *
 */

START_TEST(string, string_release)

    TEST_NOT_IMPLEMENTED;

END_TEST


/* Test for copy routine.
 *
 */

START_TEST(string, string_copy)

    TEST_NOT_IMPLEMENTED;

END_TEST


/* Test for hold routine.
 *
 */

START_TEST(string, string_hold)

    TEST_NOT_IMPLEMENTED;

END_TEST


