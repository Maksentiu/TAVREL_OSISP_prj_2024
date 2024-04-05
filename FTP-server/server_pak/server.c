#include "../common/common.h"
#include "server.h"
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void init_ftp_server(FTPServer *server, int port) {
    server->num_users = 0;
    server->port = port;
    load_users(server->users, MAX_USERS);
}

void start_ftp_server(FTPServer *server) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error opening socket");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server->port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        return;
    }

    listen(server_socket, 5);
    printf("FTP server started on port %d\n", server->port);

    while (true) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }

        // Аутентификация и авторизация клиента
        char username[MAX_USERNAME_LENGTH];
        char password[MAX_PASSWORD_LENGTH];
        recv(client_socket, username, MAX_USERNAME_LENGTH, 0);
        recv(client_socket, password, MAX_PASSWORD_LENGTH, 0);
        if (authenticate_user(server->users, MAX_USERS, username, password)) {
            send(client_socket, "Authenticated", strlen("Authenticated"), 0);
            monitor_activity(username, "Authentication successful");
        } else {
            send(client_socket, "Authentication failed", strlen("Authentication failed"), 0);
            monitor_activity(username, "Authentication failed");
        }

        close(client_socket);
    }

    close(server_socket);
    save_users(server->users, server->num_users);
}
