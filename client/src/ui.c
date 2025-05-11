#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkevents.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>
#include "db.h"
#include "ui.h"
#include <gtk/gtkx.h>
#include "ui_callback.h"

GtkWidget *window;
GtkWidget *list_box_channels;
GtkWidget *text_view_chat;
GtkWidget *entry_message;
GtkWidget *label_users;
GtkWidget *messages_box;
char current_channel[64] = "";

// Functions
void show_login_window(GtkApplication *app, gpointer user_data);
void show_register_window(GtkApplication *app, gpointer user_data);
void on_login_button_clicked(GtkWidget *widget, gpointer user_data);
void on_register_button_clicked(GtkWidget *widget, gpointer user_data);
void send_reaction_request(GtkWidget *widget, gpointer user_data);

// Validation functions
gboolean validate_username(const char *username)
{
    regex_t regex;
    int ret;

    ret = regcomp(&regex, "^[A-Za-z'-]+$", REG_EXTENDED);
    if (ret)
        return FALSE;

    ret = regexec(&regex, username, 0, NULL, 0);
    regfree(&regex);

    return ret == 0;
}

gboolean validate_email(const char *email)
{
    regex_t regex;
    int ret;

    ret = regcomp(&regex, "^[a-z0-9._%+-]+@[a-z0-9.-]+\\.[a-z]{2,}$", REG_EXTENDED | REG_ICASE);
    if (ret)
        return FALSE;

    ret = regexec(&regex, email, 0, NULL, 0);
    regfree(&regex);

    return ret == 0;
}

gboolean validate_password(const char *password)
{
    if (strlen(password) < 6 || strlen(password) > 20)
        return FALSE;

    gboolean has_upper = FALSE, has_lower = FALSE, has_digit = FALSE, has_special = FALSE;

    for (const char *p = password; *p; p++)
    {
        if (g_ascii_isupper(*p))
            has_upper = TRUE;
        else if (g_ascii_islower(*p))
            has_lower = TRUE;
        else if (g_ascii_isdigit(*p))
            has_digit = TRUE;
        else
            has_special = TRUE;
    }

    return has_upper && has_lower && has_digit && has_special;
}

// Create the main window
GtkWidget *create_welcome_window(GtkApplication *app)
{
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Welcome to MyDiscord");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), box);

    GtkWidget *login_button = gtk_button_new_with_label("Log In");
    g_signal_connect_swapped(login_button, "clicked", G_CALLBACK(show_login_window), app);
    gtk_box_append(GTK_BOX(box), login_button);

    GtkWidget *register_button = gtk_button_new_with_label("Register");
    g_signal_connect_swapped(register_button, "clicked", G_CALLBACK(show_register_window), app);
    gtk_box_append(GTK_BOX(box), register_button);

    return window;
}

// Fonction pour créer la fenêtre principale
GtkWidget *create_main_window(GtkApplication *app)
{
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Mydiscord");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(window), main_box);

    // Liste des canaux
    list_box_channels = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(list_box_channels), GTK_SELECTION_SINGLE);
    g_signal_connect(list_box_channels, "row-selected", G_CALLBACK(on_channel_selected), NULL);
    gtk_box_append(GTK_BOX(main_box), list_box_channels);

    // Affichage des messages
    text_view_chat = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view_chat), FALSE);
    gtk_box_append(GTK_BOX(main_box), text_view_chat);

    // Zone de saisie de message
    entry_message = gtk_entry_new();
    gtk_box_append(GTK_BOX(main_box), entry_message);
    g_signal_connect(entry_message, "activate", G_CALLBACK(on_send_button_clicked), NULL);

    // Label pour la liste des utilisateurs du canal
    label_users = gtk_label_new("Utilisateurs:");
    gtk_box_append(GTK_BOX(main_box), label_users);

    gtk_widget_show(window);
}

// Helper function: handle login success dialog response
static void on_login_success_response(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
    GtkWindow *login_window = GTK_WINDOW(user_data);
    GtkApplication *app = GTK_APPLICATION(g_object_get_data(source_object, "app"));

    GtkWidget *main_win = create_main_window(app);
    gtk_window_present(GTK_WINDOW(main_win));
    gtk_window_destroy(login_window);
}

// Handle login button click
void on_login_button_clicked(GtkWidget *widget, gpointer user_data)
{
    LoginData *data = (LoginData *)user_data;

    const char *username = gtk_editable_get_text(GTK_EDITABLE(data->entry_username));
    const char *password = gtk_editable_get_text(GTK_EDITABLE(data->entry_password));

    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_root(widget));

    if (!validate_email(username) || !validate_password(password))
    {
        GtkAlertDialog *alert = gtk_alert_dialog_new("Invalid username or password format.");
        gtk_alert_dialog_show(alert, parent_window);
        g_object_unref(alert);
        return;
    }

    int user_id;
    char role[20];

    if (authenticate_user(username, password))
    {
        GtkAlertDialog *alert = gtk_alert_dialog_new("Successfully logged in!");
        g_object_set_data(G_OBJECT(alert), "app", data->app);
        gtk_alert_dialog_choose(alert, parent_window, NULL, on_login_success_response, parent_window);
        g_object_unref(alert);
    }
    else
    {
        GtkAlertDialog *alert = gtk_alert_dialog_new("Login failed. Please check your username and password.");
        gtk_alert_dialog_show(alert, parent_window);
        g_object_unref(alert);
    }
}

