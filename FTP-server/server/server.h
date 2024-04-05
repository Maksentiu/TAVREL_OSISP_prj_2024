#ifndef SERVER_H
#define SERVER_H

#include "../common/common.h"
#include <stdbool.h>

#define MAX_USERS 10
#define DEFAULT_PORT 21

// Структура для представления FTP-сервера
typedef struct {
    User users[MAX_USERS];
    int num_users;
    int port;
} FTPServer;

// Функции для работы с FTP-сервером
void init_ftp_server(FTPServer *server, int port);
void start_ftp_server(FTPServer *server);

#endif
