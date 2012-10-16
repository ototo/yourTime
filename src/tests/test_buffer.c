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
    TEST_EQUAL(buf->tip_page_offset, 0);
    TEST_NOT_EQUAL(buf->head_page, NULL);
    TEST_EQUAL(buf->head_page, buf->tail_page);
    TEST_EQUAL(buf->head_page, buf->tip_page);

    SIGNAL_MARK;
    for (unsigned int offset = 0; offset < buf->page_size; ++offset)
        buf->head_page->data[offset] = 0xFF;

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
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    Buffer *buf = NULL;
    rc = buffer_resize(&buf, 0);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_resize(&buf, 16);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_alloc(&buf, 16);
    TEST_EQUAL(rc, RC_OK);

    rc = buffer_resize(&buf, 0);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_resize(&buf, 17);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf->size, 32);

    rc = buffer_resize(&buf, 32);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf->pages, 2);
    TEST_EQUAL(buf->size, 32);

    rc = buffer_resize(&buf, 1);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf->pages, 1);
    TEST_EQUAL(buf->size, 16);

    rc = buffer_resize(&buf, 128);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf->pages, 8);
    TEST_EQUAL(buf->size, 128);

    rc = buffer_free(&buf);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf, NULL);

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
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_write(NULL, test_chars, 0);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_write(NULL, NULL, test_chars_len);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    Buffer *buf = NULL;
    rc = buffer_write(&buf, NULL, 0);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_write(&buf, test_chars, 0);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_write(&buf, NULL, test_chars_len);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    rc = buffer_alloc(&buf, 16);
    TEST_EQUAL(rc, RC_OK);

    rc = buffer_write(&buf, NULL, 0);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_write(&buf, test_chars, 0);
    TEST_EQUAL(rc, RC_OK);

    rc = buffer_write(&buf, NULL, test_chars_len);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    rc = buffer_write(&buf, test_chars, test_chars_len);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf->used, test_chars_len);
    TEST_EQUAL(buf->tip, test_chars_len);
    TEST_EQUAL(buf->pages, 1);
    TEST_EQUAL(buf->tip_page, buf->head_page);
    TEST_EQUAL(buf->tip_page_offset, buf->tip);

    SIGNAL_MARK;
    rc = buffer_write(&buf, test_chars, test_chars_len);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf->used, test_chars_len * 2);
    TEST_EQUAL(buf->tip, test_chars_len * 2);
    TEST_EQUAL(buf->pages, 2);
    TEST_NOT_EQUAL(buf->tip_page, buf->head_page);
    TEST_EQUAL(buf->tip_page, buf->tail_page);
    TEST_EQUAL(buf->tip_page_offset, test_chars_len * 2 - buf->page_size);

    SIGNAL_MARK;
    for (int i = 0; i < 100; ++i) {
        rc = buffer_write(&buf, "A", 1);
        TEST_EQUAL_ITER(rc, RC_OK, i, 99);
    }
    int consumed_size = test_chars_len * 2 + 100;
    div_t r = div(consumed_size, buf->page_size);
    int expected_pages = r.quot + (r.rem ? 1 : 0);
    TEST_EQUAL(buf->pages, expected_pages);
    TEST_EQUAL(buf->used, consumed_size);
    TEST_EQUAL(buf->tip, buf->used);

    SIGNAL_MARK;
    rc = buffer_write(&buf, 
                     TEST_DATA_STRING TEST_DATA_STRING TEST_DATA_STRING,
                     test_chars_len * 3);
    TEST_EQUAL(rc, RC_OK);
    consumed_size += test_chars_len * 3;
    r = div(consumed_size, buf->page_size);
    expected_pages = r.quot + (r.rem ? 1 : 0);
    TEST_EQUAL(buf->pages, expected_pages);
    TEST_EQUAL(buf->used, consumed_size);
    TEST_EQUAL(buf->tip, buf->used);

    rc = buffer_free(&buf);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf, NULL);

END_TEST


/* Test for buffer writing routine using String.
 *
 */