// Helper function: handle registration success dialog response
static void on_register_success_response(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
    GtkWindow *register_window = GTK_WINDOW(user_data);
    gtk_window_destroy(register_window);
}

// Handle register button click
void on_register_button_clicked(GtkWidget *widget, gpointer user_data)
{
    RegisterData *reg_data = (RegisterData *)user_data;

    const char *username = gtk_editable_get_text(GTK_EDITABLE(reg_data->entry_username));
    const char *email = gtk_editable_get_text(GTK_EDITABLE(reg_data->entry_email));
    const char *password = gtk_editable_get_text(GTK_EDITABLE(reg_data->entry_password));

    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_root(widget));

    if (!validate_username(username))
    {
        GtkAlertDialog *alert = gtk_alert_dialog_new("Invalid username. Only Latin letters, '-' and ''' allowed.");
        gtk_alert_dialog_show(alert, parent_window);
        g_object_unref(alert);
        return;
    }

    if (!validate_email(email))
    {
        GtkAlertDialog *alert = gtk_alert_dialog_new("Invalid email format. Must contain '@'.");
        gtk_alert_dialog_show(alert, parent_window);
        g_object_unref(alert);
        return;
    }

    if (!validate_password(password))
    {
        GtkAlertDialog *alert = gtk_alert_dialog_new("Invalid password. 6–20 chars, 1 upper, 1 lower, 1 number, 1 symbol required.");
        gtk_alert_dialog_show(alert, parent_window);
        g_object_unref(alert);
        return;
    }

    if (add_user(username, email, password))
    {
        GtkAlertDialog *alert = gtk_alert_dialog_new("User successfully registered!");

        g_signal_connect(alert, "response", G_CALLBACK(on_register_success_response), parent_window);
        gtk_alert_dialog_show(alert, parent_window);

        // gtk_alert_dialog_choose(alert, parent_window, NULL, on_register_success_response, parent_window);
        g_object_unref(alert);
    }
    else
    {
        GtkAlertDialog *alert = gtk_alert_dialog_new("Error. Username or email already exists.");
        gtk_alert_dialog_show(alert, parent_window);
        g_object_unref(alert);
    }
}

// Nelson's code

// Fonction pour mettre à jour la liste des canaux
void update_channel_list(const char **channels, int num_channels)
{
    gtk_list_box_remove_all(GTK_LIST_BOX(list_box_channels));

    for (int i = 0; i < num_channels; ++i)
    {
        GtkWidget *label = gtk_label_new(channels[i]);
        gtk_list_box_insert(GTK_LIST_BOX(list_box_channels), label, -1);
    }

    gtk_widget_show(list_box_channels);
}

// Fonction appelée lorsqu'un canal est sélectionné
void on_channel_selected(GtkListBox *box, GtkListBoxRow *row, gpointer data)
{
    if (!row)
        return;

    GtkWidget *child = gtk_list_box_row_get_child(row);
    const char *channel = gtk_label_get_text(GTK_LABEL(child));

    strncpy(current_channel, channel, sizeof(current_channel));
    current_channel[sizeof(current_channel) - 1] = '\0';

    request_channel_history(current_channel); // Demander l'historique des messages
    request_channel_users(current_channel);   // Demander la liste des utilisateurs
}

// Fonction pour envoyer un message au serveur
void on_send_button_clicked(GtkWidget *widget, gpointer data)
{
    const char *message = gtk_entry_get_text(GTK_ENTRY(entry_message));
    if (strlen(message) == 0 || strlen(current_channel) == 0)
        return;

    char buffer[512];
    snprintf(buffer, sizeof(buffer), "MSG|%s|%s", current_channel, message);
    send_message_to_server(buffer);

    gtk_entry_set_text(GTK_ENTRY(entry_message), "");
}

// Fonction pour ajouter un message à la vue de chat
// void append_message_to_view(const char *message)
// {
//     GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_chat));

//     GtkTextIter end;
//     gtk_text_buffer_get_end_iter(buffer, &end);

//     // Format du message
//     char formatted[512];
//     snprintf(formatted, sizeof(formatted), "%s\n", message);

//     // Insertion du message dans la vue
//     gtk_text_buffer_insert(buffer, &end, formatted, -1);
//     gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(text_view_chat), &end, 0.0, FALSE, 0, 0);
// }

