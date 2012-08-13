/* yourTime
 * 
 * Main program entry file.
 *
 * TODO: complete heared (license, author, etc)
 */

#define VERSION "0.1a"
#define DEFAULT_VERBOSITY 1
#define VERBOSITY_BITS 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* commands */
#define CMD_START "start"
#define CMD_START_LEN (sizeof(CMD_START) - 1)
#define CMD_STOP "stop"
#define CMD_STOP_LEN (sizeof(CMD_STOP) - 1)
#define CMD_SWITCH "switch"
#define CMD_SWITCH_LEN (sizeof(CMD_SWITCH) - 1)

/* types */
typedef struct _Config {
    char            program_name[_PC_PATH_MAX];
    char*           version;
    unsigned int    verbosity   : VERBOSITY_BITS;
} Config;

static Config cfg;

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

ShortOptionDefinition short_options[] = {
    {
        .name= 'h',
        .id = OP_HELP,
    },
    {
        .name = 'v',
        .id = OP_VERBOSE,
    },
    {
        .name = 'q',
        .id = OP_QUIET,
    }
};

const int short_options_count = sizeof(short_options) / sizeof(short_options[0]);

LongOptionDefinition long_options[] = {
    {
        .name = "help",
        .name_length = 4,
        .id = OP_HELP,
    },
    {
        .name = "version",
        .name_length = 7,
        .id = OP_VERSION,
    },
    {
        .name = "verbosity",
        .name_length = 9,
        .id = OP_VERBOSITY_1,
        .number_of_args = 1,
    },
    {
        .name = "quiet",
        .name_length = 5,
        .id = OP_QUIET,
    }
};

const int long_options_count = sizeof(long_options) / sizeof(long_options[0]);

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
void print_usage(Config* config);
void print_version(Config* config);
void print_config(Config* config);
int parse_options(int argc, char* argv[], OptionProcessor processor, Config *config);


/* Go through th ecommand line arguments, update configuration
 * accordingly, return an index of the first non-option argument */
int parse_options(
        int argc, char* argv[],
        OptionProcessor processor, Config *config)
{
    strncpy(config->program_name, argv[0], _PC_PATH_MAX);
    config->version = VERSION;

    AnyOption opt;

    /* print option definitions */
    /* printf("= Short option definitions:\n");
    for (int idx = 0; idx < short_options_count; ++idx)
        printf("\tname: %c  id: %d  args: %d\n",
            short_options[idx].name, short_options[idx].id,
            short_options[idx].number_of_args);

    printf("= Long option definitions:\n");
    for (int idx = 0; idx < long_options_count; ++idx)
        printf("\tname: %s (%d)  id: %d  args: %d\n",
            long_options[idx].name, long_options[idx].name_length,
            long_options[idx].id, long_options[idx].number_of_args);
    printf("\n");
    */

    for (int idx = 1; idx < argc; ++idx ) {
        /* printf("* parsing option %d...\n", idx); */
        if (argv[idx][0] == '-') {
            if (argv[idx][1] == '-') {
                /* double-dash option - a longer one */
                for (int op_idx = 0; op_idx < long_options_count; ++op_idx) {
                    /* printf(". checking longer option '%s'...\n",
                        options[op_idx].longer); */
                    if (!strncmp(long_options[op_idx].name,
                                 &argv[idx][2],
                                 long_options[op_idx].name_length)) {

                        memset(&opt, 0, sizeof(opt));
                        opt.type = OT_LONG;
                        opt.id = long_options[op_idx].id;
                        opt.u.l.name = long_options[op_idx].name;
                        opt.u.l.length = long_options[op_idx].name_length;
                        if (opt.argc = long_options[op_idx].number_of_args)
                            for (int i = 0; i < 4; ++i)
                                opt.argv[i] = argv[idx + i + 1];

                        processor(&opt, config);

                        idx += opt.argc;

                        /*printf("+ found long option '%s' (%d args)\n",
                                long_options[op_idx].name,
                                long_options[op_idx].number_of_args);
                        */
                        break;
                    }
                }
            }
            else {
                /* single-dash option - a shorter one */
                for (char* ptr = &argv[idx][1]; *ptr; ++ptr) {
                    /* printf("- parsing symbol at %d ('%c')...\n", 
                        ptr - &argv[idx][1], *ptr); */
                    for (int op_idx = 0; op_idx < short_options_count; ++op_idx) {
                        /* printf(". checking shorter option '%c'...\n",
                            options[op_idx].shorter); */
                        if (*ptr == short_options[op_idx].name){

                            memset(&opt, 0, sizeof(opt));
                            opt.type = OT_SHORT;
                            opt.id = short_options[op_idx].id;
                            opt.u.s.name = short_options[op_idx].name;
                            if (opt.argc = short_options[op_idx].number_of_args)
                                for (int i = 0; i < 4; ++i)
                                    opt.argv[i] = argv[idx + i + 1];

                            processor(&opt, config);

                            idx += opt.argc;

                            /*
                            printf("+ found short option '%c' (%d args)\n",
                                    short_options[op_idx].name,
                                    short_options[op_idx].number_of_args);
                            */
                            break;
                        }
                    }
                }
            }
        }
        else
            return idx;
    }

    return argc;
}


