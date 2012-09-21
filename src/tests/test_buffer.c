/* yourTime
 *
 * Buffer test set file.
 *
 * TODO: complete header (license, author, etc)
 */

#define _XOPEN_SOURCE 600

#include "test.h"
#include "../buffer.h"

/* data for tests */
static const char const *test_chars = "test chars";
static int test_chars_len = sizeof(test_chars) - 1;

/* Test for allocation routine.
 *
 */

START_TEST(buffer, buffer_alloc)

    Buffer *buf;

    SIGNAL_MARK;
    int rc = buffer_alloc(16, &buf);
    TEST_EQUAL(rc, RC_OK);
    TEST_NOT_EQUAL(buf, NULL);
    TEST_EQUAL(buf->page_size, 16);
    TEST_EQUAL(buf->pages, 1);
    TEST_EQUAL(buf->size, 16);
    TEST_NOT_EQUAL(buf->head, NULL);
    TEST_EQUAL(buf->head, buf->tail);

    SIGNAL_MARK;
    rc = buffer_free(&buf);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf, NULL);

END_TEST


/* Test for data appending routine.
 *
 */

START_TEST(buffer, buffer_append)

    int rc = buffer_append(NULL, NULL, 0);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

END_TEST


/* Test for used space aize query routine.
 *
 */

START_TEST(buffer, buffer_used)

    int rc = buffer_used(NULL, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    int used = -1;
    rc = buffer_used(NULL, &used);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);
    TEST_EQUAL(used, -1);

    SIGNAL_MARK;
    Buffer *buf = NULL;
    rc = buffer_used(&buf, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    rc = buffer_used(&buf, &used);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(used, 0);

    SIGNAL_MARK;
    rc = buffer_alloc(16, &buf);
    TEST_EQUAL(rc, RC_OK);

    used = -1;
    rc = buffer_used(&buf, &used);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(used, 0);

    SIGNAL_MARK;
    rc = buffer_append(&buf, test_chars, test_chars_len);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf->used, test_chars_len);

    rc = buffer_free(&buf);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf, NULL);

END_TEST


/* Test for getting buffer contents a C string routine.
 *
 */

START_TEST(buffer, buffer_get_as_string)

    TEST_NOT_IMPLEMENTED;

END_TEST


/* Test for string deallocation routine.
 *
 */

START_TEST(buffer, buffer_free_string)

    TEST_NOT_IMPLEMENTED;

END_TEST


/* Test for buffer tip moving routine.
 *
 */

START_TEST(buffer, buffer_seek)

    TEST_NOT_IMPLEMENTED;

END_TEST


/* Test for buffer trimming routine.
 *
 */

START_TEST(buffer, buffer_trim)

    TEST_NOT_IMPLEMENTED;

END_TEST


/* Test for deallocation routine.
 *
 */

START_TEST(buffer, buffer_free)

    SIGNAL_MARK;
    int rc = buffer_free(NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    Buffer *buf = NULL;
    rc = buffer_free(&buf);
    TEST_EQUAL(rc, RC_OK);

    SIGNAL_MARK;
    rc = buffer_alloc(16, &buf);
    TEST_EQUAL(rc, RC_OK);
    TEST_NOT_EQUAL(buf, NULL);

    rc = buffer_free(&buf);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf, NULL);

END_TEST


