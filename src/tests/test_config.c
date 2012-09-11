/* yourTime
 *
 * Configuration test set file.
 *
 * TODO: complete header (license, author, etc)
 */

#define _XOPEN_SOURCE 600

#include "test.h"
#include "../config.h"


/* Test for initialization and uninitialization routines.
 *
 */

START_TEST(config, config_init)

    ShortSwitchDefinition ssws[] =
    {
        { .id = -1, },
    };

    LongSwitchDefinition lsws[] =
    {
        { .id = -1, },
    };

    OptionDefinition opts[] =
    {
        { .id = -1, },
    };

    Config cfg;

    int rc = config_init(&cfg, ssws, lsws, opts);

    TEST_EQUAL(rc, 0);
    TEST_EQUAL(cfg.short_switch_defs, ssws);
    TEST_EQUAL(cfg.long_switch_defs, lsws);
    TEST_EQUAL(cfg.option_defs, opts);
    TEST_EQUAL(cfg.verbosity, CFG_DEFAULT_VERBOSITY);

    // TODO: add more tests as more items are added into the
    // configuration structure.

    rc = config_free(&cfg);

    TEST_EQUAL(rc, 0);

    // TODO: add more tests as more items are added into the
    // configuration structure.

END_TEST


/* Test for argument processor.
 *
 */

static
void TestSwitchProcessor(AnySwitch *option, Config *config)
{
}


START_TEST(config, config_process_args)

    char *argv[] = {
    };

    int argc = sizeof(argv) / sizeof(argv[0]);

    ShortSwitchDefinition ssws[] =
    {
        { .id = -1, },
    };

    LongSwitchDefinition lsws[] =
    {
        { .id = -1, },
    };

    OptionDefinition opts[] =
    {
        { .id = -1, },
    };

    Config cfg;

    int rc = config_init(&cfg, ssws, lsws, opts);

    TEST_EQUAL(rc, 0);

    int free_args = 0;

    rc = config_process_args(&cfg, argc, argv, TestSwitchProcessor,
                                    &free_args);

    TEST_EQUAL(rc, 0);

    rc = config_free(&cfg);

    TEST_EQUAL(rc, 0);

END_TEST


START_TEST(config, config_parse_file)

    TEST_NOT_IMPLEMENTED;

END_TEST


START_TEST(config, config_set_option)

    TEST_NOT_IMPLEMENTED;

END_TEST
