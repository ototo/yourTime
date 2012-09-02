/* yourTime
 *
 * Buffer implementation
 *
 * TODO: complete header (license, author, etc)
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"


/* Seek buffer by offset or page+offset.
 *
 * Two modes are supported - seeking by a global offset in the buffer and
 * seeking by page number and intrapage offset.
 *
 * Parameters:
 *   @buffer         [in] buffer to seek;
 *   @seek_mode      [in] seek mode;
 *   @seek_offset    [in] offset (global or in-page depending on seek_type);
 *   @seek_page      [in/out] page number to seek to (for page+offset mode);
 *                            updated after a successfull seek;
 *   @page           [out] page structure found or NULL if not;
 *   @page_offset    [out] intra-page offset after seek.
 *
 * Returns:
 *   YTE_OK              for success;
 *   YTE_OUT_OF_BOUNDS   when buffer is smaller then the seek
 *                       requested;
 *   YTE_CORRUPTION      when data integrity is broken.
 */

int buffer_seek(Buffer *buffer, int seek_mode,
                   int seek_offset, int *seek_page,
                   BufferPage **page, int *page_offset)
{
    assert(buffer);

    int target_page;
    int offset;

    switch (seek_mode)
    {
    case BUF_SEEK_PAGE_OFFSET:
        target_page = *seek_page;
        offset = seek_offset;
        break;
    case BUF_SEEK_PAGE_REL_OFFSET:
        // TODO: implement
        break;
    case BUF_SEEK_BUFFER_OFFSET:
        {
            div_t q = div(seek_offset, buffer->page_size);
            target_page = q.quot;
            offset = q.rem;
        }
        break;
    case BUF_SEEK_BUFFER_REL_OFFSET:
        // TODO: implement
        break;
    }

    BufferPage *p = buffer->tail;
    int idx;

    for (idx = 0; p && idx < target_page; p = p->next, ++idx) ;

    if (idx != target_page)
        return -1; // TODO: YTE_OUT_OF_BOUNDS

    *seek_page = target_page;
    *page = p;
    *page_offset = offset;

    return 0; // TODO: YTE_OK
}


/* Add pages to the buffer.
 *
 * Appends pages to the buffer and
 *
 * Parameters:
 *   @buffer        [in/out] buffer to work on;
 *   @pages         [in] number of pages to add;
 *   @used          [in] new value for used bytes counter;
 *                       counter does not get updated if -1 is passed;
 *   @zero_data     [in] zero newly allocated pages if != 0.
 *
 * Returns:
 *   YTE_OK             for success;
 *   YTE_OUT_OF_MEMORY  when there is no memory available;
 *   YTE_CORRUPTION     when data integrity is broken.
 */

static
int buffer_add_pages(Buffer **buffer, int pages,
                        int used, int zero_data)
{
    assert(buffer);

    if (!*buffer)
        return -1; /* TODO: error code */

    Buffer *buf = *buffer;
    if (!buf->page_size) {
        fprintf(stderr,
                "%s - zero page size is a nonsense!\n", __func__);
        return -1; // TODO: error code
    }

    int page_size = buf->page_size;

    for (int page_nr = 0; page_nr < pages; ++page_nr) {
        BufferPage *page = malloc(sizeof(BufferPage) + page_size);
        if (!page)
            return -1; // TODO: error code

        memset(page, 0, sizeof(BufferPage) + (zero_data ? page_size : 1));

        if (!buf->head)
            buf->tail = page;
        else
            buf->head->next = page;
        buf->pages += 1;
        buf->size += buf->page_size;
        buf->head = page;
    }
    // TODO: set used

    return 0;
}


/* Allocate new Buffer.
 *
 * Allocates new buffer with just one page pre-allocated.
 *
 * Parameters:
 *   @page_size     [in] size of the data page (just the data part);
 *   @buffer        [out] newly allocated buffer.
 *
 * Returns:
 *   YTE_OK             on success;
 *   YTE_OUT_OF_MEMORY  when there is no memory available.
 */

int buffer_alloc(int page_size, Buffer **buffer)
{
    Buffer *buf;

    buf = (Buffer *)malloc(sizeof(Buffer));
    if (!buf)
        return -1; /* TODO: error code */

    memset(buf, 0, sizeof(Buffer));

    buf->page_size = page_size;

    int rc = buffer_add_pages(&buf, 1, 0, 0);
    if (rc) {
        free(buf);
        return rc; // TODO: report error
    }

    *buffer = buf;

    return 0;
}


/* Append string to the buffer.
 *
 * Appends a zero-terminated string to the buffer.
 *
 * Parameters:
 *   @buffer        [in/out] buffer to operate on;
 *   @string        [in] string to add to the buffer;
 *   @size          [in] counter of characters to be added;
 *                       if negative, then till zero terminator.
 *
 * Returns:
 *   YTE_OK             on success;
 *   YTE_ARGUMENTS      invalid arguments are provided;
 *   YTE_OUT_OF_MEMORY  when there is no memory available.
 */

int buffer_append(Buffer **buffer, char *string, int size)
{
    assert(buffer);

    if (!*buffer)
        return -1; /* TODO: error code */

    if (!string || !size)
        return 0;

    Buffer *buf = *buffer;
    int string_len = strlen(string);
    int left_to_append = (size > 0)
                       ? ((size < string_len) ? size : string_len)
                       : string_len;
    char *chars_to_append = string;

    while (left_to_append) {
        BufferPage *head = buf->head;
        if (!head || (buf->tip_page_used == buf->page_size)) {
            buffer_add_pages(buffer, 1, -1, 0);
            continue;
        }
        else {
            int available = buf->page_size - buf->tip_page_used;
            int to_copy = (available < left_to_append)
                        ? available
                        : left_to_append;
            memcpy(head->data, chars_to_append, to_copy);
            left_to_append -= to_copy;
            chars_to_append += to_copy;
            buf->tip_page_used += to_copy;
            buf->used += to_copy;
        }
    }

    return 0;
}

/* Get a string from buffer contents.
 *
 * Creates a zero-terminated string from all the contents of the buffer.
 *
 * Parameters:
 *   @buffer        [in] buffer to work on;
 *   @string        [out] string containing buffer data;
 *                        to be freed by buffer_free_string.
 *
 * Returns:
 *   YTE_OK             on success;
 */

int buffer_get_as_string(Buffer **buffer, char **string)
{
    assert(buffer);

    return 0;
}

int buffer_free_string(char **string)
{
    assert(string);

    if (!*string)
        return 0; // TODO: result code

    free(*string);
    *string = NULL;

    return 0;
}

int buffer_trim(Buffer **buffer, int new_size)
{
    assert(*buffer);

    if (!*buffer)
        return -1; // TODO: error code

    return 0;
}

int buffer_free(Buffer **buffer)
{
    assert(buffer);

    if (!*buffer)
        return 0;

    BufferPage *current;
    BufferPage *page = (*buffer)->tail;
    while (page) {
        current = page;
        page = page->next;
        free(current);
    }
    free(buffer);

    return 0;
}

