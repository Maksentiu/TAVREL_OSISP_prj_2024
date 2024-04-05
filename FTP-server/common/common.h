#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_PATH_LENGTH 256
#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_LOG_LENGTH 256

// Структура для представления пользователя
typedef struct {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    bool is_authenticated;
} User;

// Функции для работы с пользователями
void load_users(User *users, int max_users);
bool authenticate_user(User *users, int max_users, const char *username, const char *password);
void save_users(User *users, int num_users);
void load_auth_file(User *users, int max_users);

// Функции для логирования и мониторинга
void log_message(const char *message);
void monitor_activity(const char *username, const char *activity);

#endif
