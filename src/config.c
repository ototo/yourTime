/* yourTime
 *
 * Configuration related structures and functions.
 *
 * TODO: complete header (license, author, etc)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "version.h"
#include "config.h"


/* Go through the command line arguments, update configuration
 * accordingly, return an index of the first non-option argument */
int parse_options(
        int argc, char* argv[],
        const ShortOptionDefinition* short_options,
        const LongOptionDefinition* long_options,
        OptionProcessor processor, Config *config)
{
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
            long_options[idx].name, long_options[idx].length,
            long_options[idx].id, long_options[idx].number_of_args);
    printf("\n");
    */

    for (int idx = 1; idx < argc; ++idx ) {
        /* printf("* parsing option %d...\n", idx); */
        if (argv[idx][0] == '-') {
            if (argv[idx][1] == '-') {
                /* double dash with no name = end of options */
                if (argv[idx][2] == ' ' || argv[idx][2] == '\0')
                    return idx + 1;

                /* double-dash option - a longer one */
                for (int op_idx = 0; long_options[op_idx].id != -1; ++op_idx)
                {
                    /* printf(". checking longer option '%s'...\n",
                        options[op_idx].longer); */
                    if (!strncmp(long_options[op_idx].name,
                                 &argv[idx][2],
                                 long_options[op_idx].length)) {

                        memset(&opt, 0, sizeof(opt));
                        opt.type = OT_LONG;
                        opt.id = long_options[op_idx].id;
                        opt.u.l.name = long_options[op_idx].name;
                        opt.u.l.length = long_options[op_idx].length;
                        if ((opt.argc =
                                long_options[op_idx].number_of_args)) {
                            if (idx >= argc - 1) {
                                fprintf(stderr,
                                        "Option '--%s' requires %d "
                                        "argument%s", &argv[idx][2],
                                        opt.argc, (opt.argc % 10 == 1)
                                                    ? "s" : "");
                                return argc;
                            }
                            for (int i = 0; i < opt.argc; ++i)
                                opt.argv[i] = argv[idx + i + 1];
                        }

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
                    int op_idx;
                    for (op_idx = 0; short_options[op_idx].id != -1 ;
                            ++op_idx) {
                        /* printf(". checking shorter option '%c'...\n",
                            options[op_idx].shorter); */
                        if (*ptr == short_options[op_idx].name) {

                            memset(&opt, 0, sizeof(opt));
                            opt.type = OT_SHORT;
                            opt.id = short_options[op_idx].id;
                            opt.u.s.name = short_options[op_idx].name;
                            if ((opt.argc =
                                    short_options[op_idx].number_of_args))
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
                    if (short_options[op_idx].id == -1) {
                        fprintf(stderr, "Invalid switch '%c' - "
                                        "ignoring.\n", *ptr);
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
void process_option(AnyOption* option, Config* cfg)
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

    switch(option->id)
    {
    case OP_HELP:
        print_usage(cfg);
        break;
    case OP_HELP_1:
        // TODO: implement
        if (!strncmp(option->argv[0], "settings", 8))
            print_settings(cfg);
        break;
    case OP_VERSION:
        print_version(cfg);
        break;
    case OP_VERBOSE:
        if (cfg->verbosity < (1 << VERBOSITY_BITS) - 1)
            cfg->verbosity += 1;
        break;
    case OP_VERBOSITY_1:
        {
            int val = atoi(option->argv[0]);
            cfg->verbosity = val ? val < (1 << VERBOSITY_BITS)
                                 : (1 << VERBOSITY_BITS) - 1;
        }
        break;
    case OP_QUIET:
        cfg->verbosity = 0;
        break;
    }
}

void print_config(Config* config)
{
    printf(
        "Configuration:\n"
        "\tverbosity: %d\n",
        config->verbosity
    );
}

void print_settings(Config *config)
{
    printf("Settings:\n");
    for (SettingDefinition *sd = config->setting_defs; sd->id != -1; ++sd)
        printf("\t%s: %s\n", sd->name, "N/I");
}

void print_version(Config* config)
{
    printf(
        "%s v%s, compiled on %s\n",
         YOURTIME_FULL_NAME, YOURTIME_VERSION,
         YOURTIME_BUILD_TIMESTAMP);
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
        YOURTIME_SHORT_NAME
    );

    if (config->verbosity > 1) {
        printf("\n");
        print_config(config);
    }
}

int parse_config(char *config_file)
{
    return 0;
}
