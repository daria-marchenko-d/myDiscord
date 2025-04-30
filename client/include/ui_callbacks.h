#ifndef UI_CALLBACKS_H
#define UI_CALLBACKS_H

#include <gtk/gtk.h>
#include <sys/socket.h>

// Structure to hold data needed in callbacks
typedef struct {
    int socket;
    GtkWidget *login_window;
    GtkWidget *main_window;
} login_data_t;

// Callback function for login button
void on_login_button_clicked(GtkWidget *widget, login_data_t *data);

// Callback function for register button
void on_register_button_clicked(GtkWidget *widget, login_data_t *data);

// Fonction de connexion au serveur (prend IP et port)
int connect_to_server(const char *ip, int port);

// Fonction pour envoyer un message texte brut au serveur
void send_message_to_server(const char *message);

// Thread de réception des messages
void *receive_messages(void *arg);

// Nettoyage du réseau (fermer socket, libérer ressources)
void cleanup_network();

#endif