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

int add_user(const char *first_name, const char *last_name, const char *email, const char *hashed_password)

{

    const char *params[4] = {first_name, last_name, email, hashed_password};

    // Insert new user into UserAccount table

    PGresult *res = PQexecParams(conn,

                                 "INSERT INTO UserAccount (first_name, last_name, email, password_hash) VALUES ($1, $2, $3, $4)",

                                 4, NULL, params, NULL, NULL, 0);

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

int authenticate_user(const char *email, const char *hashed_password, int *user_id, char *role)

{

    const char *params[2] = {email, hashed_password};

    // Query the user by email and password hash

    PGresult *res = PQexecParams(conn,

                                 "SELECT user_id, role FROM UserAccount WHERE email=$1 AND password_hash=$2",

                                 2, NULL, params, NULL, NULL, 0);

    // Check if exactly one user matched

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) != 1)
    {

        PQclear(res);

        return 0; // Invalid login
    }

    // Extract user ID and role from the result

    *user_id = atoi(PQgetvalue(res, 0, 0));

    strcpy(role, PQgetvalue(res, 0, 1));

    PQclear(res);

    return 1;
}
