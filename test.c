#include "db.h"
#include <stdio.h>

// Prototype with crypto.c
void hash_password(const char *password, char *output_hex);

int main()
{
    if (!db_init())
    {
        printf("❌ Could not connect to the database.\n");
        return 1;
    }

    char hashed_password[65];
    hash_password("test123", hashed_password);

    if (add_user("Daria", "Test", "daria@example.com", hashed_password))
    {
        printf("✅ User registered successfully.\n");
    }
    else
    {
        printf("❌ Registration failed.\n");
    }

    int user_id;
    char role[20];
    if (authenticate_user("daria@example.com", hashed_password, &user_id, role))
    {
        printf("✅ Login successful! ID: %d, Role: %s\n", user_id, role);
    }
    else
    {
        printf("❌ Login failed.\n");
    }

    db_close();
    return 0;
}
