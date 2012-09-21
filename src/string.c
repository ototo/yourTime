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
    if (!size || !new_string)
        return RC_E_INVALID_ARGS;

    char *str = malloc(size + 1);

    if (!str)
        return RC_E_OUT_OF_MEMORY;

    new_string->recycler = free;
    new_string->refcount = 1;
    new_string->chars = str;
    *new_string->chars = '\0';

    return RC_OK;
}

int string_allocate_static(const char const *string,
                           String *new_string)
{
    if (!string || !new_string)
        return RC_E_INVALID_ARGS;

    new_string->recycler = string_zero_recycler;
    new_string->refcount = 1;
    new_string->chars = (char *)string;

    return RC_OK;
}

int string_allocate_dynamic(const char const *chars, String *string)
{
    if (!chars || !string)
        return RC_E_INVALID_ARGS;

    int len = strlen(chars);
    String str;
    int rc = string_allocate(len, &str);
    if (rc != RC_OK)
        return rc;

    memcpy(str.chars, chars, len + 1);
    memcpy(string, &str, sizeof(*string));

    return RC_OK;
}

int string_copy(String *to, const String *from)
{
    if (!from || !to)
        return RC_E_INVALID_ARGS;

    if (to->chars && (!to->recycler || !to->refcount))
        return RC_E_INVALID_STATE;

    if (to->refcount > 1)
        return RC_E_NOT_EXCLUSIVE;

    if (to->refcount) {
        int rc = string_release(to);
        if (rc != RC_OK)
            return rc;
    }

    int len = from->chars ? strlen(from->chars) + 1 : 0;
    char *str = len ? malloc(len) : NULL;
    if (len && !str)
        return RC_E_OUT_OF_MEMORY;

    if (str)
        memcpy(str, from->chars, len);

    to->refcount = 1;
    to->chars = str;
    to->recycler = len ? free : string_zero_recycler;

    return RC_OK;
}

int string_hold(String *string, int *new_ref)
{
    if (!string)
        return RC_E_INVALID_ARGS;

    if (!string->chars || !string->refcount)
        return RC_E_INVALID_STATE;

    if (string->refcount == STRING_REFCOUNT_MAX)
        return RC_OK_NO_ACTION;

    ++string->refcount;

    if (new_ref)
        *new_ref = string->refcount;

    return RC_OK;
}

int string_release(String *string)
{
    if (!string)
        return RC_E_INVALID_ARGS;

    if (string->refcount == 0)
        return RC_E_INVALID_ARGS;

    if (! --string->refcount) {
        if (string->recycler) {
            string->recycler(string->chars);
            string->recycler = NULL;
        }
        string->chars = NULL;
    }

    return RC_OK;
}

void string_zero_recycler(void *ptr)
{
    /* nothing */
}