void append_message_to_view(const char *username, const char *message)
{
    GtkWidget *message_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_halign(message_container, GTK_ALIGN_START);

    GtkWidget *message_label = gtk_label_new(NULL);
    char *formatted_text = g_strdup_printf("<b>%s:</b> %s", username, message);
    gtk_label_set_markup(GTK_LABEL(message_label), formatted_text);
    g_free(formatted_text);

    // TODO: Add right click menu for reactions
    // g_signal_connect(message_label, "button-press-event", G_CALLBACK(on_right_click), NULL);

    gtk_widget_set_halign(message_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(message_container), message_label);

    // Create a box for reactions
    GtkWidget *reactions_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_name(reactions_box, "reactions_box");
    gtk_widget_set_halign(reactions_box, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(message_container), reactions_box);

    // Add a reaction label (this can be updated later with actual reactions)
    gtk_box_append(GTK_BOX(messages_box), message_container);
}

// Fonction pour afficher la liste des utilisateurs dans le canal actif
void show_channel_users(const char *users_list)
{
    // Mise à jour du label pour afficher la liste des utilisateurs
    char formatted[1024];
    snprintf(formatted, sizeof(formatted), "Utilisateurs:\n%s", users_list);
    gtk_label_set_text(GTK_LABEL(label_users), formatted);
}

// Create the login window
GtkWidget *create_login_window(GtkApplication *app)
{
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Log In");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), box);

    GtkWidget *entry_username = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), "Email");
    gtk_box_append(GTK_BOX(box), entry_username);

    GtkWidget *hint_username = gtk_label_new("Use only Latin letters, '-' and ''' allowed.");
    gtk_widget_set_halign(hint_username, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(box), hint_username);

    GtkWidget *entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_box_append(GTK_BOX(box), entry_password);

    GtkWidget *hint_password = gtk_label_new("6–20 chars, 1 uppercase, 1 lowercase, 1 digit, 1 symbol.");
    gtk_widget_set_halign(hint_password, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(box), hint_password);

    GtkWidget *login_button = gtk_button_new_with_label("Log In");

    LoginData *data = g_new(LoginData, 1);
    data->app = app;
    data->entry_username = entry_username;
    data->entry_password = entry_password;

    g_signal_connect_data(login_button, "clicked", G_CALLBACK(on_login_button_clicked), data, (GClosureNotify)g_free, 0);
    gtk_box_append(GTK_BOX(box), login_button);

    GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");
    g_signal_connect_swapped(cancel_button, "clicked", G_CALLBACK(gtk_window_destroy), window);
    gtk_box_append(GTK_BOX(box), cancel_button);

    return window;
}

// Create the register window
GtkWidget *create_register_window(GtkApplication *app)
{
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Register");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), box);

    GtkWidget *entry_username = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), "Username");
    gtk_box_append(GTK_BOX(box), entry_username);

    GtkWidget *hint_username = gtk_label_new("Use only Latin letters, '-' and ''' allowed.");
    gtk_widget_set_halign(hint_username, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(box), hint_username);

    GtkWidget *entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Email");
    gtk_box_append(GTK_BOX(box), entry_email);

    GtkWidget *hint_email = gtk_label_new("Must include '@'.");
    gtk_widget_set_halign(hint_email, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(box), hint_email);

    GtkWidget *entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_box_append(GTK_BOX(box), entry_password);

    GtkWidget *hint_password = gtk_label_new("6–20 chars, 1 uppercase, 1 lowercase, 1 digit, 1 symbol.");
    gtk_widget_set_halign(hint_password, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(box), hint_password);

    RegisterData *reg_data = g_new(RegisterData, 1);
    reg_data->app = app;
    reg_data->entry_username = entry_username;
    reg_data->entry_email = entry_email;
    reg_data->entry_password = entry_password;

    GtkWidget *register_button = gtk_button_new_with_label("Register");
    g_signal_connect_data(register_button, "clicked", G_CALLBACK(on_register_button_clicked), reg_data, (GClosureNotify)g_free, 0);
    gtk_box_append(GTK_BOX(box), register_button);

    GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");
    g_signal_connect_swapped(cancel_button, "clicked", G_CALLBACK(gtk_window_destroy), window);
    gtk_box_append(GTK_BOX(box), cancel_button);

    return window;
}

// Show the login window
void show_login_window(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window = create_login_window(app);
    gtk_window_present(GTK_WINDOW(window));
}

// Show the register window
void show_register_window(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window = create_register_window(app);
    gtk_window_present(GTK_WINDOW(window));
}

// Application activation
void on_app_activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window = create_welcome_window(app);
    gtk_window_present(GTK_WINDOW(window));
}

// gboolean on_message_right_click(GtkWidget *widget, GdkEvent *event, gpointer user_data)
// {
//     if (event->type == GDK_BUTTON_PRESS && event->button == 3) // Right-click
//     {
//         GtkWidget *emoji_menu = create_emoji_selector("1"); // TODO: замінити "1" на реальний message_id
//         gtk_menu_popup_at_pointer(GTK_MENU(emoji_menu), (GdkEvent *)event);
//         return TRUE;
//     }
//     return FALSE;
// }

