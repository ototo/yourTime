/* yourTime
 *
 * Buffer test set file.
 *
 * TODO: complete header (license, author, etc)
 */

#define _XOPEN_SOURCE 600

#include <string.h>

#include "test.h"
#include "../buffer.h"


/* data for tests */
#define TEST_DATA_STRING "test chars"
static const char const *test_chars = TEST_DATA_STRING;
static unsigned int test_chars_len = sizeof(TEST_DATA_STRING) - 1;

/* Test for allocation routine.
 *
 */

START_TEST(buffer, buffer_alloc)

    int rc = buffer_alloc(NULL, 0);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_alloc(NULL, 16);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    Buffer *buf;

    rc = buffer_alloc(&buf, 0);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;

    rc = buffer_alloc(&buf, 16);
    TEST_EQUAL(rc, RC_OK);
    TEST_NOT_EQUAL(buf, NULL);
    TEST_EQUAL(buf->page_size, 16);
    TEST_EQUAL(buf->pages, 1);
    TEST_EQUAL(buf->size, 16);
    TEST_EQUAL(buf->tip_offset, 0);
    TEST_NOT_EQUAL(buf->head, NULL);
    TEST_EQUAL(buf->head, buf->tail);
    TEST_EQUAL(buf->head, buf->tip);

    SIGNAL_MARK;
    rc = buffer_free(&buf);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf, NULL);

END_TEST


/* Test for resizing routine.
 *
 */

START_TEST(buffer, buffer_resize)

    int rc = buffer_resize(NULL, 0);
    TEST_EQUAL(rc, RC_E_NOT_IMPLEMENTED);

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
    rc = buffer_alloc(&buf, 16);
    TEST_EQUAL(rc, RC_OK);
    TEST_NOT_EQUAL(buf, NULL);

    rc = buffer_free(&buf);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf, NULL);

END_TEST


/* Test for buffer writing routine.
 *
 */

START_TEST(buffer, buffer_write)

    int rc = buffer_write(NULL, NULL, 0);
    TEST_EQUAL(rc, RC_E_NOT_IMPLEMENTED);

END_TEST


/* Test for buffer writing routine using String.
 *
 */

START_TEST(buffer, buffer_write_string)

    int rc = buffer_write_string(NULL, NULL);
    TEST_EQUAL(rc, RC_E_NOT_IMPLEMENTED);

END_TEST


/* Test for buffer reading routine.
 *
 */

START_TEST(buffer, buffer_read)

    int rc = buffer_read(NULL, NULL, 0, NULL);
    TEST_EQUAL(rc, RC_E_NOT_IMPLEMENTED);

END_TEST


/* Test for buffer reading routine using String.
 *
 */

START_TEST(buffer, buffer_read_string)

    int rc = buffer_read_string(NULL, NULL, 0, NULL);
    TEST_EQUAL(rc, RC_E_NOT_IMPLEMENTED);

END_TEST


/* Test for getting buffer contents a C string routine.
 *
 */

START_TEST(buffer, buffer_get_as_string)

    int rc = buffer_get_as_string(NULL, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    Buffer *buf = NULL;
    rc = buffer_get_as_string(&buf, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    String str;
    rc = buffer_get_as_string(NULL, &str);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    rc = buffer_get_as_string(&buf, &str);
    TEST_EQUAL(rc, RC_E_INVALID_STATE);

    SIGNAL_MARK;
    rc = buffer_alloc(&buf, 16);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf->pages, 1);
    TEST_EQUAL(buf->size, 16);
    rc = buffer_append(&buf, test_chars, test_chars_len);
    TEST_EQUAL(rc, RC_OK);
    rc = buffer_append(&buf, test_chars, test_chars_len);
    TEST_EQUAL(rc, RC_OK);
    rc = buffer_get_as_string(&buf, &str);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(0, strcmp(str.chars, TEST_DATA_STRING TEST_DATA_STRING));
    rc = string_release(&str);
    TEST_EQUAL(rc, RC_OK);
    rc = buffer_free(&buf);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf, NULL);

    SIGNAL_MARK;
    rc = buffer_alloc(&buf, 3);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf->pages, 1);
    TEST_EQUAL(buf->size, 3);
    div_t r = div(test_chars_len * 16, 3);
    for (unsigned int i = 0; i < 16; ++i) {
        rc = buffer_append(&buf, test_chars, test_chars_len);
        TEST_EQUAL(rc, RC_OK);
    }
    TEST_EQUAL(buf->pages, r.quot + (r.rem ? 1 : 0));
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

    SIGNAL_MARK;
    rc = buffer_append(NULL, test_chars, test_chars_len);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    Buffer *buf = NULL;
    rc = buffer_append(&buf, test_chars, test_chars_len);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    rc = buffer_alloc(&buf, 16);
    TEST_EQUAL(rc, RC_OK);
    rc = buffer_append(&buf, NULL, test_chars_len);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf->used, 0);
    rc = buffer_append(&buf, test_chars, 0);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf->used, 0);

    SIGNAL_MARK;
    rc = buffer_append(&buf, test_chars, test_chars_len);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf->used, test_chars_len);
    TEST_EQUAL(buf->size, buf->page_size);
    TEST_EQUAL(buf->pages, 1);

    SIGNAL_MARK;
    rc = buffer_append(&buf, test_chars, test_chars_len);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf->used, test_chars_len * 2);
    TEST_EQUAL(buf->size, buf->page_size * 2);
    TEST_EQUAL(buf->pages, 2);

    bool is_test_complete = false;
    TEST_TRUE(is_test_complete);

END_TEST


/* Test for data appending routine using String.
 *
 */

START_TEST(buffer, buffer_append_string)

    int rc = buffer_append_string(NULL, NULL);
    TEST_EQUAL(rc, RC_E_NOT_IMPLEMENTED);

END_TEST


/* Test for used space size query routine.
 *
 */

START_TEST(buffer, buffer_used)

    int rc = buffer_used(NULL, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    unsigned int used = -1;
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
    rc = buffer_alloc(&buf, 16);
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


/* Test for allocated space size query routine.
 *
 */

START_TEST(buffer, buffer_allocated)

    int rc = buffer_allocated(NULL, NULL);
    TEST_EQUAL(rc, RC_E_NOT_IMPLEMENTED);

END_TEST


/* Test for buffer tip moving routine.
 *
 */
START_TEST(buffer, buffer_seek)

    int rc = buffer_seek(NULL, 0, 0, NULL, NULL, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    Buffer *buf = NULL;
    rc = buffer_alloc(&buf, 16);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf->size, 16);

    SIGNAL_MARK;
    rc = buffer_seek(&buf, -1, 0, NULL, NULL, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    /******** seek by buffer offset ********/
    SIGNAL_MARK;
    rc = buffer_seek(&buf, BUF_SEEK_BUFFER_OFFSET, 2, NULL, NULL, NULL);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf->tip_offset, 2);
    TEST_EQUAL(buf->tip, buf->head);

    bool is_test_complete = false;
    TEST_TRUE(is_test_complete);

END_TEST


/* Test for buffer tip position quering routine.
 *
 */

START_TEST(buffer, buffer_tip)

    int rc = buffer_tip(NULL, NULL);
    TEST_EQUAL(rc, RC_E_NOT_IMPLEMENTED);

END_TEST


