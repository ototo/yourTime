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
static int test_chars_len = sizeof(TEST_DATA_STRING) - 1;

/* Test for allocation routine.
 *
 */

START_TEST(buffer, buffer_alloc)

    int rc = buffer_alloc(NULL, 16);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    Buffer *buf;

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
    rc = buffer_alloc(&buf, 16);
    TEST_EQUAL(rc, RC_OK);
    TEST_NOT_EQUAL(buf, NULL);

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
    // TODO: complete


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
    for (int i = 0; i < 16; ++i) {
        rc = buffer_append(&buf, test_chars, test_chars_len);
        TEST_EQUAL(rc, RC_OK);
    }
    TEST_EQUAL(buf->pages, r.quot + (r.rem ? 1 : 0));
    rc = buffer_free(&buf);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf, NULL);

END_TEST


/* Test for buffer tip moving routine.
 *
 */

/* Seek buffer by offset or page+offset.
 *
 * Two modes are supported - seeking by a global offset in the buffer and
 * seeking by page number and intrapage offset.
 *
 * Parameters:
 *   @buffer         [in] buffer to seek;
 *   @seek_mode      [in] seek mode;
 *   @seek_offset    [in] offset (global or in-page depending on
 *                   seek_type);
 *   @seek_page      [in/out] page number to seek to (for page+offset
 *                   mode); updated after a successfull seek if not
 *                   NULL;
 *   @page           [out] page structure found or NULL if not;
 *   @page_offset    [out] intra-page offset after seek.
 *
 * Returns:
 *   RC_E_OK                for success;
 *   RC_E_OUT_OF_BOUNDS     when buffer is smaller then the seek
 *                          is requested;
 *   RC_E_CORRUPTION        when data integrity is broken.

int buffer_seek(Buffer **buffer, int seek_mode,
                   int seek_offset, int *seek_page,
                   BufferPage **page, int *page_offset)
 */
START_TEST(buffer, buffer_seek)

    int rc = buffer_seek(NULL, 0, 0, NULL, NULL, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    Buffer *buf = NULL;
    rc = buffer_alloc(16, &buf);
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
    // TODO: complete

END_TEST


/* Test for buffer trimming routine.
 *
 */

START_TEST(buffer, buffer_trim)

    TEST_NOT_IMPLEMENTED;

END_TEST