// gboolean on_message_right_click(GtkWidget *widget, GdkEvent *event, gpointer user_data)
// {
//     if (event->type == GDK_BUTTON_PRESS)
//     {
//         GdkEventButton *button_event = (GdkEventButton *)event;
//         if (button_event->button == GDK_BUTTON_SECONDARY) // Right click
//         {
//             GtkWidget *menu = create_emoji_selector("1"); // Replace with actual message ID
//             gtk_menu_popup_at_pointer(GTK_MENU(menu), (GdkEvent *)event);
//             return TRUE;
//         }
//     }
//     return FALSE;
// }

GtkWidget *create_emoji_selector(const char *message_id)
{
    GtkWidget *menu = gtk_menu_new();

    const char *emojis[] = {"👍", "❤️", "😂", "😮", "😢", "😡"};
    for (int i = 0; i < sizeof(emojis) / sizeof(emojis[0]); i++)
    {
        GtkWidget *item = gtk_menu_item_new_with_label(emojis[i]);
        g_object_set_data(G_OBJECT(item), "message_id", (gpointer)message_id);
        g_object_set_data(G_OBJECT(item), "emoji", (gpointer)emojis[i]);
        g_signal_connect(item, "activate", G_CALLBACK(send_reaction_request), NULL);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    }

    gtk_widget_show_all(menu);
    return menu;
}

// void send_reaction_request(GtkMenuItem *item, gpointer user_data)
// {
//     const char *message_id = g_object_get_data(G_OBJECT(item), "message_id");
//     const char *emoji = g_object_get_data(G_OBJECT(item), "emoji");

//     if (strlen(current_channel) == 0)
//         return;

//     char buffer[256];
//     snprintf(buffer, sizeof(buffer), "REACT|%s|%s|%s", current_channel, message_id, emoji);
//     send_message_to_server(buffer);
// }

void send_reaction_request(GtkWidget *widget, gpointer user_data)
{
    const char *message_id = g_object_get_data(G_OBJECT(widget), "message_id");
    const char *emoji = g_object_get_data(G_OBJECT(widget), "emoji");

    if (!message_id || !emoji)
    {
        g_warning("Missing message_id or emoji");
        return;
    }

    //     g_print("Sending reaction: %s for message ID: %s\n", emoji, message_id);
    // }

    // 🛰️ Example of sending a reaction — modify with your actual socket/API call
    char request[256];
    snprintf(request, sizeof(request), "REACT %s %s\n", message_id, emoji);
    send_to_server(request);
}

void update_message_reactions(const char *message_id, const char *emoji, int count)
{

    GList *children = gtk_widget_get_children(GTK_WIDGET(text_view_chat));
    for (GList *iter = children; iter != NULL; iter = iter->next)
    {
        GtkWidget *container = GTK_WIDGET(iter->data);
        GtkWidget *reactions_box = gtk_widget_get_first_child(container);
        while (reactions_box && strcmp(gtk_widget_get_name(reactions_box), "reactions_box") != 0)
        {
            reactions_box = gtk_widget_get_next_sibling(reactions_box);
        }

        if (reactions_box)
        {
            char buf[64];
            snprintf(buf, sizeof(buf), "%s %d", emoji, count);
            GtkWidget *reaction_label = gtk_label_new(buf);
            gtk_box_append(GTK_BOX(reactions_box), reaction_label);
            gtk_widget_show(reaction_label);
            break;
        }
    }
}

// #include <gtk/gtk.h>
// #include <gdk/gdk.h>
// #include <gdk/gdkevents.h>
// #include <string.h>
// #include <stdio.h>
// #include <regex.h>
// #include "db.h"
// #include "ui.h"
// #include "ui_callback.h"

// GtkWidget *window;
// GtkWidget *list_box_channels;
// GtkWidget *text_view_chat;
// GtkWidget *entry_message;
// GtkWidget *label_users;
// GtkWidget *messages_box;
// char current_channel[64] = "";

// // Functions
// void show_login_window(GtkApplication *app, gpointer user_data);
// void show_register_window(GtkApplication *app, gpointer user_data);
// void on_login_button_clicked(GtkWidget *widget, gpointer user_data);
// void on_register_button_clicked(GtkWidget *widget, gpointer user_data);
// void send_reaction_request(GtkWidget *widget, gpointer user_data);

// // Validation functions
// gboolean validate_username(const char *username)
// {
//     regex_t regex;
//     int ret;

//     ret = regcomp(&regex, "^[A-Za-z'-]+$", REG_EXTENDED);
//     if (ret)
//         return FALSE;

//     ret = regexec(&regex, username, 0, NULL, 0);
//     regfree(&regex);

//     return ret == 0;
// }

// gboolean validate_email(const char *email)
// {
//     regex_t regex;
//     int ret;

//     ret = regcomp(&regex, "^[a-z0-9._%+-]+@[a-z0-9.-]+\\.[a-z]{2,}$", REG_EXTENDED | REG_ICASE);
//     if (ret)
//         return FALSE;

