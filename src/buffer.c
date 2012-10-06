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
#include "string.h"
#include "buffer.h"


/* forward declarations */
static int buffer_add_pages(Buffer **buffer, int pages, int zero_data);


int buffer_alloc(Buffer **buffer, int page_size)
{
    if (!buffer)
        return RC_E_INVALID_ARGS;

    Buffer *buf;

    buf = (Buffer *)malloc(sizeof(Buffer));
    if (!buf)
        return RC_E_OUT_OF_MEMORY;

    memset(buf, 0, sizeof(Buffer));

    buf->page_size = page_size;

    int rc = buffer_add_pages(&buf, 1, 0);
    if (rc) {
        free(buf);
        return rc;
    }

    *buffer = buf;

    return RC_OK;
}


int buffer_free(Buffer **buffer)
{
    if (!buffer)
        return RC_E_INVALID_ARGS;

    if (!*buffer)
        return RC_OK;

    BufferPage *current;
    BufferPage *page = (*buffer)->head;
    while (page) {
        current = page;
        page = page->next;
        free(current);
    }
    free(*buffer);
    *buffer = NULL;

    return RC_OK;
}


int buffer_resize(Buffer **buffer, int new_size)
{
    return RC_E_NOT_IMPLEMENTED;
}


/* Add pages to the buffer.
 *
 * Appends pages to the buffer and
 *
 * Parameters:
 *   @buffer        [in/out] buffer to work on;
 *   @pages         [in] number of pages to add;
 *   @zero_data     [in] zero newly allocated pages if != 0.
 *
 * Returns:
 *   RC_E_OK                for success;
 *   RC_E_OUT_OF_MEMORY     when there is no memory available;
 *   RC_E_CORRUPTION        when data integrity is broken.
 */

static
int buffer_add_pages(Buffer **buffer, int pages, int zero_data)
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
            buf->head = page;
        else
            buf->tail->next = page;
        buf->tail = page;
        buf->pages += 1;
        buf->size += buf->page_size;
    }

    if (!buf->tip)
        buf->tip = buf->head;

    // when adding pages need to adjust the tip pointer as it points to
    // the next available byte in the buffer (except for the case, when
    // all the allocated pages are used up).
    if (buf->tip_offset >= buf->page_size) {
        buf->tip = buf->tip->next;
        buf->tip_offset = 0;
    }

    return RC_OK;
}



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
        if (!seek_page)
            return RC_E_INVALID_ARGS;

        int current_page = 0;
        for (BufferPage *page = buf->head; page; page = page->next) {
            if (page == buf->tip)
                break;
            ++current_page;
        }
        target_page = current_page + *seek_page;
        offset = seek_offset;
        break;

    case BUF_SEEK_BUFFER_OFFSET:
        {
            div_t q = div(seek_offset, buf->page_size);
            target_page = q.quot;
            offset = q.rem;
        }
        break;

    case BUF_SEEK_BUFFER_REL_OFFSET:
        {
            div_t q = div(buf->used + seek_offset, buf->page_size);
            target_page = q.quot;
            offset = q.rem;
        }
        break;

    default:
        return RC_E_INVALID_ARGS;
    }

    BufferPage *p = buf->head;
    int idx;

    for (idx = 0; p && idx < target_page; p = p->next, ++idx) ;

    if (idx != target_page)
        return RC_E_OUT_OF_BOUNDS;

    buf->tip = p;
    buf->tip_offset = offset;

    if (seek_page)
        *seek_page = target_page;
    if (page)
        *page = p;
    if (page_offset)
        *page_offset = offset;

    return RC_OK;
}


int buffer_append(Buffer **buffer, const char *string, int size)
{
    if (!buffer || !*buffer)
        return RC_E_INVALID_ARGS;

    if (!string || !size)
        return RC_OK;

    Buffer *buf = *buffer;
    int left_to_append = (size > 0) ? size : strlen(string);
    const char *chars_to_append = string;

    while (left_to_append) {
        if (!buf->head || (buf->used == buf->size)) {
            buffer_add_pages(buffer, 1, 0);
            continue;
        }
        else {
            if (buf->tip_offset == buf->page_size) {
                if (!buf->tip->next)
                    return RC_E_INVALID_STATE;
                else
                    buf->tip = buf->tip->next;
            }

            int available = buf->page_size - buf->tip_offset;
            int to_copy = (available < left_to_append)
                        ? available
                        : left_to_append;
            memcpy(buf->tip->data + buf->tip_offset, chars_to_append, to_copy);
            left_to_append -= to_copy;
            chars_to_append += to_copy;
            buf->tip_offset += to_copy;
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


int buffer_get_as_string(Buffer **buffer, String *string)
{
    if (!buffer || !string)
        return RC_E_INVALID_ARGS;

    if (!*buffer)
        return RC_E_INVALID_STATE;

    if (!(*buffer)->used) {
        String str;
        int rc = string_allocate_static("", &str);
        if (rc != RC_OK)
            return rc;
        *string = str;
        return RC_OK;
    }

    String str;
    int rc = string_allocate((*buffer)->used, &str);
    if (rc != RC_OK)
        return rc;

    int page_size = (*buffer)->page_size;
    BufferPage *last = (*buffer)->tip;
    char *to = str.chars;

    for (BufferPage *page = (*buffer)->head; page; page = page->next) {
        if (page == last) {
            memcpy(to, page->data, (*buffer)->tip_offset);
            to += (*buffer)->tip_offset;
        }
        else {
            memcpy(to, page->data, page_size);
            to += page_size;
        }
    }
    *to = '\0';
    *string = str;

    return RC_OK;
}

int buffer_trim(Buffer **buffer, int new_size)
{
    assert(*buffer);

    if (!*buffer)
        return RC_E_INVALID_ARGS;

    return RC_OK;
}
