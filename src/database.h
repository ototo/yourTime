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

struct Event
{
    int             id;
    time_t          timestamp;
    char            *summary;
};

typedef struct _Event Event;



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

int db_init(Config *cfg, Database *db);
int db_free(Database *db);
int db_open(Database *db, char *file);
int db_close(Database *db);
int db_add_event(Database *db, Event *event);