//     ret = regexec(&regex, email, 0, NULL, 0);
//     regfree(&regex);

//     return ret == 0;
// }

// gboolean validate_password(const char *password)
// {
//     if (strlen(password) < 6 || strlen(password) > 20)
//         return FALSE;

//     gboolean has_upper = FALSE, has_lower = FALSE, has_digit = FALSE, has_special = FALSE;

//     for (const char *p = password; *p; p++)
//     {
//         if (g_ascii_isupper(*p))
//             has_upper = TRUE;
//         else if (g_ascii_islower(*p))
//             has_lower = TRUE;
//         else if (g_ascii_isdigit(*p))
//             has_digit = TRUE;
//         else
//             has_special = TRUE;
//     }

//     return has_upper && has_lower && has_digit && has_special;
// }

// // Create the main window
// GtkWidget *create_welcome_window(GtkApplication *app)
// {
//     GtkWidget *window = gtk_application_window_new(app);
//     gtk_window_set_title(GTK_WINDOW(window), "Welcome to MyDiscord");
//     gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

//     GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
//     gtk_window_set_child(GTK_WINDOW(window), box);

//     GtkWidget *login_button = gtk_button_new_with_label("Log In");
//     g_signal_connect_swapped(login_button, "clicked", G_CALLBACK(show_login_window), app);
//     gtk_box_append(GTK_BOX(box), login_button);

//     GtkWidget *register_button = gtk_button_new_with_label("Register");
//     g_signal_connect_swapped(register_button, "clicked", G_CALLBACK(show_register_window), app);
//     gtk_box_append(GTK_BOX(box), register_button);

//     return window;
// }

// // Fonction pour créer la fenêtre principale
// GtkWidget *create_main_window(GtkApplication *app)
// {
//     window = gtk_application_window_new(app);
//     gtk_window_set_title(GTK_WINDOW(window), "Mydiscord");
//     gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

//     GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
//     gtk_window_set_child(GTK_WINDOW(window), main_box);

//     // Liste des canaux
//     list_box_channels = gtk_list_box_new();
//     gtk_list_box_set_selection_mode(GTK_LIST_BOX(list_box_channels), GTK_SELECTION_SINGLE);
//     g_signal_connect(list_box_channels, "row-selected", G_CALLBACK(on_channel_selected), NULL);
//     gtk_box_append(GTK_BOX(main_box), list_box_channels);

//     // Affichage des messages
//     text_view_chat = gtk_text_view_new();
//     gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view_chat), FALSE);
//     gtk_box_append(GTK_BOX(main_box), text_view_chat);

//     // Zone de saisie de message
//     entry_message = gtk_entry_new();
//     gtk_box_append(GTK_BOX(main_box), entry_message);
//     g_signal_connect(entry_message, "activate", G_CALLBACK(on_send_button_clicked), NULL);

//     // Label pour la liste des utilisateurs du canal
//     label_users = gtk_label_new("Utilisateurs:");
//     gtk_box_append(GTK_BOX(main_box), label_users);

//     gtk_widget_show(window);
// }

// // Helper function: handle login success dialog response
// static void on_login_success_response(GObject *source_object, GAsyncResult *res, gpointer user_data)
// {
//     GtkWindow *login_window = GTK_WINDOW(user_data);
//     GtkApplication *app = GTK_APPLICATION(g_object_get_data(source_object, "app"));

//     GtkWidget *main_win = create_main_window(app);
//     gtk_window_present(GTK_WINDOW(main_win));
//     gtk_window_destroy(login_window);
// }

// // Handle login button click
// void on_login_button_clicked(GtkWidget *widget, gpointer user_data)
// {
//     LoginData *data = (LoginData *)user_data;

//     const char *username = gtk_editable_get_text(GTK_EDITABLE(data->entry_username));
//     const char *password = gtk_editable_get_text(GTK_EDITABLE(data->entry_password));

//     GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_root(widget));

//     if (!validate_email(username) || !validate_password(password))
//     {
//         GtkAlertDialog *alert = gtk_alert_dialog_new("Invalid username or password format.");
//         gtk_alert_dialog_show(alert, parent_window);
//         g_object_unref(alert);
//         return;
//     }

//     int user_id;
//     char role[20];

//     if (authenticate_user(username, password))
//     {
//         GtkAlertDialog *alert = gtk_alert_dialog_new("Successfully logged in!");
//         g_object_set_data(G_OBJECT(alert), "app", data->app);
//         gtk_alert_dialog_choose(alert, parent_window, NULL, on_login_success_response, parent_window);
//         g_object_unref(alert);
//     }
//     else
//     {
//         GtkAlertDialog *alert = gtk_alert_dialog_new("Login failed. Please check your username and password.");
//         gtk_alert_dialog_show(alert, parent_window);
//         g_object_unref(alert);
//     }
// }

// // Helper function: handle registration success dialog response
// static void on_register_success_response(GObject *source_object, GAsyncResult *res, gpointer user_data)
// {
//     GtkWindow *register_window = GTK_WINDOW(user_data);
//     gtk_window_destroy(register_window);
// }

