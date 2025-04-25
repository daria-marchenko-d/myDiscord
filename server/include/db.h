#ifndef DB_H

#define DB_H

#include <libpq-fe.h> // PostgreSQL client library

// Initializes database connection

int db_init();

// Closes the database connection

void db_close();

// Registers a new user

int add_user(const char *first_name, const char *last_name, const char *email, const char *hashed_password);

// Authenticates a user during login

int authenticate_user(const char *email, const char *hashed_password, int *user_id, char *role);

#endif
