#include <libpq-fe.h>
#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static PGconn *conn = NULL; // Database connection object

int db_init()

{
    // Connect to the PostgreSQL database with user, db name, and password

    conn = PQconnectdb("host=localhost port=5432 user=postgres dbname=myDiscord password=0000");

    // Check if connection was successful

    if (PQstatus(conn) != CONNECTION_OK)

    {

        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));

        return 0;
    }

    printf("Connected to database successfully.\n");

    return 1;
}

void db_close()

{

    // Close the database connection

    PQfinish(conn);
}

int add_user(const char *first_name, const char *email, const char *hashed_password)

{

    const char *params[3] = {first_name, email, hashed_password};

    // Insert new user into UserAccount table

    PGresult *res = PQexecParams(conn,

                                 "INSERT INTO UserAccount (first_name, email, password_hash) VALUES ($1, $2, $3)",

                                 3, NULL, params, NULL, NULL, 0);

    // Check if insertion was successful

    if (PQresultStatus(res) != PGRES_COMMAND_OK)

    {

        fprintf(stderr, "Registration failed: %s", PQerrorMessage(conn));

        PQclear(res);

        return 0;
    }

    PQclear(res);

    return 1;
}

int authenticate_user(const char *email, const char *hashed_password)

{

    const char *params[2] = {email, hashed_password};

    // Query the user by email and password hash

    PGresult *res = PQexecParams(conn,

                                 "SELECT 1 FROM UserAccount WHERE email=$1 AND password_hash=$2",

                                 2, NULL, params, NULL, NULL, 0);

    // Check if exactly one user matched

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) != 1)
    {

        PQclear(res);

        return 0; // Invalid login
    }

    // Extract user ID and role from the result

    // *user_id = atoi(PQgetvalue(res, 0, 0));

    // strcpy(role, PQgetvalue(res, 0, 1));

    PQclear(res);

    return 1;
}

int get_user_role(const char *email, char *role_buffer)
{
    const char *params[1] = {email};
    PGresult *res = PQexecParams(conn,
                                 "SELECT role FROM UserAccount WHERE email=$1",
                                 1, NULL, params, NULL, NULL, 0);

    if (PQntuples(res) == 1)
    {
        strncpy(role_buffer, PQgetvalue(res, 0, 0), 20);
        PQclear(res);
        return 1;
    }
    PQclear(res);
    return 0;
}

// Add or update emoji in the database
int add_or_update_reaction(int message_id, int user_id, const char *emoji)
{
    const char *params[3];
    char msg_id_str[12], user_id_str[12];

    snprintf(msg_id_str, sizeof(msg_id_str), "%d", message_id);
    snprintf(user_id_str, sizeof(user_id_str), "%d", user_id);

    params[0] = msg_id_str;
    params[1] = user_id_str;
    params[2] = emoji;

    PGresult *res = PQexecParams(conn,
                                 "INSERT INTO Reaction (message_id, user_id, emoji) "
                                 "VALUES ($1, $2, $3) "
                                 "ON CONFLICT (message_id, user_id) "
                                 "DO UPDATE SET emoji = EXCLUDED.emoji",
                                 3, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Failed to add or update reaction: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 0;
    }

    PQclear(res);
    return 1;
}

// Remove emoji from the database
int remove_reaction(int message_id, int user_id)
{
    const char *params[2];
    char msg_id_str[12], user_id_str[12];

    snprintf(msg_id_str, sizeof(msg_id_str), "%d", message_id);
    snprintf(user_id_str, sizeof(user_id_str), "%d", user_id);

    params[0] = msg_id_str;
    params[1] = user_id_str;

    PGresult *res = PQexecParams(conn,
                                 "DELETE FROM Reaction WHERE message_id=$1 AND user_id=$2",
                                 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Failed to remove reaction: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 0;
    }

    PQclear(res);
    return 1;
}
