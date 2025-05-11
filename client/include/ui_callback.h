#ifndef NETWORK_H
#define NETWORK_H

// Pour Gtk et types utilisés
#include <gtk/gtk.h>

// Fonction de connexion au serveur (prend IP et port)
int connect_to_server(const char *ip, int port);

// Fonction pour envoyer un message texte brut au serveur
void send_message_to_server(const char *message);

// Thread de réception des messages
void *receive_messages(void *arg);

void show_channel_users(const char *users_list);

// Pour ajouter un message dans la TextView depuis un thread secondaire
gboolean append_message_to_view_idle(gpointer data);

// Nettoyage du réseau (fermer socket, libérer ressources)
void cleanup_network();

#endif