// // Handle register button click
// void on_register_button_clicked(GtkWidget *widget, gpointer user_data)
// {
//     RegisterData *reg_data = (RegisterData *)user_data;

//     const char *username = gtk_editable_get_text(GTK_EDITABLE(reg_data->entry_username));
//     const char *email = gtk_editable_get_text(GTK_EDITABLE(reg_data->entry_email));
//     const char *password = gtk_editable_get_text(GTK_EDITABLE(reg_data->entry_password));

//     GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_root(widget));

//     if (!validate_username(username))
//     {
//         GtkAlertDialog *alert = gtk_alert_dialog_new("Invalid username. Only Latin letters, '-' and ''' allowed.");
//         gtk_alert_dialog_show(alert, parent_window);
//         g_object_unref(alert);
//         return;
//     }

//     if (!validate_email(email))
//     {
//         GtkAlertDialog *alert = gtk_alert_dialog_new("Invalid email format. Must contain '@'.");
//         gtk_alert_dialog_show(alert, parent_window);
//         g_object_unref(alert);
//         return;
//     }

//     if (!validate_password(password))
//     {
//         GtkAlertDialog *alert = gtk_alert_dialog_new("Invalid password. 6–20 chars, 1 upper, 1 lower, 1 number, 1 symbol required.");
//         gtk_alert_dialog_show(alert, parent_window);
//         g_object_unref(alert);
//         return;
//     }

//     if (add_user(username, email, password))
//     {
//         GtkAlertDialog *alert = gtk_alert_dialog_new("User successfully registered!");

//         g_signal_connect(alert, "response", G_CALLBACK(on_register_success_response), parent_window);
//         gtk_alert_dialog_show(alert, parent_window);

//         // gtk_alert_dialog_choose(alert, parent_window, NULL, on_register_success_response, parent_window);
//         g_object_unref(alert);
//     }
//     else
//     {
//         GtkAlertDialog *alert = gtk_alert_dialog_new("Error. Username or email already exists.");
//         gtk_alert_dialog_show(alert, parent_window);
//         g_object_unref(alert);
//     }
// }

// // Nelson's code

// // Fonction pour mettre à jour la liste des canaux
// void update_channel_list(const char **channels, int num_channels)
// {
//     gtk_list_box_remove_all(GTK_LIST_BOX(list_box_channels));

//     for (int i = 0; i < num_channels; ++i)
//     {
//         GtkWidget *label = gtk_label_new(channels[i]);
//         gtk_list_box_insert(GTK_LIST_BOX(list_box_channels), label, -1);
//     }

//     gtk_widget_show(list_box_channels);
// }

// // Fonction appelée lorsqu'un canal est sélectionné
// void on_channel_selected(GtkListBox *box, GtkListBoxRow *row, gpointer data)
// {
//     if (!row)
//         return;

//     GtkWidget *child = gtk_list_box_row_get_child(row);
//     const char *channel = gtk_label_get_text(GTK_LABEL(child));

//     strncpy(current_channel, channel, sizeof(current_channel));
//     current_channel[sizeof(current_channel) - 1] = '\0';

//     request_channel_history(current_channel); // Demander l'historique des messages
//     request_channel_users(current_channel);   // Demander la liste des utilisateurs
// }

// // Fonction pour envoyer un message au serveur
// void on_send_button_clicked(GtkWidget *widget, gpointer data)
// {
//     const char *message = gtk_entry_get_text(GTK_ENTRY(entry_message));
//     if (strlen(message) == 0 || strlen(current_channel) == 0)
//         return;

//     char buffer[512];
//     snprintf(buffer, sizeof(buffer), "MSG|%s|%s", current_channel, message);
//     send_message_to_server(buffer);

//     gtk_entry_set_text(GTK_ENTRY(entry_message), "");
// }

// // Fonction pour ajouter un message à la vue de chat
// // void append_message_to_view(const char *message)
// // {
// //     GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_chat));

// //     GtkTextIter end;
// //     gtk_text_buffer_get_end_iter(buffer, &end);

// //     // Format du message
// //     char formatted[512];
// //     snprintf(formatted, sizeof(formatted), "%s\n", message);

// //     // Insertion du message dans la vue
// //     gtk_text_buffer_insert(buffer, &end, formatted, -1);
// //     gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(text_view_chat), &end, 0.0, FALSE, 0, 0);
// // }

// void append_message_to_view(const char *username, const char *message)
// {
//     GtkWidget *message_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
//     gtk_widget_set_halign(message_container, GTK_ALIGN_START);

//     GtkWidget *message_label = gtk_label_new(NULL);
//     char *formatted_text = g_strdup_printf("<b>%s:</b> %s", username, message);
//     gtk_label_set_markup(GTK_LABEL(message_label), formatted_text);
//     g_free(formatted_text);

