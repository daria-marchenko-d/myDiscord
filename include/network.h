#ifndef NETWORK_H
#define NETWORK_H

int connect_to_server(const char *host, int port);
void send_message(const char *msg);
int recv_message(char *buffer, int bufsize);

#endif
