/* yourTime
 *
 * Database implementation (SQLite based).
 *
 * TODO: complete heared (license, author, etc)
 */

#include <stdint.h>
#include <error.h>
#include <assert.h>
#include <sqlite.h>

#include "database.h"
#include "buffer.h"

struct _ColumnDefinition
{
    char                *name;
    char                *type;
};

typedef struct _ColumnDefinition ColumnDefinition;

struct _TableDefinition
{
    char                *name;
    ColumnDefinition    *columns;
};

typedef struct _TableDefinition TableDefinition;


ColumnDefinition columns_Event[] =
{
    {
        .name = "id",
        .type = "INTEGER PRIMARY KEY",
    },
    {
        .name = "utc_ts",
        .type = "INTEGER NOT NULL",
    },
    {
        .name = "type",
        .type = "TEXT",
    },
    {
        .name = NULL,
    },
};

ColumnDefinition columns_Author[] =
{
    {
        .name = "id",
        .type = "INTEGER PRIMARY KEY",
    },
    {
        .name = "name",
        .type = "TEXT",
    },
    {
        .name = "email",
        .type = "TEXT",
    },
    {
        .name = NULL,
    },
};


TableDefinition tables[] =
{
    {
        .name = "Event",
        .columns = columns_Event,
    },
    {
        .name = "Author",
        .columns = columns_Author,
    },
    {
        .name = NULL,
    },
};


int db_init(Database *db, Config *cfg)
{
    assert(cfg);
    assert(db);

    memset(db, 0, sizeof(db));
    db->config = cfg;
    db->file_name = cfg->database_file;

    return 0;
}

int db_free(Database *db)
{
    assert(db);

    if (db->db) {
        sqlite3_close(db->db);
        db->db = 0;
        return 0;
    }

    return -1;
}

int db_open(Database *db, int mode)
{
    int rc;

    rc = sqlite3_open_v2(db->file_name, &db->db, mode, NULL);
    if (rc) {
        fprintf(stderr, "Can't open SQLite database: %s\n",
                sqlite3_errmsg(db->db));
        sqlite3_close(db->db);
        db->db = 0;
    }

    return rc;
}

int db_is_open(Database *db)
{
    return db->db != NULL;
}

int db_close(Database *db)
{
    int rc;

    rc = sqlite3_close(db->db);
    if (rc == SQLITE_OK)
        db->db = NULL;

    return rc;
}

#define SQL_CREATE_HEAD     "CREATE TABLE "
#define SQL_CREATE_FIELDS   " ("
#define SQL_CREATE_SPACE    " "
#define SQL_CREATE_LIST     ", "
#define SQL_CREATE_END      "); "

#define BUFFER_APPEND(buf, str) \
        rc = buffer_append(buf, str, -1);\
        if (rc) goto error_cleanup;

int db_create_schema(Database *db)
{
    Buffer *buf;

    int rc = buffer_alloc(512, &buf);
    if (buf)
        return rc;

    int first;
    for (TableDefinition *tbl = tables; tbl->name; ++tbl) {
        BUFFER_APPEND(&buf, SQL_CREATE_HEAD)
        BUFFER_APPEND(&buf, tbl->name)
        BUFFER_APPEND(&buf, SQL_CREATE_FIELDS)
        first = 1;
        for (ColumnDefinition *col = tbl->columns; col->name; ++col) {
            if (first) {
                first = 0;
                BUFFER_APPEND(&buf, SQL_CREATE_LIST)
            }
            BUFFER_APPEND(&buf, col->name)
            BUFFER_APPEND(&buf, SQL_CREATE_SPACE)
            BUFFER_APPEND(&buf, col->type)
        }
        BUFFER_APPEND(&buf, SQL_CREATE_END)
    }

    char *sql;
    rc = buffer_get_as_string(&buf, &sql);
    if (rc)
        goto error_cleanup;

    char *errmsg;
    rc = sqlite3_exec(db->db, sql, NULL, NULL, &errmsg);
    if (rc) {
        fprintf(stderr, "Can't create database: %s\n",
                sqlite3_errmsg(db->db));
        goto error_cleanup_sql;
    }

    return rc;

error_cleanup_sql:
    buffer_free_string(&sql);

error_cleanup:
    buffer_free(&buf);

    return rc;
    //char *errmsg;

    /*
    rc = sqlite3_exec(db->db, SQLQ_CREATE_DB, NULL, NULL, &errmsg);
    if (rc)
        fprintf(stderr, "Can't create database: %s\n",
                sqlite3_errmsg(db->db));
    */

    return rc;
}

int db_update_schema(Database *db)
{
    return -1;
}

int db_check_schema(Database *db)
{

    return -1;
}

int db_start_activity(Database *db, int actvc, Activity **actvs)
{
    return -1;
}

int db_switch_activity(Database *db, int actvc, Activity **actvs)
{
    return -1;
}

int db_stop_activity(Database *db, int actvc, Activity **actvs)
{
    return -1;
}

int db_alloc_string(int chars, char **str)
{
    char *buffer;
    buffer = (char *)calloc(chars + 1, 1); /* adjust for '\0' */

    if (!buffer)
        return -1; /* TODO: error code */

    *str = buffer;

    return 0;
}

int db_append_string(char *source, int *chars, char **str)
{
    return 0;
}

int db_free_string(char **str)
{
    free(*str);
    *str = NULL;

    return 0;
}

int db_alloc_activities(int actvc, Activity ***actvs)
{
    uint8_t *buffer;

    buffer = calloc(actvc * (sizeof(Activity *) + sizeof(Activity)), 1);
    if (!buffer)
        return -1; /* TODO: error code */

    for (int idx = 0; idx < actvc; ++idx)
        ((Activity **)buffer)[idx] = (Activity *)(buffer
            + idx * (sizeof(Activity *) + sizeof(Activity)));

    *actvs = (Activity **)buffer;

    return 0;
}

int db_free_activities(Activity ***actvs)
{
    free(*actvs);
    *actvs = NULL;

    return 0;
}

