/* yourTime
 *
 * Database implementation (SQLite based).
 *
 * TODO: complete heared (license, author, etc)
 */

#include <sqlite.h>
#include <error.h>
#include <assert.h>

#include "database.h"

int db_init(Config *cfg, Database *db)
{
    assert(cfg);
    assert(db);

    int rc = 0;

    memset(db, 0, sizeof(db));
    db->config = cfg;
    db->file_name = cfg->database_file;

    rc = sqlite3_open_v2(db->file_name, &db->db,
                         SQLITE_OPEN_READWRITE, NULL);
    if (rc) {
        fprintf(stderr, "Can't open SQLite database: %s\n",
                sqlite3_errmsg(db->db));
        sqlite3_close(db->db);
        db->db = 0;
        return -1;
    }

    return -100;
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

int db_open(Database *db, char *file)
{
    return -1;
}

int db_close(Database *db)
{
    return -1;
}

int db_add_event(Database *db, Event *event)
{
    return -1;
}

