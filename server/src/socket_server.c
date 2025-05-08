#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "crypto.h"
#include <ws2tcpip.h>
#include "config.h" // Defines SERVER_PORT
#include "db.h"     // For connecting to database functions (especially authenticate_user)

int authenticate_user(const char *email, const char *hashed_password); // Keeps the old declaration for compatibility

// User roles definition
typedef enum
{
    USER,
    MODERATOR,
    ADMINISTRATOR
} UserRole;

// Server initialization function
int main()
{
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address, client_address;
    int opt = 1;
    int client_addrlen = sizeof(client_address);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        fprintf(stderr, "WSAStartup error\n");
        exit(EXIT_FAILURE);
    }

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        perror("Socket error");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Allow reuse of address
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
    {
        perror("Setsockopt error");
        closesocket(server_fd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Configure address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind error");
        closesocket(server_fd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 10) < 0)
    {
        perror("Listen error");
        closesocket(server_fd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

    // Loop to accept incoming connections
    while (1)
    {
        printf("Waiting for connections...\n");

        new_socket = accept(server_fd, (struct sockaddr *)&client_address, &client_addrlen);
        if (new_socket == INVALID_SOCKET)
        {
            perror("Accept error");
            continue;
        }

        printf("Client connected (socket: %d)\n", (int)new_socket);
        handle_client(new_socket); // Handle client interaction
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}

// ========== Client Handling ==========
// Client handling function
void handle_client(SOCKET client_socket)
{
    char buffer[1024] = {0};
    int valread;

    printf("Handling client (socket %d)...\n", (int)client_socket);

    // 1. Read the message (authentication)
    valread = recv(client_socket, buffer, sizeof(buffer), 0);
    if (valread <= 0)
    {
        printf("Client %d disconnected or error\n", (int)client_socket);
        closesocket(client_socket);
        return;
    }

    buffer[valread] = '\0'; // Null-terminate the string
    printf("Received message from client %d: %s\n", (int)client_socket, buffer);

    // 2. Extract the email and password
    char *email = strtok(buffer, " ");
    char *password = strtok(NULL, " ");

    char *response;
    UserRole role;

    // 3. Check the credentials via the database
    if (email && password)
    {
        // Hash the password before sending it to the server
        char hashed_password[65]; // 64 characters for SHA-256 hash
        hash_password(password, hashed_password);

        if (authenticate_user(email, hashed_password))
        {
            char role[20];
            if (get_user_role(email, role))
            {
                printf("User %s authenticated. Role: %s\n", (int)client_socket, role);

                char response_with_role[256];
                sprintf(response_with_role, "Authentication successful! Role: %s", role);
                send(client_socket, response_with_role, strlen(response_with_role), 0);
            }
            else
            {

                response = "Authentication successful!";
                send(client_socket, response, strlen(response), 0);
            }
        }
        else
        {
            response = "Authentication failed.";
            send(client_socket, response, strlen(response), 0);
            printf("Authentication failed for client %d\n", (int)client_socket);
        }
    }
    else
    {
        response = "Invalid input format.";
        send(client_socket, response, strlen(response), 0);
        printf("Invalid email or password for client %d\n", (int)client_socket);
    }

    // 4. Send the response to the client
    send(client_socket, response, strlen(response), 0);
    printf("Response sent: %s\n", response);

    // 5. Display the assigned role (if authentication is successful)
    if (strcmp(response, "Authentication successful!") == 0)
    {
        if (role == USER)
        {
            printf("Role assigned: USER\n");
        }
        else if (role == MODERATOR)
        {
            printf("Role assigned: MODERATOR\n");
        }
        else if (role == ADMINISTRATOR)
        {
            printf("Role assigned: ADMINISTRATOR\n");
        }
    }

    // 6. End the communication with the client
    closesocket(client_socket);
}
