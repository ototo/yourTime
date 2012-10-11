/* yourTime
 *
 * String related structures and functions.
 *
 * TODO: complete header (license, author, etc)
 */

#pragma once

#include <stdint.h>


#define STRING_REFCOUNT_TYPE    uint8_t
#define STRING_REFCOUNT_MAX     255


typedef void (*MemoryRecycler)(void *ptr);

struct _String
{
    MemoryRecycler  recycler;
    uint8_t         refcount;
    char            *chars;
};

typedef struct _String String;

int string_allocate(String *str, unsigned int size);
int string_allocate_dynamic(String *str, const char const *chars);
int string_allocate_static(String *str, const char const *chars);
int string_copy(String *str_to, const String *str_from);
int string_hold(String *str);
int string_release(String *str);
void string_zero_recycler(void *ptr);
