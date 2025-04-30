// ui_callbacks.c (Fichier fusionné)
#include "ui_callbacks.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
    static WSADATA wsa;
#else
    #include <arpa/inet.h>
    #include <pthread.h>
    #define closesocket close
#endif
#include <gtk/gtkwindow.h>

int sockfd = -1;
static GThread *recv_thread = NULL;
static char current_channel[256] = ""; // Stocke le canal actuel

// === Fonction pour initialiser la connexion au serveur ===
int connect_to_server(const char *ip, int port) {
#ifdef _WIN32
    // Initialisation de Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");

        // Affichage d'un message d'erreur spécifique à Windows
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                                   "Erreur lors de l'initialisation du réseau sous Windows. Réessayez plus tard.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return -1;
    }
#endif

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erreur lors de la création de la socket");

        // Affichage d'un message d'erreur
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                                   "Impossible de créer la connexion au serveur. Veuillez vérifier l'adresse et le port.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur de connexion");

        // Affichage d'un message d'erreur spécifique
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                                   "Impossible de se connecter au serveur. Vérifiez l'adresse IP et le port.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return -1;
    }

    // Lancement du thread pour recevoir les messages
    recv_thread = g_thread_new("receiver_thread", (GThreadFunc)receive_messages, NULL);
    return 0;
}

// === Fonction pour envoyer un message au serveur ===
void send_message_to_server(const char *msg) {
    if (sockfd < 0 || msg == NULL) return;

    send(sockfd, msg, strlen(msg), 0);
}

// === Thread de réception des messages ===
void *receive_messages(void *arg) {
    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            g_print("Déconnecté du serveur.\n");

            // Afficher un message d'erreur de connexion
            GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT,
                                                     GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                                     "Déconnexion du serveur. Veuillez vérifier votre connexion.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            break;
        }

        // Traitement du message reçu
        char *sep = strchr(buffer, '|');
        if (sep) {
            *sep = '\0';
            const char *channel = buffer;
            const char *message = sep + 1;

            // Vérifie si le message appartient au canal actuel
            if (strcmp(channel, current_channel) == 0) {
                if (strncmp(message, "USERS", 5) == 0) {
                    // Traitement de la liste des utilisateurs
                    const char *users_list = message + 6; // Ignorer le "USERS|"
                    g_idle_add((GSourceFunc)show_channel_users, users_list);
                } else {
                    char *username = strtok(message, ":");
                    char *msg = strtok(NULL, "");

                    // Ajoute le message à l'interface utilisateur
                    g_idle_add((GSourceFunc)append_message_to_view, g_strdup_printf("%s: %s", username, msg));
                }
            }
        }
    }

    return NULL;
}

// === Fonction pour rejoindre un canal ===
void join_channel(const char *channel_name) {
    snprintf(current_channel, sizeof(current_channel), "%s", channel_name); // Met à jour le canal actuel
    char message[256];
    snprintf(message, sizeof(message), "JOIN %s", channel_name);
    send_message_to_server(message);
}

// === Fonction pour quitter un canal ===
void leave_channel() {
    char message[256];
    snprintf(message, sizeof(message), "LEAVE %s", current_channel);
    send_message_to_server(message);
    memset(current_channel, 0, sizeof(current_channel)); // Réinitialise le canal actuel
}

// === Fonction pour créer un nouveau canal ===
void create_channel(const char *channel_name) {
    snprintf(current_channel, sizeof(current_channel), "%s", channel_name); // Définit le nouveau canal
    char message[256];
    snprintf(message, sizeof(message), "CREATE %s", channel_name);
    send_message_to_server(message);
}

// === Demander l'historique des messages d'un canal ===
void request_channel_history() {
    char message[256];
    snprintf(message, sizeof(message), "HISTORY %s", current_channel);
    send_message_to_server(message);
}

// === Demander la liste des utilisateurs d'un canal ===
void request_channel_users(const char *channel_name) {
    char message[256];
    snprintf(message, sizeof(message), "USERS %s", channel_name);
    send_message_to_server(message);
}

// === Fonction pour envoyer un message dans le canal actuel ===
void send_message_in_channel(const char *message) {
    char full_message[512];
    snprintf(full_message, sizeof(full_message), "MESSAGE %s: %s", current_channel, message);
    send_message_to_server(full_message);
}

// === Nettoyage ===
void cleanup_network() {
    if (recv_thread) {
        g_thread_join(recv_thread);
        recv_thread = NULL;
    }

    if (sockfd >= 0) {
        // Sous Windows, on utilise closesocket
        closesocket(sockfd);
        sockfd = -1;
    }

#ifdef _WIN32
    WSACleanup(); // Cleanup de Winsock
#endif
}

// Callback function for login button
void on_login_button_clicked(GtkWidget *widget, login_data_t *data) {
    // Get username and password from the entry fields
    GtkBuilder *builder = gtk_builder_get_object(gtk_widget_get_buildable(widget), "builder");
    GtkWidget *username_entry = GTK_WIDGET(gtk_builder_get_object(builder, "entry_username"));
    GtkWidget *password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "entry_password"));

    const char *username = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(password_entry));

    // Format the login message
    char message[256];
    snprintf(message, sizeof(message), "LOGIN %s %s", username, password);

    // Send the message to the server
    send_message_to_server(message);

    // Receive the response from the server
    char buffer[256];
    ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Received: %s\n", buffer);

        // Check the response and show the main window if login is successful
        if (strcmp(buffer, "LOGIN_SUCCESS") == 0) {
            gtk_widget_hide(data->login_window);
            gtk_widget_show(data->main_window);
        } else {
            // Display an error message
            GtkWidget *dialog;
            dialog = gtk_message_dialog_new(GTK_WINDOW(data->login_window),
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_OK,
                                             "Login failed. Invalid username or password.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }
    } else {
        perror("Receive failed");
    }
}

// Callback function for register button
void on_register_button_clicked(GtkWidget *widget, login_data_t *data) {
    // Get the values from the registration fields
    GtkBuilder *builder = gtk_builder_get_object(gtk_widget_get_buildable(widget), "builder");
    GtkWidget *register_username_entry = GTK_WIDGET(gtk_builder_get_object(builder, "entry_username"));
    GtkWidget *register_password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "entry_password"));
    GtkWidget *register_email_entry = GTK_WIDGET(gtk_builder_get_object(builder, "entry_email"));

    const char *username = gtk_entry_get_text(GTK_ENTRY(register_username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(register_password_entry));
    const char *email = gtk_entry_get_text(GTK_ENTRY(register_email_entry));

    // Format the register message
    char message[256];
    snprintf(message, sizeof(message), "REGISTER %s %s %s", username, password, email);

    // Send the message to the server
    send_message_to_server(message);

    // Receive the response from the server
    char buffer[256];
    ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Received: %s\n", buffer);

        // Check the response and display a message
        GtkWidget *dialog;
        if (strcmp(buffer, "REGISTER_SUCCESS") == 0) {
            dialog = gtk_message_dialog_new(GTK_WINDOW(data->login_window),
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_INFO,
                                             GTK_BUTTONS_OK,
                                             "Registration successful!");
        } else {
            dialog = gtk_message_dialog_new(GTK_WINDOW(data->login_window),
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_OK,
                                             "Registration failed.");
        }
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        perror("Receive failed");
    }
}