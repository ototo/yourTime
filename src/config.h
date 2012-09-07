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
#include "string.h"

#define CFG_USER_NAME_LENGTH        256
#define CFG_USER_EMAIL_LENGTH       256
#define CFG_DEFAULT_VERBOSITY       1
#define CFG_DEFAULT_CONFIG_FILE     "~/.yourTube/config"
#define CFG_DEFAULT_DATABASE_FILE   "~/.yourTime/yourTime.sqlite"
#define CFG_BUFFER_SIZE             512

#define VERBOSITY_BITS 3


/* Configuration setting definition structure */

enum _OptionId {
    ST_CONFIG_FILE = 0,
    ST_DATABASE_FILE,
    ST_LOG_FILE,
    ST_USER_NAME,
    ST_USER_EMAIL,
};

typedef enum _OptionId OptionId;

struct _OptionDefinition {
    OptionId    id;
    char        *name;
    char        *def_value;
    int         offset;
    char        *description;
};

typedef struct _OptionDefinition OptionDefinition;

#define field_offset(type, field) \
    (intptr_t)(&(((type*)0)->field))


/* Switch definitions */

enum _SwitchId {
    OP_HELP = 0,
    OP_HELP_1,
    OP_VERSION,
    OP_VERBOSE,
    OP_VERBOSITY_1,
    OP_QUIET,
    OP_SET_OPTION,
};

/* switch argument number stuff */
#define ARGS_FROM_MASK          0x000000FF
#define ARGS_TO_MASK            0x0000FF00
#define ARGS_SET_FROM(num)      (num & ARGS_FROM_MASK)
#define ARGS_FROM(args)         (args & ARGS_FROM_MASK)
#define ARGS_SET_TO(num)        ((num & ARGS_TO_MASK) << 8)
#define ARGS_TO(num)            ((num & ARGS_TO_MASK) >> 8)
#define ARGS_SET_EXACTLY(num)   ARGS_FROM(num) | ARGS_TO(num)
#define ARGS_SET_NONE           0x00000000
#define ARGS_SET_OPTIONAL       0x000000FF
#define ARGS_SET_ALL            0x0000FF00

typedef enum _SwitchId SwitchId;

struct _ShortSwitchDefinition
{
    char        name;
    SwitchId    id;
    int         args;
    char        *description;
};

#define DEFINE_SHORT_SWITCH(n,i) \
    .name = n,\
    .id = i

struct _LongSwitchDefinition
{
    char        *name;
    int         length;
    SwitchId    id;
    int         args;
    char        *description;
};

#define DEFINE_LONG_SWITCH(arg_name, arg_id) \
    .name = arg_name,\
    .length = sizeof(arg_name) - 1,\
    .id = arg_id

typedef struct _ShortSwitchDefinition ShortSwitchDefinition;
typedef struct _LongSwitchDefinition LongSwitchDefinition;

enum _SwitchType {
    OT_SHORT = 0,
    OT_LONG,
};

typedef enum _SwitchType SwitchType;

struct _AnySwitch {
    SwitchType  type;
    SwitchId    id;
    char        *argv[4];
    int         argc;
    union {
        struct {
            char    name;
        } s;
        struct {
            char    *name;
            int     length;
        } l;
    } u;
};

typedef struct _AnySwitch AnySwitch;


/* In-memory configuration for the program. */
struct _Config
{
    ShortSwitchDefinition   *short_switch_defs;
    LongSwitchDefinition    *long_switch_defs;
    OptionDefinition        *option_defs;

    /* variables */
    unsigned int            verbosity   : VERBOSITY_BITS;
    String                  config_file;
    String                  database_file;
    String                  user_name;
    String                  user_email;
};

typedef struct _Config Config;

typedef void (*SwitchProcessor)(AnySwitch *option, Config *config);


/* Initialize configuration.
 *
 * Initializes configuration structure, sets defaults.
 *
 * Parameters:
 *   @config        configuration structure to initialize;
 *   @short_options list of short option definitions, terminated by an
 *                  entry with .id = -1;
 *   @long_options  list of long option definitions, terminated by an
 *                  entry with .id = -1;
 *   @settings      list of setting definitions, terminated by an entry
 *                  with .id = -1.
 *
 * Returns:
 *   YTE_OK             on success;
 *   TODO: complete
 */

int config_init(Config *config, ShortSwitchDefinition *short_switches,
                LongSwitchDefinition *long_switches,
                OptionDefinition *options);


/* Process configuration and command line.
 *
 * Configure application for the run time by:
 *   1. processing default configuration file (if present);
 *   2. processing command line arguments in the order of their
 *      occurence.
 *
 * Parameters:
 *   @config        [in/out] config structure to work on;
 *   @argc          [in] number of arguments in argv;
 *   @argv          [in] command line arguments to process;
 *   @processor     [in] a callback to process options found;
 *   @free_args     [out] index of the first free argument.
 *
 * Returns:
 *   YTE_OK             on success;
 *   TODO: complete
 */

int config_process_args(Config *config, int argc, char *argv[],
                        SwitchProcessor processor, int *free_args);

void config_process_switch(AnySwitch *aswitch, Config *cfg);
void print_config(Config *config);
void print_options(Config *config);
void print_version(Config *config);
void print_usage(Config *config);

int config_parse_file(Config *config, char *config_file);
int config_set_option(Config *config, const String *name,
                      const String *value);