//     // TODO: Add right click menu for reactions
//     // g_signal_connect(message_label, "button-press-event", G_CALLBACK(on_right_click), NULL);

//     gtk_widget_set_halign(message_label, GTK_ALIGN_START);
//     gtk_box_append(GTK_BOX(message_container), message_label);

//     // Create a box for reactions
//     GtkWidget *reactions_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
//     gtk_widget_set_name(reactions_box, "reactions_box");
//     gtk_widget_set_halign(reactions_box, GTK_ALIGN_START);
//     gtk_box_append(GTK_BOX(message_container), reactions_box);

//     // Add a reaction label (this can be updated later with actual reactions)
//     gtk_box_append(GTK_BOX(messages_box), message_container);
// }

// // Fonction pour afficher la liste des utilisateurs dans le canal actif
// void show_channel_users(const char *users_list)
// {
//     // Mise à jour du label pour afficher la liste des utilisateurs
//     char formatted[1024];
//     snprintf(formatted, sizeof(formatted), "Utilisateurs:\n%s", users_list);
//     gtk_label_set_text(GTK_LABEL(label_users), formatted);
// }

// // Create the login window
// GtkWidget *create_login_window(GtkApplication *app)
// {
//     GtkWidget *window = gtk_application_window_new(app);
//     gtk_window_set_title(GTK_WINDOW(window), "Log In");
//     gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

//     GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
//     gtk_window_set_child(GTK_WINDOW(window), box);

//     GtkWidget *entry_username = gtk_entry_new();
//     gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), "Email");
//     gtk_box_append(GTK_BOX(box), entry_username);

//     GtkWidget *hint_username = gtk_label_new("Use only Latin letters, '-' and ''' allowed.");
//     gtk_widget_set_halign(hint_username, GTK_ALIGN_START);
//     gtk_box_append(GTK_BOX(box), hint_username);

//     GtkWidget *entry_password = gtk_entry_new();
//     gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
//     gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
//     gtk_box_append(GTK_BOX(box), entry_password);

//     GtkWidget *hint_password = gtk_label_new("6–20 chars, 1 uppercase, 1 lowercase, 1 digit, 1 symbol.");
//     gtk_widget_set_halign(hint_password, GTK_ALIGN_START);
//     gtk_box_append(GTK_BOX(box), hint_password);

//     GtkWidget *login_button = gtk_button_new_with_label("Log In");

//     LoginData *data = g_new(LoginData, 1);
//     data->app = app;
//     data->entry_username = entry_username;
//     data->entry_password = entry_password;

//     g_signal_connect_data(login_button, "clicked", G_CALLBACK(on_login_button_clicked), data, (GClosureNotify)g_free, 0);
//     gtk_box_append(GTK_BOX(box), login_button);

//     GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");
//     g_signal_connect_swapped(cancel_button, "clicked", G_CALLBACK(gtk_window_destroy), window);
//     gtk_box_append(GTK_BOX(box), cancel_button);

//     return window;
// }

// // Create the register window
// GtkWidget *create_register_window(GtkApplication *app)
// {
//     GtkWidget *window = gtk_application_window_new(app);
//     gtk_window_set_title(GTK_WINDOW(window), "Register");
//     gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);

//     GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
//     gtk_window_set_child(GTK_WINDOW(window), box);

//     GtkWidget *entry_username = gtk_entry_new();
//     gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), "Username");
//     gtk_box_append(GTK_BOX(box), entry_username);

//     GtkWidget *hint_username = gtk_label_new("Use only Latin letters, '-' and ''' allowed.");
//     gtk_widget_set_halign(hint_username, GTK_ALIGN_START);
//     gtk_box_append(GTK_BOX(box), hint_username);

//     GtkWidget *entry_email = gtk_entry_new();
//     gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Email");
//     gtk_box_append(GTK_BOX(box), entry_email);

//     GtkWidget *hint_email = gtk_label_new("Must include '@'.");
//     gtk_widget_set_halign(hint_email, GTK_ALIGN_START);
//     gtk_box_append(GTK_BOX(box), hint_email);

//     GtkWidget *entry_password = gtk_entry_new();
//     gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
//     gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
//     gtk_box_append(GTK_BOX(box), entry_password);

//     GtkWidget *hint_password = gtk_label_new("6–20 chars, 1 uppercase, 1 lowercase, 1 digit, 1 symbol.");
//     gtk_widget_set_halign(hint_password, GTK_ALIGN_START);
//     gtk_box_append(GTK_BOX(box), hint_password);

//     RegisterData *reg_data = g_new(RegisterData, 1);
//     reg_data->app = app;
//     reg_data->entry_username = entry_username;
//     reg_data->entry_email = entry_email;
//     reg_data->entry_password = entry_password;

//     GtkWidget *register_button = gtk_button_new_with_label("Register");
//     g_signal_connect_data(register_button, "clicked", G_CALLBACK(on_register_button_clicked), reg_data, (GClosureNotify)g_free, 0);
//     gtk_box_append(GTK_BOX(box), register_button);

