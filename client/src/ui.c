#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include "db.h"
#include "ui.h"

// Function declarations
void show_login_window(GtkApplication *app, gpointer user_data);
void show_register_window(GtkApplication *app, gpointer user_data);
void on_login_button_clicked(GtkWidget *widget, gpointer user_data);
void on_register_button_clicked(GtkWidget *widget, gpointer user_data);

// Create main window with Log In and Register buttons
GtkWidget *create_main_window(GtkApplication *app)
{
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *login_button;
    GtkWidget *register_button;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Welcome to MyDiscord");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), box);

    login_button = gtk_button_new_with_label("Log In");
    g_signal_connect(login_button, "clicked", G_CALLBACK(show_login_window), app);
    gtk_box_append(GTK_BOX(box), login_button);

    register_button = gtk_button_new_with_label("Register");
    g_signal_connect(register_button, "clicked", G_CALLBACK(show_register_window), app);
    gtk_box_append(GTK_BOX(box), register_button);

    return window;
}

// Create login window
GtkWidget *create_login_window(GtkApplication *app)
{
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *entry_username;
    GtkWidget *entry_password;
    GtkWidget *login_button;
    GtkWidget *cancel_button;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Log In");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), box);

    entry_username = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), "Username or Email");
    gtk_box_append(GTK_BOX(box), entry_username);

    entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_box_append(GTK_BOX(box), entry_password);

    login_button = gtk_button_new_with_label("Log In");
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), entry_username);
    gtk_box_append(GTK_BOX(box), login_button);

    cancel_button = gtk_button_new_with_label("Cancel");
    g_signal_connect_swapped(cancel_button, "clicked", G_CALLBACK(gtk_window_destroy), window);
    gtk_box_append(GTK_BOX(box), cancel_button);

    return window;
}

// Create registration window
GtkWidget *create_register_window(GtkApplication *app)
{
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *entry_username;
    GtkWidget *entry_email;
    GtkWidget *entry_password;
    GtkWidget *register_button;
    GtkWidget *cancel_button;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Register");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), box);

    entry_username = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), "Username");
    gtk_box_append(GTK_BOX(box), entry_username);

    entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Email");
    gtk_box_append(GTK_BOX(box), entry_email);

    entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_box_append(GTK_BOX(box), entry_password);

    register_button = gtk_button_new_with_label("Register");
    g_signal_connect(register_button, "clicked", G_CALLBACK(on_register_button_clicked), entry_username);
    gtk_box_append(GTK_BOX(box), register_button);

    cancel_button = gtk_button_new_with_label("Cancel");
    g_signal_connect_swapped(cancel_button, "clicked", G_CALLBACK(gtk_window_destroy), window);
    gtk_box_append(GTK_BOX(box), cancel_button);

    return window;
}

// Log In button handler
void on_login_button_clicked(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *username_entry = GTK_WIDGET(user_data);
    GtkWidget *password_entry = gtk_widget_get_prev_sibling(username_entry);

    const char *username = gtk_editable_get_text(GTK_EDITABLE(username_entry));
    const char *password = gtk_editable_get_text(GTK_EDITABLE(password_entry));

    int user_id;
    char role[20];

    if (authenticate_user(username, password, &user_id, role))
    {
        GtkAlertDialog *alert = gtk_alert_dialog_new("Login Successful!");
        gtk_alert_dialog_show(alert, GTK_WINDOW(gtk_widget_get_root(widget)));
        g_object_unref(alert);

        GtkWidget *new_window = create_main_window(NULL);
        gtk_window_present(GTK_WINDOW(new_window));
        gtk_window_destroy(GTK_WINDOW(gtk_widget_get_root(widget)));
    }
    else
    {
        GtkAlertDialog *alert = gtk_alert_dialog_new("Login Failed. Check your credentials.");
        gtk_alert_dialog_show(alert, GTK_WINDOW(gtk_widget_get_root(widget)));
        g_object_unref(alert);
    }
}

// Register button handler
void on_register_button_clicked(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *username_entry = GTK_WIDGET(user_data);
    GtkWidget *email_entry = gtk_widget_get_next_sibling(username_entry);
    GtkWidget *password_entry = gtk_widget_get_next_sibling(email_entry);

    const char *username = gtk_editable_get_text(GTK_EDITABLE(username_entry));
    const char *email = gtk_editable_get_text(GTK_EDITABLE(email_entry));
    const char *password = gtk_editable_get_text(GTK_EDITABLE(password_entry));

    if (add_user(username, "", email, password)) // Empty string for last_name
    {
        GtkAlertDialog *alert = gtk_alert_dialog_new("Registration Successful!");
        gtk_alert_dialog_show(alert, GTK_WINDOW(gtk_widget_get_root(widget)));
        g_object_unref(alert);
    }
    else
    {
        GtkAlertDialog *alert = gtk_alert_dialog_new("Registration Failed. User may already exist.");
        gtk_alert_dialog_show(alert, GTK_WINDOW(gtk_widget_get_root(widget)));
        g_object_unref(alert);
    }
}

// Show login window
void show_login_window(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window = create_login_window(app);
    gtk_window_present(GTK_WINDOW(window));
}

// Show register window
void show_register_window(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window = create_register_window(app);
    gtk_window_present(GTK_WINDOW(window));
}

// Entry point
void on_app_activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window = create_main_window(app);
    gtk_window_present(GTK_WINDOW(window));
}
