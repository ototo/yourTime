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

#include "error.h"
#include "buffer.h"


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
 */

int buffer_seek(Buffer **buffer, int seek_mode,
                   int seek_offset, int *seek_page,
                   BufferPage **page, int *page_offset)
{
    if (!buffer)
        return RC_E_INVALID_ARGS;

    int target_page;
    int offset;

    Buffer *buf = *buffer;

    switch (seek_mode)
    {
    case BUF_SEEK_PAGE_OFFSET:
        if (!seek_page)
            return RC_E_INVALID_ARGS;
        target_page = *seek_page;
        offset = seek_offset;
        break;

    case BUF_SEEK_PAGE_REL_OFFSET:
        // TODO: implement
        break;

    case BUF_SEEK_BUFFER_OFFSET:
        {
            div_t q = div(seek_offset, buf->page_size);
            target_page = q.quot;
            offset = q.rem;
        }
        break;

    case BUF_SEEK_BUFFER_REL_OFFSET:
        // TODO: implement
        break;
    }

    BufferPage *p = buf->tail;
    int idx;

    for (idx = 0; p && idx < target_page; p = p->next, ++idx) ;

    if (idx != target_page)
        return RC_E_OUT_OF_BOUNDS;

    if (seek_page)
        *seek_page = target_page;
    if (page)
        *page = p;
    if (page_offset)
        *page_offset = offset;

    return RC_OK;
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
 *   RC_E_OK                for success;
 *   RC_E_OUT_OF_MEMORY     when there is no memory available;
 *   RC_E_CORRUPTION        when data integrity is broken.
 */

static
int buffer_add_pages(Buffer **buffer, int pages,
                        int used, int zero_data)
{
    if (!buffer && !*buffer)
        return RC_E_INVALID_ARGS;

    Buffer *buf = *buffer;
    if (!buf->page_size)
        return RC_E_INVALID_STATE;

    int page_size = buf->page_size;

    for (int page_nr = 0; page_nr < pages; ++page_nr) {
        BufferPage *page = malloc(sizeof(BufferPage) + page_size);
        if (!page)
            return RC_E_OUT_OF_MEMORY;

        memset(page, 0, sizeof(BufferPage) + (zero_data ? page_size : 1));

        if (!buf->head)
            buf->tail = page;
        else
            page->next = buf->head;
        buf->head = page;
        buf->pages += 1;
        buf->size += buf->page_size;
    }

    if (used < 0)
        return RC_OK;

    // TODO: seek to the right page/offset
    BufferPage *page = NULL;
    int page_offset = -1;

    int rc = buffer_seek(buffer, BUF_SEEK_BUFFER_OFFSET,
                         used, NULL, &page, &page_offset);
    if (rc != RC_OK)
        return rc;

    buf->used = used;
    buf->tip = page;
    buf->tip_page_used = page_offset;

    return RC_OK;
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
 *   RC_E_OK                on success;
 *   RC_E_OUT_OF_MEMORY     when there is no memory available.
 */

int buffer_alloc(int page_size, Buffer **buffer)
{
    Buffer *buf;

    buf = (Buffer *)malloc(sizeof(Buffer));
    if (!buf)
        return RC_E_OUT_OF_MEMORY;

    memset(buf, 0, sizeof(Buffer));

    buf->page_size = page_size;

    int rc = buffer_add_pages(&buf, 1, 0, 0);
    if (rc) {
        free(buf);
        return rc; // TODO: report error
    }

    *buffer = buf;

    return RC_OK;
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
 *   RC_E_OK                on success;
 *   RC_E_ARGUMENTS         invalid arguments are provided;
 *   RC_E_OUT_OF_MEMORY     when there is no memory available.
 */

int buffer_append(Buffer **buffer, const char *string, int size)
{
    if (!buffer || !*buffer)
        return RC_E_INVALID_ARGS;

    if (!string || !size)
        return RC_OK;

    Buffer *buf = *buffer;
    int string_len = strlen(string);
    int left_to_append = (size > 0) ? size : string_len;
    const char *chars_to_append = string;

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

    return RC_OK;
}


int buffer_used(Buffer **buffer, int *used)
{
    if (!buffer || !used)
        return RC_E_INVALID_ARGS;

    if (!*buffer)
        *used = 0;
    else
        *used = (*buffer)->used;

    return RC_OK;
}


/* Get a string from buffer contents.
 *
 * Creates a zero-terminated string from all the contents of the buffer.
 *
 * Parameters:
 *   @buffer        [in] buffer to work on;
 *   @string        [out] String containing buffer data.
 *
 * Returns:
 *   RC_OK          on success;
 */

int buffer_get_as_string(Buffer **buffer, String **string)
{
    if (!buffer || !string)
        return RC_E_INVALID_ARGS;

    if (!*buffer)
        return RC_E_INVALID_STATE;

    return RC_OK;
}

int buffer_trim(Buffer **buffer, int new_size)
{
    assert(*buffer);

    if (!*buffer)
        return RC_E_INVALID_ARGS;

    return RC_OK;
}

int buffer_free(Buffer **buffer)
{
    if (!buffer)
        return RC_E_INVALID_ARGS;

    if (!*buffer)
        return RC_OK;

    BufferPage *current;
    BufferPage *page = (*buffer)->tail;
    while (page) {
        current = page;
        page = page->next;
        free(current);
    }
    free(*buffer);
    *buffer = NULL;

    return RC_OK;
}

