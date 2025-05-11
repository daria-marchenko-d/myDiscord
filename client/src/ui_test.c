// #include <gtk/gtk.h>
// #include <string.h>
// #include <stdlib.h>
// #include <stdio.h>

// #include "ui.h"
// #include "ui_callback.h"
// #include "network.h"
// #include "db.h"

// GtkWidget *window_welcome;
// GtkWidget *window_login;
// GtkWidget *window_register;
// GtkWidget *window_main;
// GtkWidget *text_view_chat;
// GtkWidget *entry_username;
// GtkWidget *entry_password;
// GtkWidget *entry_email;
// GtkWidget *channel_list;
// GtkWidget *entry_message;
// GtkWidget *label_users;
// GtkWidget *emoji_selector;

// GtkWidget *create_welcome_window(GtkApplication *app);
// GtkWidget *create_login_window(GtkApplication *app);
// GtkWidget *create_register_window(GtkApplication *app);
// GtkWidget *create_main_window(GtkApplication *app);

// void show_login_window(GtkApplication *app);
// void show_register_window(GtkApplication *app);

// void append_message_to_view(const char *username, const char *message);
// void update_channel_list(const char *channels[]);
// void show_channel_users(const char *users_list);
// void create_emoji_selector(int x, int y);
// void update_message_reactions(const char *emoji, int count);

// void init_ui(GtkApplication *app)
// {
//     create_welcome_window(app);
// }

// void create_welcome_window(GtkApplication *app)
// {
//     window_welcome = gtk_application_window_new(app);
//     gtk_window_set_title(GTK_WINDOW(window_welcome), "Welcome");
//     gtk_window_set_default_size(GTK_WINDOW(window_welcome), 400, 300);

//     GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
//     gtk_widget_set_margin_top(box, 30);
//     gtk_widget_set_margin_bottom(box, 30);
//     gtk_widget_set_margin_start(box, 30);
//     gtk_widget_set_margin_end(box, 30);
//     gtk_window_set_child(GTK_WINDOW(window_welcome), box);

//     GtkWidget *label = gtk_label_new("Welcome to ChatApp");
//     gtk_box_append(GTK_BOX(box), label);

//     GtkWidget *login_button = gtk_button_new_with_label("Log In");
//     g_signal_connect_swapped(login_button, "clicked", G_CALLBACK(show_login_window), app);
//     gtk_box_append(GTK_BOX(box), login_button);

//     GtkWidget *register_button = gtk_button_new_with_label("Register");
//     g_signal_connect_swapped(register_button, "clicked", G_CALLBACK(show_register_window), app);
//     gtk_box_append(GTK_BOX(box), register_button);

//     gtk_window_present(GTK_WINDOW(window_welcome));
// }

// void show_login_window(GtkApplication *app)
// {
//     create_login_window(app);
//     gtk_window_destroy(GTK_WINDOW(window_welcome));
// }

// void show_register_window(GtkApplication *app)
// {
//     create_register_window(app);
//     gtk_window_destroy(GTK_WINDOW(window_welcome));
// }

// void create_login_window(GtkApplication *app)
// {
//     window_login = gtk_application_window_new(app);
//     gtk_window_set_title(GTK_WINDOW(window_login), "Log In");
//     gtk_window_set_default_size(GTK_WINDOW(window_login), 400, 300);

//     GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
//     gtk_widget_set_margin_top(box, 20);
//     gtk_widget_set_margin_bottom(box, 20);
//     gtk_widget_set_margin_start(box, 20);
//     gtk_widget_set_margin_end(box, 20);
//     gtk_window_set_child(GTK_WINDOW(window_login), box);

//     entry_email = gtk_entry_new();
//     gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Email");
//     gtk_box_append(GTK_BOX(box), entry_email);

//     GtkWidget *hint_email = gtk_label_new("Must include '@'.");
//     gtk_widget_set_halign(hint_email, GTK_ALIGN_START);
//     gtk_box_append(GTK_BOX(box), hint_email);

//     entry_password = gtk_entry_new();
//     gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
//     gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
//     gtk_box_append(GTK_BOX(box), entry_password);

//     GtkWidget *hint_password = gtk_label_new("6–20 chars, 1 uppercase, 1 lowercase, 1 digit, 1 symbol.");
//     gtk_widget_set_halign(hint_password, GTK_ALIGN_START);
//     gtk_box_append(GTK_BOX(box), hint_password);

//     GtkWidget *login_button = gtk_button_new_with_label("Log In");
//     g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), NULL);
//     gtk_box_append(GTK_BOX(box), login_button);

//     GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");
//     g_signal_connect_swapped(cancel_button, "clicked", G_CALLBACK(gtk_window_destroy), window_login);
//     gtk_box_append(GTK_BOX(box), cancel_button);

//     gtk_window_present(GTK_WINDOW(window_login));
// }

// void create_register_window(GtkApplication *app)
// {
//     window_register = gtk_application_window_new(app);
//     gtk_window_set_title(GTK_WINDOW(window_register), "Register");
//     gtk_window_set_default_size(GTK_WINDOW(window_register), 400, 400);

//     GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
//     gtk_widget_set_margin_top(box, 20);
//     gtk_widget_set_margin_bottom(box, 20);
//     gtk_widget_set_margin_start(box, 20);
//     gtk_widget_set_margin_end(box, 20);
//     gtk_window_set_child(GTK_WINDOW(window_register), box);

//     entry_username = gtk_entry_new();
//     gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), "Username");
//     gtk_box_append(GTK_BOX(box), entry_username);

//     GtkWidget *hint_username = gtk_label_new("Use only Latin letters, '-' and ''' allowed.");
//     gtk_widget_set_halign(hint_username, GTK_ALIGN_START);
//     gtk_box_append(GTK_BOX(box), hint_username);

