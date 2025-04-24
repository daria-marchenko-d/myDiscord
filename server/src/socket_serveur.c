#include <stdio.h> //Pour afficher du texte (comme messages de log)
#include <stdlib.h> //Pour des fonctions générales (comme quitter le programme en cas d'erreur)
#include <string.h> //Pour manipuler du texte(chaînes de caractères)...

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
    #define CLOSESOCKET closesocket
    #define CLEANUP WSACleanup()
    #define INIT_SOCKETS() \
        WSADATA wsaData; \
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { \
            fprintf(stderr, "WSAStartup a échoué\n"); \
            exit(EXIT_FAILURE); \
        }
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    typedef int SOCKET;
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define CLOSESOCKET close
    #define CLEANUP
    #define INIT_SOCKETS()
#endif

#include "config.h"  // Assurez-vous que #define SERVER_PORT 12345 par exemple

// Déclaration de Nelson
void handle_client(SOCKET client_socket);

// Définir les rôles
typedef enum {
    USER,
    MODERATOR,
    ADMINISTRATOR
} UserRole;

int main() {
    INIT_SOCKETS();

    SOCKET server_fd, new_socket;
    struct sockaddr_in address, client_address;
    int opt = 1;
    socklen_t client_addrlen = sizeof(client_address);

    //Créer le socket (comme demander une prise électrique)
    if((server_fd = socket(AF_INET,SOCK_STREAM, 0))== INVALID_SOCKET) {
        perror("Erreur lors de la création du socket");
        CLEANUP;
        exit(EXIT_FAILURE);
    }

#ifndef _WIN32
    // Autoriser la réutilisation d'adresse (Linux/macOS uniquement)
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Erreur setsockopt");
        CLEANUP;
        exit(EXIT_FAILURE);
    }    
#endif

    // 2. Configurer l'adresse
    address.sin_family = AF_INET; //On utlise l'adresse IP 
    address.sin_addr.s_addr = INADDR_ANY; //Ecouter sur toutes les adresses IP de la machine
    address.sin_port = htons(SERVER_PORT); //Utiliser le port défini dans Config.h

    //Lier le socket à l'adresse et au port (brancher la prise)
    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address)) == SOCKET_ERROR) {
        perror("Erreur lors de la création du socket");
        CLEANUP;
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, 10) == SOCKET_ERROR) {// Peut accepter jusqu'à 10 connexions en attente
        perror("Erreur lors de la mise en écoute");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en écoute sur le port %d...\n", SERVER_PORT);

    //Accepter les connexions des clients : Créer une boucle infinie pour attendre et accepter chaque nouvelle connexion 

    while(1) {
        printf("En attente d'une nouvelle connexion...\n");
        new_socket = accept(server_fd, (struct sockaddr *)&client_address, &client_addrlen);
        if (new_socket == INVALID_SOCKET) {
            perror("Erreur lors de l'acceptation");
            continue;//Continuer à attendre d'autres connexions même si une échoue
        }

        printf("Client connecté (socket : %d\n", (int)new_socket);
        handle_client(new_socket);    
    }
    return 0;
}

//ici, commence la fonction de Nelson 

void handle_client(SOCKET client_socket) {
    char buffer[1024] = {0};
    int valread;

    printf("Gestion du client sur le socket %d...\n", (int)client_socket);

    // 1. Recevoir les infos d'authentification
    valread = recv(client_socket, buffer, sizeof(buffer), 0);
    if (valread > 0) {
        printf("Message reçu du client %d: %s\n", (int)client_socket, buffer);
    } else if (valread == 0) {
        printf("Client %d déconnecté.\n", (int)client_socket);
        CLOSESOCKET(client_socket);
        return;
    } else {
        perror("Erreur lors de la réception");
        CLOSESOCKET(client_socket);
        return;
    }

    // 2. Simuler l'authentification
    const char *response;
    UserRole role;

    if (strcmp(buffer, "testuser testpass") == 0) {
        response = "Authentification réussie!";
        role = USER;
        printf("Client %d authentifié (rôle : UTILISATEUR).\n", (int)client_socket);
    } else {
        response = "Authentification échouée.";
    }

    // 3. Envoyer la réponse
    send(client_socket, response, (int)strlen(response), 0);
    printf("Réponse envoyée au client %d: %s\n", (int)client_socket, response);

    CLOSESOCKET(client_socket);
}