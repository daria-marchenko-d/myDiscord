#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "db.h"
#include "ui_callback.h"
#include "config.h"

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET socket_t;
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int socket_t;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

#define SERVER_IP "127.0.0.1"

static socket_t sock = INVALID_SOCKET;

// Initialisation du socket client
int init_client_socket()
{
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        fprintf(stderr, "Erreur WSAStartup\n");
        return 0;
    }
#endif

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        perror("Socket creation failed");
        return 0;
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT),
        .sin_addr.s_addr = inet_addr(SERVER_IP)};

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connexion serveur");
        return 0;
    }

    printf("Connected to server %s:%d\n", SERVER_IP, SERVER_PORT);
    return 1;
}

// Fonction GTK de démarrage
static void on_activate(GtkApplication *app, gpointer user_data)
{
    g_print("DEBUG: on_activate called\n");

    GtkWidget *welcome_window = create_welcome_window(app);
    if (!welcome_window)
    {
        g_error("Failed to create welcome window");
        return;
    }
    gtk_window_present(GTK_WINDOW(welcome_window));
    g_print("Welcome window should be visible now\n");
}

// Fonction appelée à la fin de l'application
static void on_app_shutdown(GApplication *app, gpointer user_data)
{
    if (sock != INVALID_SOCKET)
    {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }

    db_close();

#ifdef _WIN32
    WSACleanup();
#endif
}

int main(int argc, char *argv[])
{
    // Initialize database connection
    if (!db_init())
    {
        fprintf(stderr, "Failed to initialize database connection.\n");
        return EXIT_FAILURE;
    }

    // Initialize client socket connection
    if (!init_client_socket())
    {
        db_close();
        return EXIT_FAILURE;
    }

    // Create GTK application
    GtkApplication *app = gtk_application_new("org.example.mydiscord", G_APPLICATION_DEFAULT_FLAGS);
    if (!app)
    {
        fprintf(stderr, "Failed to create GtkApplication.\n");
        closesocket(sock);
        db_close();
#ifdef _WIN32
        WSACleanup();
#endif
        return EXIT_FAILURE;
    }

    // Verify GTK is initialized
    if (!gtk_is_initialized())
    {
        fprintf(stderr, "GTK not initialized\n");
        g_object_unref(app);
        return EXIT_FAILURE;
    }

    // Connect GTK signals
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    g_signal_connect(app, "shutdown", G_CALLBACK(on_app_shutdown), NULL);

    // Run GTK application
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    // Cleanup
    g_object_unref(app);

    return status;
}
// int main(int argc, char *argv[]) {
//     // Initialize database connection
//     if (!db_init())
//     {
//         fprintf(stderr, "Failed to initialize database connection.\n");
//         return 1;
//     }

//     // Create GTK application
//             GtkApplication *app = gtk_application_new("org.example.mydiscord", G_APPLICATION_DEFAULT_FLAGS);
//             if (app == NULL)
//             {
//                 fprintf(stderr, "Failed to create GtkApplication.\n");
//                 db_close();
//                 return 1;
//             }

//             g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), app);

//             // Run the application
//             int status = g_application_run(G_APPLICATION(app), argc, argv);

//             // Cleanup
//             g_object_unref(app);
//             db_close(); // Close the database connection

//             return status;
//         }