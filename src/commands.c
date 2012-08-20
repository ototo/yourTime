/* yourTime
 * 
 * Command implementation.
 *
 * TODO: complete header (license, author, etc)
 */

#include <assert.h>

#include "commands.h"

static int cmd_status(Config *cfg, int argc, char *argv[]);
static int cmd_start(Config *cfg, int argc, char *argv[]);
static int cmd_switch(Config *cfg, int argc, char *argv[]);
static int cmd_stop(Config *cfg, int argc, char *argv[]);


/* command list */

#define DEF_CMD(arg_name, arg_id) .name = arg_name,\
                        .length = sizeof(arg_name) - 1,\
                        .id = arg_id

CommandDefinition commands[] = {
    {
        DEF_CMD("status", CMD_STATUS),
        .flags = CMDF_DEFAULT | CMDF_READS_DB,
        .handler = cmd_status,
        .help = "",
    },
    {
        DEF_CMD("start", CMD_START),
        .flags = CMDF_READS_DB | CMDF_WRITES_DB,
        .handler = cmd_start,
        .help = "",
    },
    {
        DEF_CMD("switch", CMD_SWITCH),
        .flags = CMDF_READS_DB | CMDF_WRITES_DB,
        .handler = cmd_switch,
        .help = "",
    },
    {
        DEF_CMD("stop", CMD_STOP),
        .flags = CMDF_READS_DB | CMDF_WRITES_DB,
        .handler = cmd_stop,
        .help = "",
    },
    {
        .id = -1,
    }
};

#undef DEF_CMD

int process_command(Config *cfg, int argc, char *argv[])
{
    if (cfg->verbosity > 6)
        printf("Entered %s\n", __func__);
    for (CommandDefinition *cd = commands; cd->id != -1; ++cd) {
        if (cfg->verbosity > 6)
            printf("%s - trying %s\n", __func__, cd->name);
        if (!argc) {
            if (cd->flags & CMDF_DEFAULT)
                return cd->handler(cfg, argc - 1, argv + 1);
        }
        else if (!strncmp(argv[0], cd->name, cd->length)) {
            assert(cd->handler);
            if (cfg->verbosity > 5)
                printf("%s - calling handler for %s\n", __func__, cd->name);
            return cd->handler(cfg, argc - 1, argv + 1);
        }
    }
    if (argc)
        fprintf(stderr, "Unknown command: %s\n", argv[0]);
    else
        fprintf(stderr, "No command specified while no default command "
                        "is defined. Nothing to do.\n");
    return -1;
}

/* handlers */

static int cmd_status(Config *cfg, int argc, char *argv[])
{
    if (cfg->verbosity > 5)
        printf("Entered %s\n", __func__);
    assert(cfg);
    return 0;
}

static int cmd_start(Config *cfg, int argc, char *argv[])
{
    if (cfg->verbosity > 5)
        printf("Entered %s\n", __func__);
    assert(cfg);
    return 0;
}

static int cmd_switch(Config *cfg, int argc, char *argv[])
{
    if (cfg->verbosity > 5)
        printf("Entered %s\n", __func__);
    assert(cfg);
    return 0;
}

static int cmd_stop(Config *cfg, int argc, char *argv[])
{
    if (cfg->verbosity > 5)
        printf("Entered %s", __func__);
    assert(cfg);
    return 0;
}

