/* yourTime
 *
 * Configuration test set file.
 *
 * TODO: complete header (license, author, etc)
 */

#define _XOPEN_SOURCE 600

#include "test.h"
#include "config.h"


START_TEST(config, config_init)

    Config cfg;

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

    int rc = config_init(&cfg, ssws, lsws, opts);

    TEST_EQUAL(rc, 0);
    TEST_EQUAL(cfg.short_switch_defs, ssws);
    TEST_EQUAL(cfg.long_switch_defs, lsws);
    TEST_EQUAL(cfg.option_defs, opts);
    TEST_EQUAL(cfg.verbosity, CFG_DEFAULT_VERBOSITY);

END_TEST


START_TEST(config, config_process_args)

    *((int *)0) = 0; // SIGSEGV
    TEST_TRUE(false);

END_TEST


START_TEST(config, config_parse_file)

    TEST_TRUE(false);
    raise(SIGINT);

END_TEST


START_TEST(config, config_set_option)

    TEST_TRUE(false);
    raise(SIGFPE);

END_TEST
