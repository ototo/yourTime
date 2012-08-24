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

#define BUF_SEEK_PAGE_OFFSET 0
#define BUF_SEEK_BUFFER_OFFSET 1

int db_seek_buffer(Buffer *buffer, int seek_type,
                   int seek_offset, int *seek_page,
                   BufferPage **page, int *page_offset)
{
    assert(buffer);

    int target_page;
    int offset;

    switch (seek_type)
    {
    case BUF_SEEK_PAGE_OFFSET:
        target_page = *seek_page;
        offset = seek_offset;
        break;
    case BUF_SEEK_BUFFER_OFFSET:
        {
        div_t q = div(seek_offset, buffer->page_size);
        target_page = q.quot;
        offset = q.rem;
        }
        break;
    }

    BufferPage *p = buffer->tail;
    int idx;

    for (idx = 0; p && idx < target_page; p = p->next, ++idx) ;

    if (idx != target_page)
        return -1; // TODO: return code - out of bounds

    *seek_page = target_page;
    *page = p;
    *page_offset = offset;

    return 0;
}

static
int db_add_buffer_pages(Buffer **buffer, int pages,
                        int used, int zero_data)
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
    // TODO: set used

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

    if (!string || !size)
        return 0;

    Buffer *buf = *buffer;
    int string_len = strlen(string);
    int left_to_append = (size > 0)
                       ? ((size < string_len) ? size : string_len)
                       : string_len;
    char *chars_to_append = string;

    while (left_to_append) {
        BufferPage *head = buf->head;
        if (!head || (buf->tip_page_used == buf->page_size)) {
            db_add_buffer_pages(buffer, 1, -1, 0);
            continue;
        }
        else {
            int available = buf->page_size - buf->tip_page_used;
            int to_copy = (available < left_to_append)
                        ? available
                        : left_to_append;
            memcpy(head->data, chars_to_append, to_copy);
            left_to_append -= to_copy;
            chars_to_append += to_copy;
            buf->tip_page_used += to_copy;
            buf->used += to_copy;
        }
    }

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

