#ifndef DB_H

#define DB_H
#include "crypto.h"
#include <libpq-fe.h> // PostgreSQL client library

// Initializes database connection

int db_init();

// Closes the database connection

void db_close();

int get_user_role(const char *email, char *role_buffer);

// Registers a new user

int add_user(const char *first_name, const char *email, const char *hashed_password);

// Authenticates a user during login

int authenticate_user(const char *email, const char *hashed_password);

// Adds or updates a reaction (emoji) to a message
int add_or_update_reaction(int message_id, int user_id, const char *emoji);

// Removes a reaction from a message
int remove_reaction(int message_id, int user_id);

#endif
