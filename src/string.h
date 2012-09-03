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

int string_allocate(unsigned int size, String *new_string);
int string_allocate_static(char *string, String *new_string);
int string_copy(String *to, const String *from);
int string_hold(String *string, int *new_ref);
int string_release(String *string);
void string_zero_recycler(void *ptr);
