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


int string_allocate(String *str, unsigned int size)
{
    if (!str || !size)
        return RC_E_INVALID_ARGS;

    char *chars = malloc(size + 1);

    if (!chars)
        return RC_E_OUT_OF_MEMORY;

    str->recycler = free;
    str->refcount = 1;
    str->chars = chars;
    *str->chars = '\0';

    return RC_OK;
}

int string_allocate_static(String *str, const char const *chars)
{
    if (!str || !chars)
        return RC_E_INVALID_ARGS;

    str->recycler = string_zero_recycler;
    str->refcount = 1;
    str->chars = (char *)chars;

    return RC_OK;
}

int string_allocate_dynamic(String *str, const char const *chars)
{
    if (!str || !chars)
        return RC_E_INVALID_ARGS;

    int len = strlen(chars);
    String new_str;
    int rc = string_allocate(&new_str, len);
    if (rc != RC_OK)
        return rc;

    memcpy(new_str.chars, chars, len + 1);
    memcpy(str, &new_str, sizeof(*str));

    return RC_OK;
}

int string_copy(String *str_to, const String *str_from)
{
    if (!str_from || !str_to)
        return RC_E_INVALID_ARGS;

    if (str_to->chars && (!str_to->recycler || !str_to->refcount))
        return RC_E_INVALID_STATE;

    if (str_to->refcount > 1)
        return RC_E_NOT_EXCLUSIVE;

    if (str_to->refcount) {
        int rc = string_release(str_to);
        if (rc != RC_OK)
            return rc;
    }

    int len = str_from->chars ? strlen(str_from->chars) + 1 : 0;
    char *str = len ? malloc(len) : NULL;
    if (len && !str)
        return RC_E_OUT_OF_MEMORY;

    if (str)
        memcpy(str, str_from->chars, len);

    str_to->refcount = 1;
    str_to->chars = str;
    str_to->recycler = len ? free : string_zero_recycler;

    return RC_OK;
}

int string_hold(String *str)
{
    if (!str)
        return RC_E_INVALID_ARGS;

    if (!str->chars || !str->refcount)
        return RC_E_INVALID_STATE;

    if (str->refcount == STRING_REFCOUNT_MAX)
        return RC_OK_NO_ACTION;

    ++str->refcount;

    return RC_OK;
}

int string_release(String *str)
{
    if (!str)
        return RC_E_INVALID_ARGS;

    if (str->refcount == 0)
        return RC_E_INVALID_ARGS;

    if (! --str->refcount) {
        if (str->recycler) {
            str->recycler(str->chars);
            str->recycler = NULL;
        }
        str->chars = NULL;
    }

    return RC_OK;
}

void string_zero_recycler(void *ptr)
{
    /* nothing */
}
