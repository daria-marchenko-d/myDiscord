#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "ui.h"
#include "ui_callbacks.h" // ou "network.h" si tu utilises network.c/h

#define SERVER_IP "127.0.0.1" // Change this to the actual server IP
#define SERVER_PORT 12345       // Change this to the actual server port

int main(int argc, char *argv[]) {
    GtkWidget *login_window;
    GtkWidget *main_window;
    int client_socket;
    struct sockaddr_in server_addr;

    gtk_init(&argc, &argv);

    // Create the login window
    login_window = create_login_window();
    gtk_widget_show_all(login_window);

    // Create the main window (but don't show it yet)
    main_window = create_main_window();

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Could not create socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect failed");
        return 1;
    }

    printf("Connected to server\n");

    // Set up callbacks (example)
    // g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), &client_socket);

    // Pass the socket to the callbacks
    login_data_t *login_data = g_new(login_data_t, 1);
    login_data->socket = client_socket;
    login_data->login_window = login_window;
    login_data->main_window = main_window;

    GtkWidget *login_button = GTK_WIDGET(gtk_builder_get_object(builder, "login_button"));
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), login_data);

    GtkWidget *register_button = GTK_WIDGET(gtk_builder_get_object(builder, "register_button"));
        g_signal_connect(register_button, "clicked", G_CALLBACK(on_register_button_clicked), login_data);

    gtk_main();

    close(client_socket);
    g_free(login_data);

    return 0;
}