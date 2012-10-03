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

struct _BufferPage
{
    BufferPage  *next;      /* next page moving from head to tail */
    char        data[];     /* data buffer itself */
};

struct _Buffer
{
    int         page_size;          /* page size */
    int         pages;              /* number of allocated pages */
    int         size;               /* allocated total size (all pages) */
    int         used;               /* total used size */
    int         tip_page_used;      /* used on the tip page */
    BufferPage  *head;              /* the oldest page */
    BufferPage  *tail;              /* most recently added page */
    BufferPage  *tip;               /* the page containing the byte,
                                       referred by the @used parameter */
};

typedef struct _Buffer Buffer;


int buffer_alloc(int page, Buffer **buffer);
int buffer_append(Buffer **buffer, const char *string, int size);
int buffer_used(Buffer **buffer, int *used);
int buffer_get_as_string(Buffer **buffer, String **string);
int buffer_seek(Buffer **buffer, int seek_mode,
                   int seek_offset, int *seek_page,
                   BufferPage **page, int *page_offset);
int buffer_trim(Buffer **buffer, int new_size);
int buffer_free(Buffer **buffer);

