#include <string.h>
#include <stdio.h>
#include <sqlite3.h>
#include "database.h"

static sqlite3 *db = NULL;

int init_db(const char *db_path) {
    sqlite3 *tmp = NULL;
    int rc = sqlite3_open(db_path, &tmp);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "❌ sqlite3_open('%s') failed: %s\n", db_path, sqlite3_errmsg(tmp));
            fflush(stderr); // force l'affichage

        sqlite3_close(tmp);  // propre
        return 0;
    }
    db = tmp;
    return 1;
}


sqlite3 *get_db(void) {
    return db;
}

int save_message_with_user(const char *content, int user_id) {
    const char *sql = "INSERT INTO messages (channel_id, user_id, content) VALUES (1, ?, ?)";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("❌ Erreur SQL insert: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, content, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return 1;
}

int load_channel_messages(int channel_id, void (*callback)(const char *msg)) {
    const char *sql =
        "SELECT users.firstname || ' ' || users.lastname || ' : ' || messages.content "
        "FROM messages JOIN users ON users.id = messages.user_id "
        "WHERE channel_id = ? ORDER BY timestamp ASC";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("❌ Erreur prepare: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, channel_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *msg = (const char *)sqlite3_column_text(stmt, 0);
        if (msg && callback) {
            callback(msg);
        }
    }

    sqlite3_finalize(stmt);
    return 1;
}
