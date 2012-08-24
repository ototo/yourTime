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

/* paged buffer structures */
struct _BufferPage;
typedef struct _BufferPage BufferPage;

struct _BufferPage
{
    BufferPage  *next;      /* next page */
    char        data[];     /* data buffer itself */
};

struct _Buffer
{
    int         page_size;          /* page size */
    int         pages;              /* number of allocated pages */
    int         size;               /* allocated total size (all pages) */
    int         used;               /* total used size */
    int         tip_page_used;      /* used on the tip page */
    int         tip_page_offset;    /* to the first unused byte */
    BufferPage  *head;              /* most recently added page */
    BufferPage  *tail;              /* the oldest page */
    BufferPage  *tip;               /* the page containing the next free
                                       byte */
};

typedef struct _Buffer Buffer;


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

int db_create_schema(Database *db);
int db_update_schema(Database *db);
int db_check_schema(Database *db);

int db_start_activity(Database *db, int actvc, Activity **actvs);
int db_switch_activity(Database *db, int actvc, Activity **actvs);
int db_stop_activity(Database *db, int actvc, Activity **actvs);

int db_alloc_string(int chars, char **str);
int db_free_string(char **str);

int db_alloc_buffer(int page, Buffer **buffer);
int db_append_buffer(Buffer **buffer, char *string, int size);
int db_buffer_get_as_string(Buffer **buffer, char **string);
int db_buffer_trim(Buffer **buffer, int new_size);
int db_free_buffer(Buffer **buffer);

int db_alloc_activities(int actvc, Activity ***actvs);
int db_free_activities(Activity ***actvs);
