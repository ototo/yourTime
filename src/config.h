/* yourTime
 * 
 * Configuration related structures and functions.
 *
 * TODO: complete header (license, author, etc)
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "version.h"

#define DEFAULT_VERBOSITY 1
#define VERBOSITY_BITS 3


/* Configuration for the program. */
typedef struct _Config {
    unsigned int    verbosity   : VERBOSITY_BITS;
} Config;

/* Option definitions */

enum _OptionId {
    OP_HELP = 0,
    OP_VERSION,
    OP_VERBOSE,
    OP_VERBOSITY_1,
    OP_QUIET
};

typedef enum _OptionId OptionId;

struct _ShortOptionDefinition
{
    char        name;
    OptionId    id;
    int         number_of_args  : 2;
};

struct _LongOptionDefinition
{
    char*       name;
    int         name_length;
    OptionId    id;
    int         number_of_args  : 2;
};

typedef struct _ShortOptionDefinition ShortOptionDefinition;
typedef struct _LongOptionDefinition LongOptionDefinition;

enum _OptionType {
    OT_SHORT = 0,
    OT_LONG,
};

typedef enum _OptionType OptionType;

struct _AnyOption {
    OptionType  type;
    OptionId    id;
    char*       argv[4];
    int         argc;
    union {
        struct {
            char    name;
        } s;
        struct {
            char*   name;
            int     length;
        } l;
    } u;
};

typedef struct _AnyOption AnyOption;

typedef void (*OptionProcessor)(AnyOption* option, Config* config);


/* forward declarations */
int parse_options(int argc, char* argv[],
                  const ShortOptionDefinition* short_options,
                  const LongOptionDefinition* long_options,
                  OptionProcessor processor, Config *config);
void process_option(AnyOption* option, Config* cfg);
void print_config(Config* config);
void print_version(Config* config);
void print_usage(Config* config);

