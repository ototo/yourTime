/* yourTime
 *
 * String related structures and functions.
 *
 * TODO: complete header (license, author, etc)
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "string.h"


int string_allocate(unsigned int size, String *new_string)
{
    if (!size)
        return -1; // TODO: result code - invalid argument

    char *str = malloc(size + 1);

    if (!str)
        return -1; // TODO: result cod - no memory

    new_string->recycler = free;
    new_string->refcount = 1;
    new_string->chars= str;

    return 0;
}

int string_allocate_static(char *string, String *new_string)
{
    if (!string)
        return -1; // TODO: result code - invalid argument

    new_string->recycler = string_zero_recycler;
    new_string->refcount = 1;
    new_string->chars = string;

    return 0; // TODO: result code - success
}

int string_copy(String *to, const String *from)
{
    if (!from || !to)
        return -1; // TODO: result code - invalid argument

    if (to->chars)
        to->recycler(to->chars);
    int len = strlen(from->chars) + 1;
    to->chars = malloc(len);
    if (!to->chars)
        return -1; // TODO: result code - out of memory
    memcpy(to->chars, from->chars, len);
    to->recycler = free;

    return 0;
}

int string_hold(String *string, int *new_ref)
{
    if (!string)
        return -1; // TODO: result code - already freed

    if (!string->chars)
        return -2; // TODO: result code - invalid state

    if (string->refcount == STRING_REFCOUNT_MAX)
        return -3; // TODO: result code - saturated refcount

    string->refcount ++;

    return 0;
}

int string_release(String *string)
{
    if (!string)
        return 0; // TODO: result code - already freed

    if (string->refcount == 0)
        return -2; // TODO: result code - invalid refcount

    if (! --string->refcount) {
        string->recycler(string->chars);
        string->chars = NULL;
    }

    return 0;
}

void string_zero_recycler(void *ptr)
{
    /* nothing */
}
