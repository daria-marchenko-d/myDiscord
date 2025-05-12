#include <stdio.h>
#include <sqlite3.h>

int main() {
    sqlite3 *db = NULL;
    int rc = sqlite3_open("../sql/mydiscord.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erreur: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    printf("Ouverture réussie.\n");
    sqlite3_close(db);
    return 0;
}
