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

int db_start_activity(Database *db, Activity **actvs)
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

Activity* db_alloc_activity(void)
{
    return (Activity*)calloc(1, sizeof(Activity));
}

void db_free_activity(int actvc, Activity **actvs)
{
    for (int idx = 0; idx < actvc ; ++idx) {
        if (actvs[idx]->summary)
            free(actvs[idx]->summary);
        free(actvs[idx]);
    }
}

