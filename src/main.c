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


/* Application config */
static Config cfg;
static Database db;

#define DEF_SOPT(n,i) .name = n,\
                      .id = i

ShortOptionDefinition short_options[] = {
    {
        DEF_SOPT('h', OP_HELP),
    },
    {
        DEF_SOPT('v', OP_VERBOSE),
    },
    {
        DEF_SOPT('q', OP_QUIET),
    },
    {
        .id = -1
    }
};

#undef DEF_SOPT

#define DEF_LOPT(arg_name, arg_id) .name = arg_name,\
                                   .length = sizeof(arg_name) - 1,\
                                   .id = arg_id

LongOptionDefinition long_options[] = {
    {
        DEF_LOPT("help", OP_HELP_1),
        .number_of_args = 1,
    },
    {
        DEF_LOPT("version", OP_VERSION),
    },
    {
        DEF_LOPT("verbosity", OP_VERBOSITY_1),
        .number_of_args = 1,
    },
    {
        DEF_LOPT("quiet", OP_QUIET),
    },
    {
        .id = -1
    }
};

#undef DEF_LOPT

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

    if ((res = parse_config(&cfg, "~/.yourTime/config")))
    {
        // TODO: improve error reporting
        fprintf(stderr, "Error parsing the config file!\n");
        return res;
    }

    if ((res = db_init(&db, &cfg)))
    {
        fprintf(stderr, "Error initializing database: 0x%08X\n", res);
        return res;
    }

    if ((res = parse_command(&cfg, &db, argc - idx_args, &argv[idx_args])))
    {
        // TODO: improve error reporting
        fprintf(stderr, "Error processing the command!\n");
        return res;
    }

    db_free(&db);

    /*
    printf("Parsed %d options, %d free arguments are present:\n",
        idx_args - 1, argc - idx_args);
    for (; idx_args < argc; ++idx_args)
        printf("\t'%s'\n", argv[idx_args]);

    printf("\n");
    */

    return 0;
}
