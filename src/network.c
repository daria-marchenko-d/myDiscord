#include "network.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#endif

int sock = -1;

int connect_to_server(const char *host, int port) {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return -1;
#endif

    struct sockaddr_in server_addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &server_addr.sin_addr);

    return connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
}

void send_message(const char *msg) {
    if (sock >= 0 && msg) {
        printf("[Client] Envoi : %s\n", msg); // ← ce log est crucial
        send(sock, msg, strlen(msg), 0);
    }
}

int recv_message(char *buffer, int bufsize) {
    if (sock >= 0)
        return recv(sock, buffer, bufsize - 1, 0);
    return -1;
}