START_TEST(buffer, buffer_write_string)

    int rc = buffer_write_string(NULL, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    String str;
    memset(&str, 0, sizeof(str));
    rc = buffer_write_string(NULL, &str);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    Buffer *buf = NULL;
    rc = buffer_write_string(&buf, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_write_string(&buf, &str);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    rc = string_allocate_static(&str, test_chars);
    TEST_EQUAL(rc, RC_OK);

    rc = buffer_write_string(NULL, &str);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_write_string(&buf, &str);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    rc = buffer_alloc(&buf, 16);
    TEST_EQUAL(rc, RC_OK);

    rc = buffer_write_string(&buf, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    SIGNAL_MARK;
    rc = buffer_write_string(&buf, &str);
    TEST_EQUAL(rc, RC_OK);

    /* As current implementation of buffer_write_string is just calling
     * buffer_write, further tests are covered by buffer:buffer_write
     * test.
     */

    rc = buffer_free(&buf);
    TEST_EQUAL(rc, RC_OK);

END_TEST


/* Test for buffer reading routine.
 *
 */

START_TEST(buffer, buffer_read)

    int rc = buffer_read(NULL, NULL, 0, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read(NULL, NULL, 16, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    unsigned int read = -1;
    rc = buffer_read(NULL, NULL, 16, &read);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);
    TEST_EQUAL(read, -1);

    SIGNAL_MARK;
    char chars[16];
    rc = buffer_read(NULL, chars, 0, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read(NULL, chars, 16, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read(NULL, chars, 16, &read);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);
    TEST_EQUAL(read, -1);

    SIGNAL_MARK;
    Buffer *buf = NULL;
    rc = buffer_read(&buf, NULL, 0, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read(&buf, NULL, 16, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read(&buf, NULL, 16, &read);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);
    TEST_EQUAL(read, -1);

    SIGNAL_MARK;
    rc = buffer_read(&buf, chars, 0, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read(&buf, chars, 16, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read(&buf, chars, 16, &read);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);
    TEST_EQUAL(read, -1);

    SIGNAL_MARK;
    rc = buffer_alloc(&buf, 16);
    TEST_EQUAL(rc, RC_OK);

    rc = buffer_read(&buf, NULL, 0, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read(&buf, NULL, 16, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read(&buf, NULL, 16, &read);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);
    TEST_EQUAL(read, -1);

    SIGNAL_MARK;
    rc = buffer_read(&buf, chars, 0, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read(&buf, chars, 0, &read);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(read, 0);

    rc = buffer_read(&buf, chars, 16, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read(&buf, chars, 16, &read);
    TEST_EQUAL(rc, RC_E_NOT_IMPLEMENTED);

    /* TODO: complete the test as soon as the functionality itself is
     * complete.
     */

    rc = buffer_free(&buf);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(buf, NULL);

END_TEST


/* Test for buffer reading routine using String.
 *
 */

START_TEST(buffer, buffer_read_string)

    int rc = buffer_read_string(NULL, NULL, 0, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read_string(NULL, NULL, 16, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    unsigned int read = -1;
    rc = buffer_read_string(NULL, NULL, 16, &read);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);
    TEST_EQUAL(read, -1);

    SIGNAL_MARK;
    String str;
    rc = buffer_read_string(NULL, &str, 0, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read_string(NULL, &str, 16, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read_string(NULL, &str, 16, &read);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);
    TEST_EQUAL(read, -1);

    SIGNAL_MARK;
    Buffer *buf = NULL;
    rc = buffer_read_string(&buf, NULL, 0, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read_string(&buf, NULL, 16, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read_string(&buf, NULL, 16, &read);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);
    TEST_EQUAL(read, -1);

    SIGNAL_MARK;
    rc = buffer_read_string(&buf, &str, 0, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read_string(&buf, &str, 16, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read_string(&buf, &str, 16, &read);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);
    TEST_EQUAL(read, -1);

    SIGNAL_MARK;
    rc = buffer_alloc(&buf, 16);
    TEST_EQUAL(rc, RC_OK);

    rc = buffer_read_string(&buf, NULL, 0, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read_string(&buf, NULL, 16, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read_string(&buf, NULL, 16, &read);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);
    TEST_EQUAL(read, -1);

    SIGNAL_MARK;
    rc = buffer_read_string(&buf, &str, 0, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read_string(&buf, &str, 0, &read);
    TEST_EQUAL(rc, RC_OK);
    TEST_EQUAL(read, 0);

    rc = buffer_read_string(&buf, &str, 16, NULL);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

    rc = buffer_read_string(&buf, &str, 16, &read);
    TEST_EQUAL(rc, RC_E_NOT_IMPLEMENTED);

    /* TODO: complete the test as soon as the functionality itself is
     * complete.
     */

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
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

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

    TEST_INCOMPLETE;

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
    TEST_EQUAL(buf->tip_page_offset, 2);
    TEST_EQUAL(buf->tip_page, buf->head_page);

    TEST_INCOMPLETE;

END_TEST


/* Test for buffer tip position quering routine.
 *
 */
START_TEST(buffer, buffer_tip)

    int rc = buffer_tip(NULL, NULL);
    TEST_EQUAL(rc, RC_E_NOT_IMPLEMENTED);

END_TEST


/* forward declarations for private functions */

int buffer_add_pages(Buffer **buffer, unsigned int pages,
                     bool zero_data);


/* Test for page allocation routine (private one).
 *
 */
START_TEST(buffer, buffer_add_pages)

    int rc = buffer_add_pages(NULL, 0, false);
    TEST_EQUAL(rc, RC_E_INVALID_ARGS);

END_TEST
