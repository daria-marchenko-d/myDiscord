#include "server.h"
#include "database.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <sqlite3.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 4242
#define BUFFER_SIZE 1024

int target_sock = -1;

void send_line_to_client(const char *msg) {
    if (target_sock >= 0 && msg) {
        send(target_sock, msg, strlen(msg), 0);
        send(target_sock, "\n", 1, 0);
    }
}

// modifié pour renvoyer aussi l'id de l'utilisateur
int verify_credentials(const char *email, const char *password_hash, int *user_id_out) {
    const char *sql = "SELECT id FROM users WHERE email = ? AND password_hash = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(get_db(), sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password_hash, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            *user_id_out = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
            return 1;
        }
        sqlite3_finalize(stmt);
    }
    return 0;
}

int register_user(const char *email, const char *hash, const char *fname, const char *lname) {
    const char *check = "SELECT COUNT(*) FROM users WHERE email = ?";
    sqlite3_stmt *stmt;
    int exists = 0;

    if (sqlite3_prepare_v2(get_db(), check, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            exists = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    if (exists > 0) return 0;

    const char *sql = "INSERT INTO users (firstname, lastname, email, password_hash) VALUES (?, ?, ?, ?)";
    if (sqlite3_prepare_v2(get_db(), sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, fname, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, lname, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, email, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, hash, -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return 1;
    }
    return 0;
}

DWORD WINAPI client_handler(LPVOID arg) {
    int client_sock = *(int *)arg;
    free(arg);
    char buffer[BUFFER_SIZE];

    int len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (len <= 0) {
        closesocket(client_sock);
        return 0;
    }
    buffer[len] = '\0';

    // Traitement inscription
    if (strncmp(buffer, "REGISTER:", 9) == 0) {
        char *parts[5];
        int i = 0;
        char *token = strtok(buffer + 9, ":");
        while (token && i < 5) {
            parts[i++] = token;
            token = strtok(NULL, ":");
        }

        if (i == 4) {
            int ok = register_user(parts[0], parts[1], parts[2], parts[3]);
            send(client_sock, ok ? "OK" : "EMAIL_EXISTS", ok ? 2 : 12, 0);
        } else {
            send(client_sock, "ERROR", 5, 0);
        }

        closesocket(client_sock);
        return 0;
    }

    // Traitement connexion
    char *sep = strchr(buffer, ':');
    if (!sep) {
        send(client_sock, "DENIED", 6, 0);
        closesocket(client_sock);
        return 0;
    }

    *sep = '\0';
    const char *email = buffer;
    const char *password_hash = sep + 1;
    int user_id = -1;

    if (!verify_credentials(email, password_hash, &user_id)) {
        send(client_sock, "DENIED", 6, 0);
        closesocket(client_sock);
        return 0;
    }

    send(client_sock, "OK", 2, 0);
    printf("✅ Connexion acceptée pour %s (user_id=%d)\n", email, user_id);

    target_sock = client_sock;
    send_line_to_client("📜 Historique des messages :");
    load_channel_messages(1, send_line_to_client);

    // Boucle réception messages
    while (1) {
        int len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0) break;
        buffer[len] = '\0';

        printf("💬 %s\n", buffer);
        save_message_with_user(buffer, user_id);
    }

    closesocket(client_sock);
    return 0;
}

void start_server(int port) {
    WSADATA wsa;
    SOCKET server_sock;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);

    if (!init_db("../sql/mydiscord.db")) {
        printf("❌ Erreur DB\n");
        return;
    }    

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("❌ Erreur WSA\n");
        return;
    }

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == INVALID_SOCKET) {
        printf("❌ Erreur socket\n");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("❌ Erreur bind\n");
        return;
    }

    listen(server_sock, 10);
    printf("🟢 Serveur en écoute sur le port %d...\n", port);

    while (1) {
        SOCKET client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock == INVALID_SOCKET) continue;

        int *pclient = malloc(sizeof(int));
        *pclient = client_sock;
        CreateThread(NULL, 0, client_handler, pclient, 0, NULL);
    }

    closesocket(server_sock);
    WSACleanup();
}
