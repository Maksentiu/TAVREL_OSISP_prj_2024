#ifndef CLIENT_H
#define CLIENT_H

#include "../common/common.h"

// Функции для работы с FTP-клиентом
void upload_file(User *user, const char *file_path, const char *ftp_url);
void download_file(User *user, const char *ftp_url, const char *local_path);

#endif
