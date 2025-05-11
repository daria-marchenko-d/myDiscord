#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>

// Registration structure
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

// Function declarations
GtkWidget *create_main_window(GtkApplication *app);
GtkWidget *create_login_window(GtkApplication *app);
GtkWidget *create_register_window(GtkApplication *app);
GtkWidget *create_welcome_window(GtkApplication *app);

void show_login_window(GtkApplication *app, gpointer user_data);
void show_register_window(GtkApplication *app, gpointer user_data);
void on_login_button_clicked(GtkWidget *widget, gpointer user_data);
void on_register_button_clicked(GtkWidget *widget, gpointer user_data);
void on_app_activate(GtkApplication *app, gpointer user_data);

// === Main chat window and messaging ===
void update_channel_list(const char **channels, int num_channels);
void append_message_to_view(const char *username, const char *message);
void request_channel_history(const char *channel);
void show_channel_users(const char *users_list);
void request_channel_users(const char *channel_name);
void send_to_server(const char *message);

// === Channel and message callbacks ===
void on_send_button_clicked(GtkWidget *widget, gpointer data);
void on_channel_selected(GtkListBox *box, GtkListBoxRow *row, gpointer user_data);

// === Global widgets for UI manipulation ===
extern GtkWidget *entry_message;
extern GtkWidget *text_view_chat;
extern GtkWidget *list_box_channels;

#endif