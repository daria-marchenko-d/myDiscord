#include <gtk/gtk.h>
#include "ui_callback.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
static WSADATA wsa;
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#define closesocket close
#endif
#include <gtk/gtkwindow.h>

int sockfd = -1;
static GThread *recv_thread = NULL;
static char current_channel[256] = ""; // Stocke le canal actuel

int connect_to_server(const char *ip, int port)
{
#ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        fprintf(stderr, "WSAStartup failed.\n");
        show_info_dialog(GTK_WINDOW(window), "Erreur lors de l'initialisation du réseau sous Windows. Réessayez plus tard.");
        return -1;
    }
#endif

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Erreur lors de la création de la socket");
        show_info_dialog(GTK_WINDOW(window), "Impossible de créer la connexion au serveur. Veuillez vérifier l'adresse et le port.");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Erreur de connexion");
        show_info_dialog(GTK_WINDOW(window), "Impossible de se connecter au serveur. Vérifiez l'adresse IP et le port.");
        return -1;
    }

    recv_thread = g_thread_new("receiver_thread", (GThreadFunc)receive_messages, NULL);
    return 0;
}

// === Fonction pour envoyer un message au serveur ===
void send_message_to_server(const char *message)
{
    if (sockfd < 0 || message == NULL)
        return;

    send(sockfd, message, strlen(message), 0);

    if (sockfd != INVALID_SOCKET)
    {
        send(sockfd, message, strlen(message), 0);
    }
}

// === Thread de réception des messages ===
// === Thread de réception des messages ===
// void *receive_messages(void *arg)
// {
//     GtkWidget *parent_window = GTK_WIDGET(arg);

//     char buffer[1024];
//     while (1)
//     {
//         memset(buffer, 0, sizeof(buffer));
//         int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
//         if (bytes_received <= 0)
//         {
//             g_print("Déconnecté du serveur.\n");

//             GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT,
//                                                        GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
//                                                        "Déconnexion du serveur. Veuillez vérifier votre connexion.");
//             gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
//             gtk_widget_unrealize(dialog);

//             break;
//         }

//         char *sep = strchr(buffer, '|');
//         if (sep)
//         {
//             *sep = '\0';
//             const char *channel = buffer;
//             const char *message = sep + 1;

//             if (strcmp(channel, current_channel) == 0)
//             {
//                 if (strncmp(message, "USERS", 5) == 0)
//                 {
//                     const char *users_list = message + 6;
//                     g_idle_add((GSourceFunc)show_channel_users, (gpointer)users_list);
//                 }
//                 else
//                 {
//                     char msg_copy[1024];
//                     strncpy(msg_copy, message, sizeof(msg_copy));
//                     msg_copy[sizeof(msg_copy) - 1] = '\0';

//                     char *username = strtok(msg_copy, ":");
//                     char *msg = strtok(NULL, "");

//                     g_idle_add((GSourceFunc)append_message_to_view, g_strdup_printf("%s: %s", username, msg));
//                 }
//             }
//         }
//     }

//     return NULL;
// }

void *receive_messages(void *arg)
{
    GtkWidget *parent_window = GTK_WIDGET(arg);

    char buffer[1024];
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0)
        {
            g_print("Déconnecté du serveur.\n");
            g_idle_add((GSourceFunc)show_info_dialog_idle, g_strdup("Déconnexion du serveur. Veuillez vérifier votre connexion."));
            break;
        }

        char *sep = strchr(buffer, '|');
        if (sep)
        {
            *sep = '\0';
            const char *channel = buffer;
            const char *message = sep + 1;

            if (strcmp(channel, current_channel) == 0)
            {
                if (strncmp(message, "USERS", 5) == 0)
                {
                    const char *users_list = message + 6;
                    g_idle_add((GSourceFunc)show_channel_users, (gpointer)users_list);
                }
                else
                {
                    char msg_copy[1024];
                    strncpy(msg_copy, message, sizeof(msg_copy));
                    msg_copy[sizeof(msg_copy) - 1] = '\0';

                    char *username = strtok(msg_copy, ":");
                    char *msg = strtok(NULL, "");

                    g_idle_add((GSourceFunc)append_message_to_view, g_strdup_printf("%s: %s", username, msg));
                }
            }
        }
    }

    return NULL;
}

// === Fonction pour rejoindre un canal ===
void join_channel(const char *channel_name)
{
    snprintf(current_channel, sizeof(current_channel), "%s", channel_name); // Met à jour le canal actuel
    char message[256];
    snprintf(message, sizeof(message), "JOIN %s", channel_name);
    send_message_to_server(message);
}

// === Fonction pour quitter un canal ===
void leave_channel()
{
    char message[256];
    snprintf(message, sizeof(message), "LEAVE %s", current_channel);
    send_message_to_server(message);
    memset(current_channel, 0, sizeof(current_channel)); // Réinitialise le canal actuel
}

// === Fonction pour créer un nouveau canal ===
void create_channel(const char *channel_name)
{
    snprintf(current_channel, sizeof(current_channel), "%s", channel_name); // Définit le nouveau canal
    char message[256];
    snprintf(message, sizeof(message), "CREATE %s", channel_name);
    send_message_to_server(message);
}

// === Demander l'historique des messages d'un canal ===
void request_channel_history(const char *channel)
{
    char message[256];
    snprintf(message, sizeof(message), "HISTORY %s", current_channel);
    send_message_to_server(message);
}

// === Demander la liste des utilisateurs d'un canal ===
void request_channel_users(const char *channel_name)
{
    char message[256];
    snprintf(message, sizeof(message), "USERS %s", channel_name);
    send_message_to_server(message);
}

// === Fonction pour envoyer un message dans le canal actuel ===
void send_message_in_channel(const char *message)
{
    char full_message[512];
    snprintf(full_message, sizeof(full_message), "MESSAGE %s: %s", current_channel, message);
    send_message_to_server(full_message);
}

// === Nettoyage ===
void cleanup_network()
{
    if (recv_thread)
    {
        g_thread_join(recv_thread);
        recv_thread = NULL;
    }

    if (sockfd >= 0)
    {
        // Sous Windows, on utilise closesocket
        closesocket(sockfd);
        sockfd = -1;
    }

#ifdef _WIN32
    WSACleanup(); // Cleanup de Winsock
#endif
}