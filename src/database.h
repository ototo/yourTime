/* yourTime
 *
 * Database implementation (SQLite based).
 *
 * TODO: complete header (license, author, etc)
 */

#pragma once

#include <stdlib.h>
#include <sqlite3.h>
#include <time.h>

#include "config.h"

/* Event
 *
 * Represents a change in activities.
 */

struct _Activity
{
    int             id;
    char            *summary;
};

typedef struct _Activity Activity;



/* Database
 *
 * Database abstraction.
 */

struct _Database {
    Config          *config;
    char            *file_name;
    sqlite3         *db;
};

typedef struct _Database Database;

int db_init(Database *db, Config *cfg);
int db_free(Database *db);
int db_open(Database *db, int mode);
int db_is_open(Database *db);
int db_close(Database *db);

int db_start_activity(Database *db, Activity **actvs);
int db_switch_activity(Database *db, int actvc, Activity **actvs);
int db_stop_activity(Database *db, int actvc, Activity **actvs);

Activity* db_alloc_activity(void);
void db_free_activity(int actvc, Activity **actvs);
