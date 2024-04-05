#include "../common/common.h"
#include "client.h"
#include <stdio.h>
#include <curl/curl.h>

void upload_file(User *user, const char *file_path, const char *ftp_url) {
    // Сетевая подсистема для загрузки файла
    CURL *curl;
    CURLcode res;
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, ftp_url);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_USERPWD, user->username);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, user->password);
        curl_easy_setopt(curl, CURLOPT_READDATA, file);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    fclose(file);
}

void download_file(User *user, const char *ftp_url, const char *local_path) {
    // Сетевая подсистема для скачивания файла
    CURL *curl;
    FILE *file = fopen(local_path, "wb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, ftp_url);
        curl_easy_setopt(curl, CURLOPT_USERPWD, user->username);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, user->password);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    fclose(file);
}
