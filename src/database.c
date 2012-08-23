/* yourTime
 *
 * Database implementation (SQLite based).
 *
 * TODO: complete heared (license, author, etc)
 */

#include <stdint.h>
#include <stdlib.h>
#include <error.h>
#include <assert.h>
#include <sqlite.h>

#include "database.h"

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

int db_create_schema(Database *db)
{
    int rc;
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

static
int db_add_buffer_pages(Buffer **buffer, int pages,
                        int last_page_offset, int zero_data)
{
    assert(buffer);

    if (!*buffer)
        return -1; /* TODO: error code */

    Buffer *buf = *buffer;
    if (!buf->page_size) {
        fprintf(stderr,
                "%s - zero page size is a nonsense!\n", __func__);
        return -1; // TODO: error code
    }

    int page_size = buf->page_size;

    for (int page_nr = 0; page_nr < pages; ++page_nr) {
        BufferPage *page = malloc(sizeof(BufferPage) + page_size);
        if (!page)
            return -1; // TODO: error code

        memset(page, 0, sizeof(BufferPage) + (zero_data ? page_size : 1));

        if (!buf->head)
            buf->tail = page;
        else
            buf->head->next = page;
        buf->pages += 1;
        buf->size += buf->page_size;
        buf->head = page;
    }

    return 0;
}

int db_alloc_buffer(int page_size, Buffer **buffer)
{
    Buffer *buf;

    buf = (Buffer *)malloc(sizeof(Buffer));
    if (!buf)
        return -1; /* TODO: error code */

    memset(buf, 0, sizeof(Buffer));

    buf->page_size = page_size;

    int rc = db_add_buffer_pages(&buf, 1, 0, 0);
    if (rc) {
        free(buf);
        return rc; // TODO: report error
    }

    *buffer = buf;

    return 0;
}

int db_append_buffer(Buffer **buffer, char *string, int size)
{
    assert(buffer);

    if (!*buffer)
        return -1; /* TODO: error code */

    if (!string)
        return 0;

    return 0;
}

int db_buffer_get_as_string(Buffer **buffer, char **string)
{
    assert(buffer);

    return 0;
}

int db_buffer_trim(Buffer **buffer, int new_size)
{
    assert(*buffer);

    if (!*buffer)
        return -1; // TODO: error code

    return 0;
}

int db_free_buffer(Buffer **buffer)
{
    assert(buffer);

    if (!*buffer)
        return 0;

    BufferPage *current;
    BufferPage *page = (*buffer)->tail;
    while (page) {
        current = page;
        page = page->next;
        free(current);
    }
    free(buffer);

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