//     entry_email = gtk_entry_new();
//     gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Email");
//     gtk_box_append(GTK_BOX(box), entry_email);

//     GtkWidget *hint_email = gtk_label_new("Must include '@'.");
//     gtk_widget_set_halign(hint_email, GTK_ALIGN_START);
//     gtk_box_append(GTK_BOX(box), hint_email);

//     entry_password = gtk_entry_new();
//     gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
//     gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
//     gtk_box_append(GTK_BOX(box), entry_password);

//     GtkWidget *hint_password = gtk_label_new("6–20 chars, 1 uppercase, 1 lowercase, 1 digit, 1 symbol.");
//     gtk_widget_set_halign(hint_password, GTK_ALIGN_START);
//     gtk_box_append(GTK_BOX(box), hint_password);

//     GtkWidget *register_button = gtk_button_new_with_label("Register");
//     g_signal_connect(register_button, "clicked", G_CALLBACK(on_register_button_clicked), NULL);
//     gtk_box_append(GTK_BOX(box), register_button);

//     GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");
//     g_signal_connect_swapped(cancel_button, "clicked", G_CALLBACK(gtk_window_destroy), window_register);
//     gtk_box_append(GTK_BOX(box), cancel_button);

//     gtk_window_present(GTK_WINDOW(window_register));
// }

// void create_main_window(GtkApplication *app)
// {
//     window_main = gtk_application_window_new(app);
//     gtk_window_set_title(GTK_WINDOW(window_main), "Main Chat");
//     gtk_window_set_default_size(GTK_WINDOW(window_main), 800, 600);

//     GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
//     gtk_window_set_child(GTK_WINDOW(window_main), main_box);

//     // Channel list
//     channel_list = gtk_list_box_new();
//     gtk_widget_set_size_request(channel_list, 200, -1);
//     g_signal_connect(channel_list, "row-selected", G_CALLBACK(on_channel_selected), NULL);
//     gtk_box_append(GTK_BOX(main_box), channel_list);

//     // Chat area
//     GtkWidget *chat_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
//     gtk_box_append(GTK_BOX(main_box), chat_box);

//     text_view_chat = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
//     gtk_widget_set_vexpand(text_view_chat, TRUE);
//     gtk_box_append(GTK_BOX(chat_box), text_view_chat);

//     entry_message = gtk_entry_new();
//     gtk_entry_set_placeholder_text(GTK_ENTRY(entry_message), "Type your message...");
//     gtk_box_append(GTK_BOX(chat_box), entry_message);

//     GtkWidget *send_button = gtk_button_new_with_label("Send");
//     g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_button_clicked), NULL);
//     gtk_box_append(GTK_BOX(chat_box), send_button);

//     // Users
//     GtkWidget *users_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
//     gtk_widget_set_size_request(users_box, 200, -1);
//     gtk_box_append(GTK_BOX(main_box), users_box);

//     label_users = gtk_label_new("Utilisateurs:");
//     gtk_box_append(GTK_BOX(users_box), label_users);

//     gtk_window_present(GTK_WINDOW(window_main));
// }

// void append_message_to_view(const char *username, const char *message)
// {
//     GtkWidget *message_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
//     gtk_widget_set_halign(message_container, GTK_ALIGN_START);

//     GtkWidget *message_label = gtk_label_new(NULL);
//     char *formatted_text = g_strdup_printf("<b>%s:</b> %s", username, message);
//     gtk_label_set_markup(GTK_LABEL(message_label), formatted_text);
//     g_free(formatted_text);

//     gtk_widget_set_halign(message_label, GTK_ALIGN_START);
//     gtk_box_append(GTK_BOX(message_container), message_label);

//     // Reactions box
//     GtkWidget *reactions_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
//     gtk_widget_set_name(reactions_box, "reactions_box");
//     gtk_widget_set_halign(reactions_box, GTK_ALIGN_START);
//     gtk_box_append(GTK_BOX(message_container), reactions_box);

//     gtk_box_append(GTK_BOX(text_view_chat), message_container);
//     gtk_widget_show(message_container);
// }

// void update_channel_list(const char *channels[])
// {
//     gtk_list_box_remove_all(GTK_LIST_BOX(channel_list));
//     for (int i = 0; channels[i] != NULL; i++)
//     {
//         GtkWidget *row = gtk_label_new(channels[i]);
//         gtk_list_box_insert(GTK_LIST_BOX(channel_list), row, -1);
//     }
//     gtk_widget_show(channel_list);
// }

// void show_channel_users(const char *users_list)
// {
//     char formatted[1024];
//     snprintf(formatted, sizeof(formatted), "Utilisateurs:\n%s", users_list);
//     gtk_label_set_text(GTK_LABEL(label_users), formatted);
// }

// void create_emoji_selector(int x, int y)
// {
//     emoji_selector = gtk_popover_new();
//     GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
//     gtk_popover_set_child(GTK_POPOVER(emoji_selector), box);

//     const char *emojis[] = {"😊", "😂", "👍", "❤️", "😢", NULL};
//     for (int i = 0; emojis[i] != NULL; i++)
//     {
//         GtkWidget *btn = gtk_button_new_with_label(emojis[i]);
//         g_signal_connect(btn, "clicked", G_CALLBACK(send_reaction_request), (gpointer)emojis[i]);
//         gtk_box_append(GTK_BOX(box), btn);
//     }

//     gtk_widget_show(emoji_selector);
//     gtk_popover_popup(GTK_POPOVER(emoji_selector));
// }

// void update_message_reactions(const char *emoji, int count)
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

// void on_app_activate(GtkApplication *app, gpointer user_data)
// {
//     GtkWidget *window = create_welcome_window(app);
//     gtk_window_present(GTK_WINDOW(window));
// }
