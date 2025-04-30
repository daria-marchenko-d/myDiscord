#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET socket_t;
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int socket_t;
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR   -1
    #define closesocket close
#endif

#include "ui.h"
#include "ui_callbacks.h"
#include "db.h" // Ajout de l'inclusion pour la base de données

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345

socket_t sock = INVALID_SOCKET; // Initialisation à INVALID_SOCKET

// Fonction appelée à la fin de l'application
static void on_app_shutdown(GApplication *app, gpointer user_data) {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
#ifdef _WIN32
        WSACleanup();
#endif
        sock = INVALID_SOCKET; // Réinitialiser pour éviter les doubles fermetures
    }
    db_close(); // Fermeture de la connexion à la base de données
}

// Envoi de message brut (à compléter avec format)
void send_message_to_server(const char *message) {
    if (sock != INVALID_SOCKET) {
        int bytes_sent = send(sock, message, strlen(message), 0);
        if (bytes_sent == SOCKET_ERROR) {
            perror("Erreur lors de l'envoi du message");
            // Gérer l'erreur (par exemple, fermer la connexion)
        }
    }
}

// Fonction GTK de démarrage
static void on_activate(GtkApplication *app, gpointer user_data) {
    // Create the login window
    GtkWidget *login_window = create_login_window();
    gtk_widget_show_all(login_window);

    // Create the main window (but don't show it yet)
    GtkWidget *main_window = create_main_window();

    // Pass the socket and windows to the callbacks
    login_data_t *login_data = g_new(login_data_t, 1);
    login_data->socket = sock; // Use the global socket
    login_data->login_window = login_window;
    login_data->main_window = main_window;

    // Get the login and register buttons from the builder
    GtkBuilder *builder = gtk_builder_new_from_file("src/ui.glade"); // Adjust path if needed
    if (!builder) {
        fprintf(stderr, "Erreur lors du chargement du fichier ui.glade\n");
        // Gérer l'erreur (par exemple, afficher un message d'erreur et quitter)
        return;
    }
    GtkWidget *login_button = GTK_WIDGET(gtk_builder_get_object(builder, "login_button"));
    GtkWidget *register_button = GTK_WIDGET(gtk_builder_get_object(builder, "register_button"));

    // Connect the callbacks
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), login_data);
    g_signal_connect(register_button, "clicked", G_CALLBACK(on_register_button_clicked), login_data);

    g_object_unref(builder); // Free the builder
}

// Initialisation du socket client
int init_client_socket() {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "Erreur WSAStartup\n");
        return 0;
    }
#endif

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        perror("Socket");
        return 0;
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT),
        .sin_addr.s_addr = inet_addr(SERVER_IP)
    };

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connexion serveur");
        closesocket(sock); // Close the socket on error
        sock = INVALID_SOCKET;
        return 0;
    }

    printf("Connecté au serveur %s:%d\n", SERVER_IP, SERVER_PORT);
    return 1;
}

int main(int argc, char **argv) {
    // Initialize database connection
    if (!db_init()) {
        fprintf(stderr, "Failed to initialize database connection.\n");
        return 1;
    }

#ifdef _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
#endif

    if (!init_client_socket()) {
        fprintf(stderr, "Failed to initialize client socket.\n");
        return EXIT_FAILURE;
    }

    GtkApplication *app = gtk_application_new("org.mydiscord.client", G_APPLICATION_FLAGS_NONE);
    if (app == NULL) {
        fprintf(stderr, "Failed to create GtkApplication.\n");
        db_close();
        return 1;
    }
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    g_signal_connect(app, "shutdown", G_CALLBACK(on_app_shutdown), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}