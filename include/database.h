#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

int init_db(const char *filename);
sqlite3 *get_db(void);

// Chargement des messages avec auteur
int load_channel_messages(int channel_id, void (*callback)(const char *msg));

// Sauvegarde de message avec user_id
int save_message_with_user(const char *content, int user_id);

#endif
