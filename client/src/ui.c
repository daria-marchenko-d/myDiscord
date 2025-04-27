#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include "db.h"
#include "ui.h"

// Functions
void show_login_window(GtkApplication *app, gpointer user_data);
void show_register_window(GtkApplication *app, gpointer user_data);
void on_login_button_clicked(GtkWidget *widget, gpointer user_data);
void on_register_button_clicked(GtkWidget *widget, gpointer user_data);

// Create the main window
GtkWidget *create_main_window(GtkApplication *app)
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

    int user_id;
    char role[20];

    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_root(widget));

    if (authenticate_user(username, password, &user_id, role))
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

    if (add_user(username, "", email, password))
    {
        GtkAlertDialog *alert = gtk_alert_dialog_new("User successfully registered!");
        gtk_alert_dialog_choose(alert, parent_window, NULL, on_register_success_response, parent_window);
        g_object_unref(alert);
    }
    else
    {
        GtkAlertDialog *alert = gtk_alert_dialog_new("Error. Username or email already exists.");
        gtk_alert_dialog_show(alert, parent_window);
        g_object_unref(alert);
    }
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
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), "Username or Email");
    gtk_box_append(GTK_BOX(box), entry_username);

    GtkWidget *entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_box_append(GTK_BOX(box), entry_password);

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

    GtkWidget *entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Email");
    gtk_box_append(GTK_BOX(box), entry_email);

    GtkWidget *entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_box_append(GTK_BOX(box), entry_password);

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
    GtkWidget *window = create_main_window(app);
    gtk_window_present(GTK_WINDOW(window));
}
