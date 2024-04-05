#include "common.h"

void load_users(User *users, int max_users) {
    load_auth_file(users, max_users);
}

bool authenticate_user(User *users, int max_users, const char *username, const char *password) {
    for (int i = 0; i < max_users; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            users[i].is_authenticated = true;
            return true;
        }
    }
    return false;
}

void save_users(User *users, int num_users) {
    FILE *auth_file = fopen(".auth", "w");
    if (!auth_file) {
        perror("Error opening .auth file");
        return;
    }

    for (int i = 0; i < num_users; i++) {
        fprintf(auth_file, "%s %s\n", users[i].username, users[i].password);
    }

    fclose(auth_file);
}

void load_auth_file(User *users, int max_users) {
    FILE *auth_file = fopen(".auth", "r");
    if (!auth_file) {
        perror("Error opening .auth file");
        return;
    }

    int i = 0;
    while (i < max_users && fscanf(auth_file, "%s %s", users[i].username, users[i].password) == 2) {
        users[i].is_authenticated = false;
        i++;
    }

    fclose(auth_file);
}

void log_message(const char *message) {
    // Логирование сообщения
    // Пример вывода сообщения в консоль:
    printf("%s\n", message);
}

void monitor_activity(const char *username, const char *activity) {
    // Мониторинг активности пользователя
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    char log_message_buff[MAX_LOG_LENGTH];
    snprintf(log_message_buff, MAX_LOG_LENGTH, "[%s] %s: %s\n", asctime(timeinfo), username, activity);
    log_message(log_message_buff);
}