//     GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");
//     g_signal_connect_swapped(cancel_button, "clicked", G_CALLBACK(gtk_window_destroy), window);
//     gtk_box_append(GTK_BOX(box), cancel_button);

//     return window;
// }

// // Show the login window
// void show_login_window(GtkApplication *app, gpointer user_data)
// {
//     GtkWidget *window = create_login_window(app);
//     gtk_window_present(GTK_WINDOW(window));
// }

// // Show the register window
// void show_register_window(GtkApplication *app, gpointer user_data)
// {
//     GtkWidget *window = create_register_window(app);
//     gtk_window_present(GTK_WINDOW(window));
// }

// // Application activation
// void on_app_activate(GtkApplication *app, gpointer user_data)
// {
//     GtkWidget *window = create_welcome_window(app);
//     gtk_window_present(GTK_WINDOW(window));
// }

// // gboolean on_message_right_click(GtkWidget *widget, GdkEvent *event, gpointer user_data)
// // {
// //     if (event->type == GDK_BUTTON_PRESS && event->button == 3) // Right-click
// //     {
// //         GtkWidget *emoji_menu = create_emoji_selector("1"); // TODO: замінити "1" на реальний message_id
// //         gtk_menu_popup_at_pointer(GTK_MENU(emoji_menu), (GdkEvent *)event);
// //         return TRUE;
// //     }
// //     return FALSE;
// // }

// // gboolean on_message_right_click(GtkWidget *widget, GdkEvent *event, gpointer user_data)
// // {
// //     if (event->type == GDK_BUTTON_PRESS)
// //     {
// //         GdkEventButton *button_event = (GdkEventButton *)event;
// //         if (button_event->button == GDK_BUTTON_SECONDARY) // Right click
// //         {
// //             GtkWidget *menu = create_emoji_selector("1"); // Replace with actual message ID
// //             gtk_menu_popup_at_pointer(GTK_MENU(menu), (GdkEvent *)event);
// //             return TRUE;
// //         }
// //     }
// //     return FALSE;
// // }

// GtkWidget *create_emoji_selector(const char *message_id)
// {
//     GtkWidget *menu = gtk_menu_new();

//     const char *emojis[] = {"👍", "❤️", "😂", "😮", "😢", "😡"};
//     for (int i = 0; i < sizeof(emojis) / sizeof(emojis[0]); i++)
//     {
//         GtkWidget *item = gtk_menu_item_new_with_label(emojis[i]);
//         g_object_set_data(G_OBJECT(item), "message_id", (gpointer)message_id);
//         g_object_set_data(G_OBJECT(item), "emoji", (gpointer)emojis[i]);
//         g_signal_connect(item, "activate", G_CALLBACK(send_reaction_request), NULL);
//         gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
//     }

//     gtk_widget_show_all(menu);
//     return menu;
// }

// // void send_reaction_request(GtkMenuItem *item, gpointer user_data)
// // {
// //     const char *message_id = g_object_get_data(G_OBJECT(item), "message_id");
// //     const char *emoji = g_object_get_data(G_OBJECT(item), "emoji");

// //     if (strlen(current_channel) == 0)
// //         return;

// //     char buffer[256];
// //     snprintf(buffer, sizeof(buffer), "REACT|%s|%s|%s", current_channel, message_id, emoji);
// //     send_message_to_server(buffer);
// // }

// void send_reaction_request(GtkWidget *widget, gpointer user_data)
// {
//     const char *message_id = g_object_get_data(G_OBJECT(widget), "message_id");
//     const char *emoji = g_object_get_data(G_OBJECT(widget), "emoji");

//     if (!message_id || !emoji)
//     {
//         g_warning("Missing message_id or emoji");
//         return;
//     }

//     //     g_print("Sending reaction: %s for message ID: %s\n", emoji, message_id);
//     // }

//     // 🛰️ Example of sending a reaction — modify with your actual socket/API call
//     char request[256];
//     snprintf(request, sizeof(request), "REACT %s %s\n", message_id, emoji);
//     send_to_server(request);
// }

// void update_message_reactions(const char *message_id, const char *emoji, int count)
// {

//     GList *children = gtk_widget_get_children(GTK_WIDGET(text_view_chat));
//     for (GList *iter = children; iter != NULL; iter = iter->next)
//     {
//         GtkWidget *container = GTK_WIDGET(iter->data);
//         GtkWidget *reactions_box = gtk_widget_get_first_child(container);
//         while (reactions_box && strcmp(gtk_widget_get_name(reactions_box), "reactions_box") != 0)
//         {
//             reactions_box = gtk_widget_get_next_sibling(reactions_box);
//         }

//         if (reactions_box)
//         {
//             char buf[64];
//             snprintf(buf, sizeof(buf), "%s %d", emoji, count);
//             GtkWidget *reaction_label = gtk_label_new(buf);
//             gtk_box_append(GTK_BOX(reactions_box), reaction_label);
//             gtk_widget_show(reaction_label);
//             break;
//         }
//     }
// }