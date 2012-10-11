/* yourTime
 *
 * Buffer implementation
 *
 * TODO: complete header (license, author, etc)
 */

#pragma once

#include <stdbool.h>

#include "string.h"

#define BUF_SEEK_PAGE_OFFSET        0
#define BUF_SEEK_PAGE_REL_OFFSET    1
#define BUF_SEEK_BUFFER_OFFSET      2
#define BUF_SEEK_BUFFER_REL_OFFSET  3


/* Page of Buffer.
 *
 * Used to allocate memory for a Buffer as needed.
 */
typedef struct _BufferPage BufferPage;
struct _BufferPage
{
    BufferPage  *next;      /* next page moving from head to tail */
    char        data[];     /* data buffer itself */
};



/* Buffer of chars.
 *
 * Allows free reading/writing of data. Buffer uses three concepts
 * related to size:
 *   1. Allocated size (@size) - amount of memory allocated to the
 *      buffer; allocation is done with page granularity on demand.
 *   2. Used size (@used) - amount of memory used by writing data.
 *   3. Current buffer pointer (@tip, @tip_offset) - points to the next
 *      char that will be read/written to.
 *
 * Appending to the buffer is always done starting from the char
 * following the one pointed to by @used offset.
 */
struct _Buffer
{
    unsigned int    page_size;  /* size of a page */
    unsigned int    pages;      /* number of allocated pages */
    unsigned int    size;       /* allocated total size (all pages) */
    unsigned int    used;       /* total used size (payload) */
    BufferPage      *head;      /* the oldest page */
    BufferPage      *tail;      /* most recently added page */
    BufferPage      *tip;       /* the page containing the byte,
                                   referred by the @tip_offset */
    unsigned int    tip_offset; /* next byte to be written */
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
int buffer_alloc(Buffer **buffer, unsigned int page_size);


/* Change buffer size.
 *
 * Increases/decreases the size of the buffer by adding/removing pages
 * and adjusting the tip accordingly. Size is adjusted to be
 * page-aligned.
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
int buffer_resize(Buffer **buffer, unsigned int new_size);


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


/* Write data to a buffer.
 *
 * Writes data to the buffer, increasing the buffer if needed.
 *
 * Parameters:
 *   @buffer        [in/out] buffer to modify;
 *   @data          [in] pointer to data to be appended;
 *   @size          [in] number of chars of data to be written.
 *
 * Returns:
 *   RC_OK                  on success.
 *   RC_E_INVALID_ARGS      when arguments provided are invalid.
 *   RC_E_OUT_OF_MEMORY     when there is no memory available.
 */
int buffer_write(Buffer **buffer, const char *data, unsigned int size);


/* Write a String to a buffer.
 *
 * Writes a String to the buffer, increasing the buffer if needed.
 *
 * Parameters:
 *   @buffer        [in/out] buffer to modify;
 *   @str           [in] String with data to be written.
 *
 * Returns:
 *   RC_OK                  on success.
 *   RC_E_INVALID_ARGS      when arguments provided are invalid.
 *   RC_E_OUT_OF_MEMORY     when there is no memory available.
 */
int buffer_write_string(Buffer **buffer, const String *str);


/* Read data from a buffer.
 *
 * Reads data from the buffer.
 *
 * Parameters:
 *   @buffer        [in/out] buffer to modify;
 *   @data          [in] pointer to a buffer of @size chars;
 *   @size          [in] number of chars of data to be read;
 *   @read          [out] number of chars got read.
 *
 * Returns:
 *   RC_OK                  on success.
 *   RC_E_INVALID_ARGS      when arguments provided are invalid.
 *   RC_E_OUT_OF_MEMORY     when there is no memory available.
 */
int buffer_read(Buffer **buffer, char *data, unsigned int size,
                unsigned int *read);


/* Read data from a buffer (into a String).
 *
 * Reads data from the buffer.
 *
 * Parameters:
 *   @buffer        [in/out] buffer to modify;
 *   @str           [in] String to receive the data being read;
 *   @size          [in] number of chars of data to be read;
 *   @read          [out] number of chars got read.
 *
 * Returns:
 *   RC_OK                  on success.
 *   RC_E_INVALID_ARGS      when arguments provided are invalid.
 *   RC_E_OUT_OF_MEMORY     when there is no memory available.
 */
int buffer_read_string(Buffer **buffer, String *str, unsigned int size,
                       unsigned int *read);


/* Get the buffer contents as a String.
 *
 * Get Buffer's contents stored in a String.
 *
 * Parameters:
 *   @buffer        [in] buffer to get data from;
 *   @str           [in] String to receive the data.
 *
 * Returns:
 *   RC_OK                  on success.
 *   RC_E_INVALID_ARGS      when arguments provided are invalid.
 *   RC_E_OUT_OF_MEMORY     when there is no memory available.
 */
int buffer_get_as_string(Buffer **buffer, String *str);


/* Append data to a buffer.
 *
 * Appends data to the buffer, increasing the buffer if needed. Data is
 * appended starting from the first unused char (pointed by the member
 * buffer->used).
 *
 * Parameters:
 *   @buffer        [in/out] buffer to modify;
 *   @data          [in] pointer to data to be appended;
 *   @size          [in] number of chars of data to be written.
 *
 * Returns:
 *   RC_OK                  on success.
 *   RC_E_INVALID_ARGS      when arguments provided are invalid.
 *   RC_E_OUT_OF_MEMORY     when there is no memory available.
 */
int buffer_append(Buffer **buffer, const char *data, unsigned int size);


/* Append a String to a buffer.
 *
 * Appends a String to the buffer, increasing the buffer if needed. Data
 * is appended starting from the first unused char (pointed by the
 * member buffer->used).
 *
 * Parameters:
 *   @buffer        [in/out] buffer to modify;
 *   @str           [in] String with data to be appended.
 *
 * Returns:
 *   RC_OK                  on success.
 *   RC_E_INVALID_ARGS      when arguments provided are invalid.
 *   RC_E_OUT_OF_MEMORY     when there is no memory available.
 */
int buffer_append_string(Buffer **buffer, String *str);


/* Get the used chars count.
 *
 * Returns the number of chars being used (written to) in the buffer.
 *
 * Parameters:
 *   @buffer        [in] buffer to query;
 *   @used          [out] variable to receive the count.
 *
 * Returns:
 *   RC_OK                  on success.
 *   RC_E_INVALID_ARGS      when arguments provided are invalid.
 *   RC_E_OUT_OF_MEMORY     when there is no memory available.
 */
int buffer_used(Buffer **buffer, unsigned int *used);


/* Get the allocated chars count.
 *
 * Returns the number of chars allocated for the buffer.
 *
 * Parameters:
 *   @buffer        [in] buffer to query;
 *   @used          [out] variable to receive the count.
 *
 * Returns:
 *   RC_OK                  on success.
 *   RC_E_INVALID_ARGS      when arguments provided are invalid.
 *   RC_E_OUT_OF_MEMORY     when there is no memory available.
 */
int buffer_allocated(Buffer **buffer, unsigned int *allocated);


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
                   unsigned int seek_offset, unsigned int *seek_page,
                   BufferPage **page, unsigned int *page_offset);


/* Get the buffer tip position.
 *
 * When writing/reading the buffer, tip position is used. This function
 * returns the current tip position (offset from the beginning of the
 * buffer).
 *
 * Parameters:
 *   @buffer        [in] buffer to query;
 *   @used          [out] variable to receive the offset.
 *
 * Returns:
 *   RC_OK                  on success.
 *   RC_E_INVALID_ARGS      when arguments provided are invalid.
 *   RC_E_OUT_OF_MEMORY     when there is no memory available.
 */
int buffer_tip(Buffer **buffer, unsigned int *buffer_offset);
