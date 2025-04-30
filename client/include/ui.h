#ifndef UI_H
#define UI_H

#include <gtk/gtk.h> // Inclut l'en-tête principal de GTK+

// Déclaration des structures
typedef struct
{
    GtkApplication *app;
    GtkWidget *entry_username;
    GtkWidget *entry_email;
    GtkWidget *entry_password;
} RegisterData;

typedef struct
{
    GtkApplication *app;
    GtkWidget *entry_username;
    GtkWidget *entry_password;
} LoginData;

// Déclaration des fonctions
GtkWidget *create_main_window(GtkApplication *app);
GtkWidget *create_login_window(GtkApplication *app);
GtkWidget *create_register_window(GtkApplication *app);

void show_login_window(GtkApplication *app, gpointer user_data);
void show_register_window(GtkApplication *app, gpointer user_data);
void on_login_button_clicked(GtkWidget *widget, gpointer user_data);
void on_register_button_clicked(GtkWidget *widget, gpointer user_data);
void on_app_activate(GtkApplication *app, gpointer user_data);

#endif