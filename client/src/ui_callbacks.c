#include "ui_callbacks.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ui.h"

// Callback function for login button
void on_login_button_clicked(GtkWidget *widget, login_data_t *data) {
    // Get username and password from the entry fields
    GtkWidget *username_entry = GTK_WIDGET(gtk_builder_get_object(builder, "username_entry"));
    GtkWidget *password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry"));

    const char *username = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(password_entry));

    // Format the login message
    char message[256];
    snprintf(message, sizeof(message), "LOGIN %s %s", username, password);

    // Send the message to the server
    send(data->socket, message, strlen(message), 0);

    // Receive the response from the server
    char buffer[256];
    ssize_t bytes_received = recv(data->socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Received: %s\n", buffer);

        // Check the response and show the main window if login is successful
        if (strcmp(buffer, "LOGIN_SUCCESS") == 0) {
            gtk_widget_hide(data->login_window);
            show_main_window();
        } else {
            // Display an error message
            GtkWidget *dialog;
            dialog = gtk_message_dialog_new(GTK_WINDOW(data->login_window),
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_OK,
                                             "Login failed. Invalid username or password.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }
    } else {
        perror("Receive failed");
    }
}

// Callback function for register button
void on_register_button_clicked(GtkWidget *widget, login_data_t *data) {
    // Get the values from the registration fields
     GtkWidget *register_username_entry = GTK_WIDGET(gtk_builder_get_object(builder, "register_username_entry"));
    GtkWidget *register_password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "register_password_entry"));
    GtkWidget *register_email_entry = GTK_WIDGET(gtk_builder_get_object(builder, "register_email_entry"));

    const char *username = gtk_entry_get_text(GTK_ENTRY(register_username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(register_password_entry));
     const char *email = gtk_entry_get_text(GTK_ENTRY(register_email_entry));

    // Format the register message
    char message[256];
    snprintf(message, sizeof(message), "REGISTER %s %s %s", username, password, email);

    // Send the message to the server
    send(data->socket, message, strlen(message), 0);

    // Receive the response from the server
    char buffer[256];
    ssize_t bytes_received = recv(data->socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Received: %s\n", buffer);

        // Check the response and display a message
        GtkWidget *dialog;
        if (strcmp(buffer, "REGISTER_SUCCESS") == 0) {
            dialog = gtk_message_dialog_new(GTK_WINDOW(data->login_window),
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_INFO,
                                             GTK_BUTTONS_OK,
                                             "Registration successful!");
        } else {
            dialog = gtk_message_dialog_new(GTK_WINDOW(data->login_window),
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_OK,
                                             "Registration failed.");
        }
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        perror("Receive failed");
    }
}