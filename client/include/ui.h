#ifndef UI_H

#define UI_H

#include <gtk/gtk.h>

// Functions to create and show the login window

GtkWidget *create_login_window();

void show_login_window(GtkApplication *app, gpointer user_data);

void on_app_activate(GtkApplication *app, gpointer user_data);

// Functions to create and show the registration window

GtkWidget *create_register_window();

void show_register_window(GtkApplication *app, gpointer user_data);

// Callback functions for button clicks

void on_login_button_clicked(GtkWidget *widget, gpointer data);

void on_register_button_clicked(GtkWidget *widget, gpointer data);

#endif
