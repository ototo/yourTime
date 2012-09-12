/* yourTime
 *
 * String related structures and functions.
 *
 * TODO: complete header (license, author, etc)
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "error.h"
#include "string.h"


int string_allocate(unsigned int size, String *new_string)
{
    if (!size)
        return RC_E_INVALID_ARGS;

    char *str = malloc(size + 1);

    if (!str)
        return RC_E_OUT_OF_BOUNDS;

    new_string->recycler = free;
    new_string->refcount = 1;
    new_string->chars= str;

    return RC_OK;
}

int string_allocate_static(char *string, String *new_string)
{
    if (!string)
        return RC_E_INVALID_ARGS;

    new_string->recycler = string_zero_recycler;
    new_string->refcount = 1;
    new_string->chars = string;

    return RC_OK;
}

int string_copy(String *to, const String *from)
{
    if (!from || !to)
        return RC_E_INVALID_ARGS;

    if (to->chars)
        to->recycler(to->chars);
    int len = strlen(from->chars) + 1;
    to->chars = malloc(len);
    if (!to->chars)
        return RC_E_OUT_OF_MEMORY;
    memcpy(to->chars, from->chars, len);
    to->recycler = free;

    return RC_OK;
}

int string_hold(String *string, int *new_ref)
{
    if (!string)
        return RC_E_INVALID_ARGS;

    if (!string->chars)
        return RC_E_INVALID_ARGS;

    if (string->refcount == STRING_REFCOUNT_MAX)
        return RC_OK_NO_ACTION;

    string->refcount ++;

    return RC_OK;
}

int string_release(String *string)
{
    if (!string)
        return RC_E_INVALID_ARGS;

    if (string->refcount == 0)
        return RC_E_INVALID_ARGS;

    if (! --string->refcount) {
        string->recycler(string->chars);
        string->chars = NULL;
    }

    return RC_OK;
}

void string_zero_recycler(void *ptr)
{
    /* nothing */
}
