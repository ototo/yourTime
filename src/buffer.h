/* yourTime
 *
 * Buffer implementation
 *
 * TODO: complete header (license, author, etc)
 */

#pragma once

#include "string.h"


#define BUF_SEEK_PAGE_OFFSET        0
#define BUF_SEEK_PAGE_REL_OFFSET    1
#define BUF_SEEK_BUFFER_OFFSET      2
#define BUF_SEEK_BUFFER_REL_OFFSET  3

/* paged buffer structures */
struct _BufferPage;
typedef struct _BufferPage BufferPage;


/* Page of buffer.
 *
 * Used to allocate memory for a Buffer as needed.
 */
struct _BufferPage
{
    BufferPage  *next;      /* next page moving from head to tail */
    char        data[];     /* data buffer itself */
};


/* Buffer of bytes.
 *
 * Allows free reading/writing of data.
 */
struct _Buffer
{
    int         page_size;          /* size of a page */
    int         pages;              /* number of allocated pages */
    int         size;               /* allocated total size (all pages) */
    int         used;               /* total used size (payload) */
    BufferPage  *head;              /* the oldest page */
    BufferPage  *tail;              /* most recently added page */
    BufferPage  *tip;               /* the page containing the byte,
                                       referred by the @tip_offset */
    int         tip_offset;         /* next byte to be written */
};

typedef struct _Buffer Buffer;


/* Allocate new Buffer.
 *
 * Allocates new buffer with just one page pre-allocated.
 *
 * Parameters:
 *   @buffer        [out] newly allocated buffer.
 *   @page_size     [in] size of the data page (just the data part);
 *
 * Returns:
 *   RC_OK                  on success.
 *   RC_E_INVALID_ARGS      when arguments provided are invalid.
 *   RC_E_OUT_OF_MEMORY     when there is no memory available.
 */
int buffer_alloc(Buffer **buffer, int page_size);


/* Change buffer size.
 *
 * Increases/decreases the size of the buffer by adding/removing pages
 * and adjusting the tip accordingly.
 *
 * Parameters:
 *   @buffer        [in/out] buffer to modify;
 *   @new_size      [in] new size of the buffer or 0 to trim it to the
 *                  size of the data contained.
 *
 * Returns:
 *   RC_OK                  on success.
 *   RC_E_INVALID_ARGS      when arguments provided are invalid.
 *   RC_E_OUT_OF_MEMORY     when there is no memory available.
 */
int buffer_resize(Buffer **buffer, int new_size);


/* Free a buffer.
 *
 * Deallocates a buffer (if not already freed).
 *
 * Parameters:
 *   @buffer        [in/out] buffer to free.
 *
 * Returns:
 *   RC_OK                  on success.
 *   RC_E_INVALID_ARGS      when arguments provided are invalid.
 *   RC_E_OUT_OF_MEMORY     when there is no memory available.
 */
int buffer_free(Buffer **buffer);

int buffer_write(Buffer **buffer, const char *data, int size);
int buffer_write_string(Buffer **buffer, const String str);
int buffer_read(Buffer **buffer, char *data, int size);
int buffer_read_string(Buffer **buffer, String *str, int size);
int buffer_get_as_string(Buffer **buffer, String *str);
int buffer_append(Buffer **buffer, const char *string, int size);
int buffer_append_string(Buffer **buffer, String *str);

int buffer_used(Buffer **buffer, int *used);
int buffer_allocated(Buffer **buffer, int *allocated);

int buffer_seek(Buffer **buffer, int seek_mode,
                   int seek_offset, int *seek_page,
                   BufferPage **page, int *page_offset);
int buffer_tip(Buffer **buffer, int *buffer_offset);
