/* yourTime
 *
 * Buffer implementation
 *
 * TODO: complete header (license, author, etc)
 */

#pragma once

#define BUF_SEEK_PAGE_OFFSET    0
#define BUF_SEEK_BUFFER_OFFSET  1

/* paged buffer structures */
struct _BufferPage;
typedef struct _BufferPage BufferPage;

struct _BufferPage
{
    BufferPage  *next;      /* next page */
    char        data[];     /* data buffer itself */
};

struct _Buffer
{
    int         page_size;          /* page size */
    int         pages;              /* number of allocated pages */
    int         size;               /* allocated total size (all pages) */
    int         used;               /* total used size */
    int         tip_page_used;      /* used on the tip page */
    int         tip_page_offset;    /* to the first unused byte */
    BufferPage  *head;              /* most recently added page */
    BufferPage  *tail;              /* the oldest page */
    BufferPage  *tip;               /* the page containing the next free
                                       byte */
};

typedef struct _Buffer Buffer;


int buffer_alloc(int page, Buffer **buffer);
int buffer_append(Buffer **buffer, char *string, int size);
int buffer_get_as_string(Buffer **buffer, char **string);
int buffer_free_string(char **string);
int buffer_trim(Buffer **buffer, int new_size);
int buffer_free(Buffer **buffer);

