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


/* forward declarations for private functions */

int buffer_add_pages(Buffer **buffer, unsigned int pages,
                     bool zero_data);


/* public API functions */

int buffer_alloc(Buffer **buffer, unsigned int page_size)
{
    if (!buffer || !page_size)
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


int buffer_resize(Buffer **buffer, unsigned int new_size)
{
    if (!buffer || !*buffer || !new_size)
        return RC_E_INVALID_ARGS;

    return RC_E_NOT_IMPLEMENTED;
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


int buffer_write(Buffer **buffer, const char *data, unsigned int size)
{
    if (!buffer || !*buffer || !data)
        return RC_E_INVALID_ARGS;

    if (!size)
        return RC_OK;

    return RC_E_NOT_IMPLEMENTED;
}


int buffer_write_string(Buffer **buffer, const String *str)
{
    if (!buffer || !*buffer || !str)
        return RC_E_INVALID_ARGS;

    if (!str->chars)
        return RC_OK;

    return RC_E_NOT_IMPLEMENTED;
}


int buffer_read(Buffer **buffer, char *data, unsigned int size,
                unsigned int *read)
{
    if (!buffer || !*buffer || !data || !read)
        return RC_E_INVALID_ARGS;

    if (!size) {
        *read = 0;
        return RC_OK;
    }

    return RC_E_NOT_IMPLEMENTED;
}


int buffer_read_string(Buffer **buffer, String *str, unsigned int size,
                       unsigned int *read)
{
    if (!buffer || !*buffer || !str || !read)
        return RC_E_INVALID_ARGS;

    if (!size) {
        *read = 0;
        return RC_OK;
    }

    return RC_E_NOT_IMPLEMENTED;
}


int buffer_get_as_string(Buffer **buffer, String *str)
{
    if (!buffer || !*buffer || !str)
        return RC_E_INVALID_ARGS;

    String new_str;

    if (!(*buffer)->used) {
        int rc = string_allocate_static(&new_str, "");
        if (rc != RC_OK)
            return rc;
        *str = new_str;
        return RC_OK;
    }

    int rc = string_allocate(&new_str, (*buffer)->used);
    if (rc != RC_OK)
        return rc;

    Buffer *buf = *buffer;
    BufferPage *page = buf->head;
    unsigned int chars_left = buf->used;
    char *copy_to = new_str.chars;
    div_t r = div(buf->used, buf->page_size);
    unsigned int chunk_size;
    while (chars_left) {
        if (!page)
            return RC_E_INVALID_STATE;

        chunk_size = (chars_left > buf->page_size)
                   ? buf->page_size
                   : r.rem;
        memcpy(copy_to, page->data, chunk_size);
        page = page->next;
        chars_left -= chunk_size;
        copy_to += chunk_size;
    }
    *copy_to = '\0';
    *str = new_str;

    return RC_OK;
}

int buffer_append(Buffer **buffer, const char *string, unsigned int size)
{
    if (!buffer || !*buffer)
        return RC_E_INVALID_ARGS;

    if (!string || !size)
        return RC_OK;

    Buffer *buf = *buffer;
    unsigned int left_to_append = size;
    const char *chars_to_append = string;

    while (left_to_append) {
        if (!buf->head || (buf->used == buf->size)) {
            buffer_add_pages(buffer, 1, 0);
            continue;
        }
        else {
            div_t r = div(buf->used, buf->page_size);
            unsigned int available = buf->page_size - r.rem;
            unsigned int to_copy = (available < left_to_append)
                        ? available
                        : left_to_append;
            memcpy(buf->tail->data + r.rem, chars_to_append, to_copy);
            left_to_append -= to_copy;
            chars_to_append += to_copy;
            buf->used += to_copy;
        }
    }

    return RC_OK;
}


int buffer_append_string(Buffer **buffer, String *str)
{
    return RC_E_NOT_IMPLEMENTED;
}


int buffer_used(Buffer **buffer, unsigned int *used)
{
    if (!buffer || !used)
        return RC_E_INVALID_ARGS;

    if (!*buffer)
        *used = 0;
    else
        *used = (*buffer)->used;

    return RC_OK;
}


int buffer_allocated(Buffer **buffer, unsigned int *allocated)
{
    return RC_E_NOT_IMPLEMENTED;
}


int buffer_seek(Buffer **buffer, int seek_mode,
                   unsigned int seek_offset, unsigned int *seek_page,
                   BufferPage **page, unsigned int *page_offset)
{
    if (!buffer)
        return RC_E_INVALID_ARGS;

    unsigned int target_page;
    unsigned int offset;

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

        unsigned int current_page = 0;
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
    unsigned int idx;

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


int buffer_tip(Buffer **buffer, unsigned int *buffer_offset)
{
    return RC_E_NOT_IMPLEMENTED;
}



/**********************************************************************
 * Internal routines                                                  *
 **********************************************************************/

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

int buffer_add_pages(Buffer **buffer, unsigned int pages,
                     bool zero_data)
{
    if (!buffer || !*buffer || !pages)
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


