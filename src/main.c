/* yourTime
 * 
 * Main program entry file.
 *
 * TODO: complete heared (license, author, etc)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

/* commands */
#define CMD_START "start"
#define CMD_START_LEN (sizeof(CMD_START) - 1)
#define CMD_STOP "stop"
#define CMD_STOP_LEN (sizeof(CMD_STOP) - 1)
#define CMD_SWITCH "switch"
#define CMD_SWITCH_LEN (sizeof(CMD_SWITCH) - 1)

/* Application config */
static Config cfg;

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
    },
    {
        .id = -1
    }
};

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
    },
    {
        .id = -1
    }
};

int main(int argc, char* argv[])
{
    memset(&cfg, 0, sizeof(cfg));
    cfg.verbosity = DEFAULT_VERBOSITY;
    int idx_args = -1;
    if ((idx_args =  parse_options(argc, argv,
                                   short_options, long_options,
                                   process_option, &cfg)) < 0)
    {
        fprintf(stderr, "Error parsing arguments!\n");
        return 1;
    }

    if (cfg.verbosity > 4)
        print_version(&cfg);

    if (parse_config("~/.yourTime/config"))
    {
        fprintf(stderr, "Error parsing the config file!\n");
        return 2;
    }

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
