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

ShortSwitchDefinition short_switches[] = {
    {   DEFINE_SHORT_SWITCH('h', OP_HELP),
        .args = ARGS_SET_NONE,
        .description =
            "", },
    {   DEFINE_SHORT_SWITCH('v', OP_VERBOSE),
        .args = ARGS_SET_NONE,
        .description =
            "", },
    {   DEFINE_SHORT_SWITCH('q', OP_QUIET),
        .args = ARGS_SET_NONE,
        .description =
            "", },
    {   DEFINE_SHORT_SWITCH('O', OP_SET_OPTION),
        .args = ARGS_SET_FROM(1) | ARGS_SET_ALL,
        .description =
            "", },
    {   .id = -1 }
};

LongSwitchDefinition long_switches[] = {
    {   DEFINE_LONG_SWITCH("help", OP_HELP_1),
        .args = ARGS_SET_FROM(0) | ARGS_SET_ALL, },
    {   DEFINE_LONG_SWITCH("version", OP_VERSION),
        .args = ARGS_SET_NONE,
        .description =
            "", },
    {   DEFINE_LONG_SWITCH("verbosity", OP_VERBOSITY_1),
        .args = ARGS_SET_EXACTLY(1),
        .description =
            "", },
    {   DEFINE_LONG_SWITCH("quiet", OP_QUIET),
        .args = ARGS_SET_NONE, },
    {   DEFINE_LONG_SWITCH("option", OP_QUIET),
        .args = ARGS_SET_FROM(1) | ARGS_SET_ALL,
        .description =
            "", },
    {   .id = -1 }
};

/* Setting variables. */
OptionDefinition options[] = {
    {
        .name = "config_file",
        .id = ST_CONFIG_FILE,
        .def_value = CFG_DEFAULT_CONFIG_FILE,
        .offset = field_offset(Config, config_file),
        .description =
            "fully qualified file name to read configuration from; "
            "default is '" CFG_DEFAULT_CONFIG_FILE "'",
    },
    {
        .id = ST_DATABASE_FILE,
        .name = "database_file",
        .def_value = CFG_DEFAULT_DATABASE_FILE,
        .offset = field_offset(Config, database_file),
        .description =
            "fully qualified database file name to store information in; "
            "default is '" CFG_DEFAULT_DATABASE_FILE "'",
    },
    {
        .id = ST_USER_NAME,
        .name = "user_name",
        .offset = field_offset(Config, user_name),
        .description =
            "display name of the user of the program; "
            "all events will be registered using this name; "
            "default is taken from the OS (if possible)",
    },
    {
        .id = ST_USER_EMAIL,
        .name = "user_email",
        .offset = field_offset(Config, user_email),
        .description =
            "e-mail address of the user of the program; "
            "all events will be registered using this address; "
            "default is taken from the OS (if possible)",
    },
    {
        .id = -1,
    }
};

int main(int argc, char* argv[])
{
    int rc = config_init(&cfg, short_switches, long_switches, options);
    if (rc)
        return rc;

    int idx_args = -1;
    rc = config_process_args(&cfg, argc, argv, config_process_switch, &idx_args);
    if (rc)
    {
        fprintf(stderr, "Error parsing arguments!\n");
        // TODO: improve error reporting
        return rc;
    }

    if (cfg.verbosity > 4)
        print_version(&cfg);

    rc = db_init(&db, &cfg);
    if (rc)
    {
        fprintf(stderr, "Error initializing database: 0x%08X\n", rc);
        return rc;
    }

    int free_args = argc - idx_args;
    rc = command_process_args(&cfg, &db, free_args, &argv[idx_args]);
    if (rc)
    {
        // TODO: improve error reporting
        fprintf(stderr, "Error processing the command!\n");
        return rc;
    }

    db_free(&db);

    /*
    printf("Parsed %d options, %d free arguments are present:\n",
        idx_args - 1, argc - idx_args);
    for (; idx_args < argc; ++idx_args)
        printf("\t'%s'\n", argv[idx_args]);

    printf("\n");
    */

    return rc;
}
