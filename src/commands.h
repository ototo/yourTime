/* yourTime
 *
 * Command implementation.
 *
 * TODO: complete header (license, author, etc)
 */

#pragma once

#include "config.h"
#include "database.h"

/* Command IDs */
enum _CommandId {
    CMD_STATUS = 0,
    CMD_START,
    CMD_SWITCH,
    CMD_STOP,
};

typedef enum _CommandId CommandId;


/* Command Flags */
enum _CommandFlag {
    CMDF_DEFAULT    = 0x00000001,
    CMDF_READS_DB   = 0x00000002,
    CMDF_WRITES_DB  = 0x00000004,
};

typedef enum _CommandFlag CommandFlag;


typedef int (*CommandHandler)(Config *cfg, Database *db,
                              int argc, char *argv[]);

struct _CommandDefinition
{
    char            *name;
    int             length;
    CommandId       id;
    int             flags;
    CommandHandler  handler;
    char            *help;
};

typedef struct _CommandDefinition CommandDefinition;

int command_process_args(Config *cfg, Database *db, int argc, char *argv[]);
int command_process(CommandDefinition *cmd, Config *cfg, Database *db,
                    int argc, char *argv[]);