/* process an option provided, update the config accordingly */
void ProcessOption(AnyOption* option, Config* cfg)
{
    /*
    if (option->type == OT_SHORT)
        printf("# OP(name: %c  id: %d  argc: %d)\n",
                option->s.name, option->id, option->argc);
    else if (option->type == OT_LONG)
        printf("# OP(name: %s (%d)  id: %d  argc: %d)\n",
                option->l.name, option->l.length, option->id,
                option->argc);
    else
        printf("# OP(unknown type: %d  id: %d  argc: %d)\n",
                option->type, option->id, option->argc);
    */

    switch(option->id) {
    case OP_HELP:
        print_usage(cfg);
        break;
    case OP_VERSION:
        print_version(cfg);
        break;
    case OP_VERBOSE:
        if (cfg->verbosity < (1 << VERBOSITY_BITS) - 1)
            cfg->verbosity += 1;
        break;
    case OP_VERBOSITY_1:
        cfg->verbosity = atoi(option->argv[0]);
        break;
    case OP_QUIET:
        cfg->verbosity = 0;
        break;
    }
}

void print_version(Config* config)
{
    printf(
        "yourTime v%s, compiled on %s\n",
         config->version, __TIMESTAMP__);
}

void print_config(Config* config)
{
    printf(
        "Configuration:\n"
        "\tverbosity: %d\n",
        config->verbosity
    );
}

void print_usage(Config* config)
{
    printf(
        "Usage: %s [<options>] [<command> [<arguments>]]\n"
        "\n"
        "where <options> are:\n"
        "\t-h, --help\n"
        "\t\tshow help (this message)\n"
        "\t-v[v[v]], --verbosity <level>\n"
        "\t\tset diagnostic message verbosity (default is 1)\n"
        "\t-q, --quiet\n"
        "\t\tdisable all diagnostic messages (set verbosity to 0)\n"
        "\n"
        "<command> and corresponding <arguments> are:\n"
        "\tstart [<summary text>]\n"
        "\t\tStart new activivty, use all the other parameters as\n"
        "\t\ta summary text; if summary text is not provided - use\n"
        "\t\t'unknown activity' one instead.\n"
        "\tswitch [<event_ids>] [<summary text>]\n"
        "\t\tStop running activities and at the same time\n"
        "\t\tstart a new one using the summary provided; if summary\n"
        "\t\ttext is not provided - use 'unknown activity' one instead.\n"
        "\tstop [<event_ids>]\n"
        "\t\tStops currently running activities.\n"
        "\n"
        "where:\n"
        "\t<summary text> can be a set of strings, quoted or not,\n"
        "\t\twhich are joined together in one string using spaces.\n"
        "\t<event_ids> can be a single number (e.g. '1' with no\n"
        "\t\tquotes), a list of numbers (e.g. '1, 3, 5'), a range\n"
        "\t\t(e.g. '1-5'), or a list of numbers and ranges (e.g.\n"
        "\t\t'1, 3-5, 9-). Please note that ranges can be open (i.e.\n"
        "\t\t'1-', '-9' and '-' are all possible).\n",
        config->program_name
    );
}

int main(int argc, char* argv[])
{
    memset(&cfg, 0, sizeof(cfg));
    cfg.verbosity = DEFAULT_VERBOSITY;
    int idx_args = -1;
    if ((idx_args =  parse_options(argc, argv, ProcessOption, &cfg)) < 0) {
        fprintf(stderr, "Error parsing arguments!\n");
        return 1;
    }

    if (cfg.verbosity > 4)
        print_version(&cfg);

    /*
    printf("Parsed %d options, %d free arguments are present:\n",
        idx_args - 1, argc - idx_args);
    for (; idx_args < argc; ++idx_args)
        printf("\t'%s'\n", argv[idx_args]);

    printf("\n");
    print_config(&cfg);
    */

    return 0;
}
