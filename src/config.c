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
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include "version.h"
#include "config.h"
#include "buffer.h"


int config_init(Config *config, ShortSwitchDefinition *short_switches,
                LongSwitchDefinition *long_switches,
                OptionDefinition *options)
{
    assert(config);

    memset(config, 0, sizeof(*config));
    config->short_switch_defs = short_switches;
    config->long_switch_defs = long_switches;
    config->option_defs = options;

    /* set defaults */
    config->verbosity = CFG_DEFAULT_VERBOSITY;
    // TODO: set config file
    // TODO: set database file
    // TODO: set user name
    // TODO: set user e-mail

    return 0; // TODO: result code
}


int config_free(Config *config)
{
    return 0; // TODO: result code - success
}


int config_process_args(Config *config, int argc, char* argv[],
                        SwitchProcessor processor, int *free_args)
{
    assert(config);

    AnySwitch aswitch;

    /* print switch definitions */
    /* printf("= Short switch definitions:\n");
    for (int idx = 0; idx < short_options_count; ++idx)
        printf("\tname: %c  id: %d  args: %d\n",
            short_switches[idx].name, short_switches[idx].id,
            short_switches[idx].args);

    printf("= Long switch definitions:\n");
    for (int idx = 0; idx < long_options_count; ++idx)
        printf("\tname: %s (%d)  id: %d  args: %d\n",
            long_options[idx].name, long_options[idx].length,
            long_options[idx].id, long_options[idx].number_of_args);
    printf("\n");
    */

    LongSwitchDefinition *long_switches = config->long_switch_defs;

    for (int idx = 1; idx < argc; ++idx ) {
        /* printf("* parsing option %d...\n", idx); */
        if (argv[idx][0] == '-') {
            if (argv[idx][1] == '-') {
                /* double dash with no name = end of options */
                if (argv[idx][2] == ' ' || argv[idx][2] == '\0')
                    return idx + 1;

                /* double-dash option - a longer one */
                for (int sw_idx = 0; long_switches[sw_idx].id != -1;
                                                                ++sw_idx)
                {
                    /* printf(". checking longer option '%s'...\n",
                        options[sw_idx].longer); */
                    if (!strncmp(long_switches[sw_idx].name,
                                 &argv[idx][2],
                                 long_switches[sw_idx].length)) {

                        memset(&aswitch, 0, sizeof(aswitch));
                        aswitch.type = OT_LONG;
                        aswitch.id = long_switches[sw_idx].id;
                        aswitch.u.l.name = long_switches[sw_idx].name;
                        aswitch.u.l.length = long_switches[sw_idx].length;
                        if ((aswitch.argc =
                                long_switches[sw_idx].args)) {
                            if (idx >= argc - 1) {
                                fprintf(stderr,
                                        "Switch '--%s' requires %d "
                                        "argument%s", &argv[idx][2],
                                        aswitch.argc,
                                        (aswitch.argc % 10 == 1)
                                                      ? "s" : "");
                                return argc;
                            }
                            for (int i = 0; i < aswitch.argc; ++i)
                                aswitch.argv[i] = argv[idx + i + 1];
                        }

                        processor(&aswitch, config);

                        idx += aswitch.argc;

                        /*printf("+ found long option '%s' (%d args)\n",
                                long_options[sw_idx].name,
                                long_options[sw_idx].number_of_args);
                        */
                        break;
                    }
                }
            }
            else {
                ShortSwitchDefinition *short_switches =
                                                config->short_switch_defs;
                /* single-dash switch - a shorter one */
                for (char* ptr = &argv[idx][1]; *ptr; ++ptr) {
                    /* printf("- parsing symbol at %d ('%c')...\n",
                        ptr - &argv[idx][1], *ptr); */
                    int sw_idx;
                    for (sw_idx = 0; short_switches[sw_idx].id != -1 ;
                            ++sw_idx) {
                        /* printf(". checking shorter option '%c'...\n",
                            options[sw_idx].shorter); */
                        if (*ptr == short_switches[sw_idx].name) {

                            memset(&aswitch, 0, sizeof(aswitch));
                            aswitch.type = OT_SHORT;
                            aswitch.id = short_switches[sw_idx].id;
                            aswitch.u.s.name = short_switches[sw_idx].name;
                            if ((aswitch.argc =
                                    short_switches[sw_idx].args))
                                for (int i = 0; i < 4; ++i)
                                    aswitch.argv[i] = argv[idx + i + 1];

                            processor(&aswitch, config);

                            idx += aswitch.argc;

                            /*
                            printf("+ found short option '%c' (%d args)\n",
                                    short_options[sw_idx].name,
                                    short_options[sw_idx].number_of_args);
                            */
                            break;
                        }
                    }
                    if (short_switches[sw_idx].id == -1) {
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


/* process a switch provided, update the config accordingly */
void config_process_switch(AnySwitch* aswitch, Config* cfg)
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

    switch(aswitch->id)
    {
    case OP_HELP:
        print_usage(cfg);
        break;
    case OP_HELP_1:
        // TODO: implement
        if (!strncmp(aswitch->argv[0], "settings", 8))
            print_options(cfg);
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
            int val = atoi(aswitch->argv[0]);
            cfg->verbosity = val ? val < (1 << VERBOSITY_BITS)
                                 : (1 << VERBOSITY_BITS) - 1;
        }
        break;
    case OP_QUIET:
        cfg->verbosity = 0;
        break;
    case OP_SET_OPTION:
        //TODO: implement
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

void print_options(Config *config)
{
    printf("Options:\n");
    for (OptionDefinition *od = config->option_defs; od->id != -1; ++od)
        printf("\t%s: %s\n", od->name, "N/I");
}

void print_version(Config* config)
{
    printf(
        "%s v%s [%s/%s], compiled on %s\n",
         YOURTIME_FULL_NAME, YOURTIME_VERSION,
         YOURTIME_REVISION, YOURTIME_BRANCH,
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

#define STATE_NONE          0
#define STATE_NAME          1
#define STATE_QUOTED_NAME   2
#define STATE_ASSIGN        3
#define STATE_VALUE         4
#define STATE_QUOTED_VALUE  5
#define STATE_SKIP_TO_EOL   6

bool is_ws(char ch)
{
    switch (ch) {
    case '\t':
    case '\f':
    case '\v':
    case ' ':
        return true;
    default:
        return false;
    }
}

bool is_eol(char ch)
{
    switch (ch) {
    case '\r':
    case '\n':
        return true;
    default:
        return false;
    }
}

int config_parse_file(Config *config, char *config_file)
{
    assert(config);
    if (!config_file || !*config_file)
        return 0; // TODO: result code

    FILE *f = fopen(config_file, "r");
    if (!f) {
        fprintf(stderr,
            "Error: Could not open config '%s'\n", config_file);
        return -1; // TODO: result code
    }
    char buf[CFG_BUFFER_SIZE];
    int state = STATE_NONE;
    Buffer  *name_buf;
    Buffer  *value_buf;
    int line = 1;
    int column = 1;

    int rc = buffer_alloc(16, &name_buf);
    if (rc) {
        goto exit;
    }
    rc = buffer_alloc(512, &value_buf);
    if (rc) {
        goto cleanup;
    }

    char *end_buf = buf + CFG_BUFFER_SIZE - 1;
    char *pch = buf;
    char ch;
    String name;
    String value;
    uint16_t eols = 0;

#define EOL_PAIR(first, second) \
    ((((uint16_t)first) << (8 & 0xFF)) | ((uint16_t)second & 0xFF))

#define EOL_PAIR_SHIFT(pair, new) \
    pair = ((pair << 8) | ((uint16_t)new & 0xFF))

    for (;;) {
        if (pch > end_buf) {
            if (!fgets(buf, sizeof(char) * CFG_BUFFER_SIZE, f)) {
                if (!feof(f)) {
                    rc = -1; // TODO: result code - error reading file
                    goto full_cleanup;
                }
                else
                    break; // EOF
            }
            pch = buf;
            ch = *pch;
        }
        /* FSM: char-by-char config file parser
         *
         * Treats missing value as an empty value.
         *
         *
         * state         input         new state     action
         * ------------  ------------  ------------  -------------
         * NONE          '#'           SKIP_TO_EOL   -
         * NONE          '"'           QUOTED_NAME   -
         * NONE          !is_ws        NAME          keep char
         * NONE          *             NONE          -
         *
         * NAME          '#'           SKIP_TO_EOL   -
         * NAME          '"'           SKIP_TO_EOL   -
         * NAME          !is_ws        NAME          name.append
         * NAME          '='           ASSIGN        name.store
         * NAME          is_eol        SKIP_TO_EOL   name.discard
         * NAME          is_ws         ASSIGN        name.store
         *
         * QUOTED_NAME   '"'           ASSIGN        name.store
         * QUOTED_NAME   is_eol        SKIP_TO_EOL   name.discard
         * QUOTED_NAME   *             QUOTED_NAME   name.append
         *
         * ASSIGN        '='           VALUE         -
         * ASSIGN        !is_ws        SKIP_TO_EOL   -
         * ASSIGN        is_eol        NONE          name.discard
         *
         * VALUE         '"'           QUOTED_VALUE  -
         * VALUE         is_eol        NONE          value.store
         * VALUE         is_ws   (1)   VALUE         -
         * VALUE         !is_ws        VALUE         value.append
         * VALUE         is_ws   (2)   SKIP_TO_EOL   value.store
         * VALUE         '#'           SKIP_TO_EOL   value.store
         *
         * QUOTED_VALUE  '"'           SKIP_TO_EOL   -
         * QUOTED_VALUE  is_eol        SKIP_TO_EOL   value.discard
         * QUOTED_VALUE  *             QUOTED_VALUE  value.append
         *
         * SKIP_TO_EOL   is_eol        NONE          -
         * SKIP_TO_EOL   *             SKIP_TO_EOL   -
         *
         * ----------------
         * (1) whitespace before the first non-whitespace char
         * (2) whitespace after at least one non-whitespace char
         *
         */
        bool skipping_ws = true;

        switch (state) {
        case STATE_NONE:
            if (ch == '#')
                state = STATE_SKIP_TO_EOL;
            else if (ch == '"') {
                skipping_ws = false;
                state = STATE_QUOTED_NAME;
            }
            else if (!is_ws(ch)) {
                skipping_ws = false;
                state = STATE_NAME;
                continue; // keep the char for the new state
            }
            break;

        case STATE_NAME:
            if (ch == '"')
                state = STATE_QUOTED_NAME;
            else if ((!skipping_ws && is_ws(ch)) || ch == '=') {
                rc = buffer_get_as_string(&name_buf, &name);
                if (!rc)
                    goto full_cleanup;
                rc = buffer_seek(&name_buf, BUF_SEEK_BUFFER_OFFSET, 0,
                                 NULL, (BufferPage **) NULL, NULL);
                if (rc)
                    goto full_cleanup;
                state = (ch == '=') ? STATE_VALUE : STATE_ASSIGN;
            }
            else if (ch == '#')
                state = STATE_SKIP_TO_EOL;
            else if (is_eol(ch)) {
                state = STATE_SKIP_TO_EOL;
            }
            else {
                rc = buffer_append(&name_buf, &ch, 1);
                if (!rc)
                    goto full_cleanup;
            }
            break;

        case STATE_QUOTED_NAME:
            if (ch == '"') {
                rc = buffer_get_as_string(&name_buf, &name);
                if (!rc)
                    goto full_cleanup;
                rc = buffer_seek(&name_buf, BUF_SEEK_BUFFER_OFFSET, 0,
                                    NULL, (BufferPage**)NULL, NULL);
                if (rc)
                    goto full_cleanup;
                state = STATE_ASSIGN;
            }
            else if (is_eol(ch)) {
                state = STATE_SKIP_TO_EOL;
                rc = buffer_seek(&name_buf, BUF_SEEK_BUFFER_OFFSET, 0,
                                 NULL, (BufferPage **) NULL, NULL);
                if (rc)
                    goto full_cleanup;
            }
            else {
                rc = buffer_append(&name_buf, &ch, 1);
                if (!rc)
                    goto full_cleanup;
            }
            break;

        case STATE_ASSIGN:
            if (ch == '=') {
                skipping_ws = true;
                state = STATE_VALUE;
            }
            else if (!is_ws(ch))
                state = STATE_SKIP_TO_EOL;
            else if (is_eol(ch)) {
                state = STATE_NONE;
                rc = buffer_seek(&name_buf, BUF_SEEK_BUFFER_OFFSET, 0,
                                 NULL, (BufferPage **) NULL, NULL);
                if (rc)
                    goto full_cleanup;
            }
            break;

        case STATE_VALUE:
            if (ch == '"') {
                state = STATE_QUOTED_VALUE;
                continue; // keep the same char
            }
            else if (ch == '#' || is_eol(ch) ||
                                    (!skipping_ws && is_ws(ch))) {
                rc = buffer_get_as_string(&value_buf, &value);
                if (rc)
                    goto full_cleanup;
                rc = buffer_seek(&value_buf, BUF_SEEK_BUFFER_OFFSET, 0,
                                    NULL, NULL, NULL);
                if (rc)
                    goto full_cleanup;

                rc = config_set_option(config, &name, &value);
                if (rc)
                    goto full_cleanup;

                state = STATE_SKIP_TO_EOL;
            }
            else if (!is_ws(ch)) {
                skipping_ws = false;
                rc = buffer_append(&value_buf, &ch, 1);
                if (!rc)
                    goto full_cleanup;
            }
            break;

        case STATE_QUOTED_VALUE:
            if (ch == '"') {
                rc = buffer_get_as_string(&value_buf, &value);
                if (!rc)
                    goto full_cleanup;
                rc = buffer_seek(&value_buf, BUF_SEEK_BUFFER_OFFSET, 0,
                                    NULL, (BufferPage**)NULL, NULL);
                if (rc)
                    goto full_cleanup;

                rc = config_set_option(config, &name, &value);
                if (rc)
                    goto full_cleanup;
                state = STATE_SKIP_TO_EOL;
            }
            else if (is_eol(ch)) {
                rc = buffer_seek(&value_buf, BUF_SEEK_BUFFER_OFFSET, 0,
                                    NULL, (BufferPage**)NULL, NULL);
                if (rc)
                    goto full_cleanup;
                state = STATE_SKIP_TO_EOL;
            }
            else {
                rc = buffer_append(&name_buf, &ch, 1);
                if (!rc)
                    goto full_cleanup;
            }
            break;

        case STATE_SKIP_TO_EOL:
            if (is_eol(ch))
                state = STATE_NONE;
            break;
        }

        /* handle end of line chars
         *
         * cr    - single eol
         * lf    - single eol
         * cr+lf - single eol
         * lf+cr - single eol
         */
        if (ch == '\r')
            switch (eols)
            {
            case EOL_PAIR('\0', '\0'):
            case EOL_PAIR('\0', '\r'):
            case EOL_PAIR('\r', '\0'):
            case EOL_PAIR('\r', '\n'):
            case EOL_PAIR('\n', '\0'):
            case EOL_PAIR('\n', '\r'):
                EOL_PAIR_SHIFT(eols, ch);
                // fall through
            case EOL_PAIR('\r', '\r'):
                ++line;
                column = 1;
                break;

            case EOL_PAIR('\n', '\n'):
                eols = EOL_PAIR('\0', '\r');
                ++line;
                column = 1;
                break;

            case EOL_PAIR('\0', '\n'):
                EOL_PAIR_SHIFT(eols, ch);
            default:
                break;
            }
        else if(ch == '\n')
            switch (eols) {
            case EOL_PAIR('\0', '\0'):
            case EOL_PAIR('\0', '\n'):
            case EOL_PAIR('\n', '\0'):
            case EOL_PAIR('\n', '\r'):
            case EOL_PAIR('\r', '\0'):
            case EOL_PAIR('\r', '\n'):
                EOL_PAIR_SHIFT(eols, ch);
                // fall through
            case EOL_PAIR('\n', '\n'):
                ++line;
                column = 1;
                break;

            case EOL_PAIR('\r', '\r'):
                eols = EOL_PAIR('\0', '\n');
                ++line;
                column = 1;
                break;

            case EOL_PAIR('\0', '\r'):
                EOL_PAIR_SHIFT(eols, ch);
            default:
                break;
            }
        else  {
            if (eols)
                EOL_PAIR_SHIFT(eols, ch);
            ++column;
        }
        ch = *(++pch);
    }

full_cleanup:
    string_release(&name);
    string_release(&value);
    buffer_free(&value_buf);
cleanup:
    buffer_free(&name_buf);
exit:
    fclose(f);

    return 0;
}

static
int strscmp(const char *safe, const char *unsafe)
{
    for (; *safe; ++safe, ++unsafe)
        if (*safe < *unsafe)
            return -1;
        else if (*safe > *unsafe)
            return 1;
    if (*unsafe)
        return 1;

    return 0;
}


int config_set_option(Config *config, const String *name,
                      const String *value)
{
    assert(config);

    int rc;

    for (OptionDefinition *od = config->option_defs; od->id != -1; ++od) {
        if (!strscmp(od->name, name->chars)) {
            String *str = (String *)((void *)config + od->offset);
            rc = string_copy(str, value);
            if (rc)
                return rc; // TODO: result code - unable to copy
            return 0;
        }
    }

    return -1; // TODO: result code - unknown option
}
