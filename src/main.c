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
#include "commands.h"


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
        .id = OP_HELP_1,
        .number_of_args = 1,
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

/* Setting variables. */
SettingDefinition settings[] = {
    {
        .id = ST_CONFIG_FILE,
        .name = "config_file",
        .def_value = DEFAULT_CONFIG_FILE,
        .offset = 0,
        .description =
            "fully qualified file name to read configuration from; "
            "default is '" DEFAULT_CONFIG_FILE "'",
    },
    {
        .id = ST_DATABASE_FILE,
        .name = "database_file",
        .def_value = DEFAULT_DATABASE_FILE,
        .offset = 0,
        .description =
            "fully qualified database file name to store information in; "
            "default is '" DEFAULT_DATABASE_FILE "'",
    },
    {
        .id = -1,
    }
};

int main(int argc, char* argv[])
{
    memset(&cfg, 0, sizeof(cfg));
    cfg.setting_defs = settings;
    cfg.verbosity = DEFAULT_VERBOSITY;
    int idx_args = -1;
    if ((idx_args =  parse_options(argc, argv,
                                   short_options, long_options,
                                   process_option, &cfg)) < 0)
    {
        fprintf(stderr, "Error parsing arguments!\n");
        // TODO: improve error reporting
        return idx_args;
    }

    if (cfg.verbosity > 4)
        print_version(&cfg);

    int res;

    if ((res = parse_config("~/.yourTime/config")))
    {
        // TODO: improve error reporting
        fprintf(stderr, "Error parsing the config file!\n");
        return res;
    }

    if ((res = process_command(argc - idx_args, &argv[idx_args], &cfg)))
    {
        // TODO: improve error reporting
        fprintf(stderr, "Error processing the command!\n");
        return res;
    }

    /*
    printf("Parsed %d options, %d free arguments are present:\n",
        idx_args - 1, argc - idx_args);
    for (; idx_args < argc; ++idx_args)
        printf("\t'%s'\n", argv[idx_args]);

    printf("\n");
    */

    return 0;
}
