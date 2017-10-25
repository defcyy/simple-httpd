#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "http.h"
#include "file.h"
#include "common.h"


static void set_default_response_header(response *resp) {
    time_t raw_time;
    struct tm *time_info;
    char time_buffer[30];
    
    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(time_buffer, 30, HTTP_HEADER_DATE_FORMAT, time_info);
    DEBUG_LOG("date time: %s\n", time_buffer);
    set_response_header(resp, "Date", time_buffer);
    set_response_header(resp, "Server", SERVER_NAME);
}

int file_serv(int conn, const char *url) {
    DEBUG_LOG("file serv log ...\n");
    char *file_path;
    int ret;
    struct stat file_stat;
    response *resp;
    
    if (url[strlen(url) - 1] == '/') {
        file_path = malloc(strlen(ROOT) + strlen(url) + strlen(DEFAULT_INDEX_FILE) + 1);
        strcpy(file_path, ROOT);
        strcat(file_path, url);
        strcat(file_path, DEFAULT_INDEX_FILE);
    } else {
        file_path = malloc(strlen(ROOT) + strlen(url) + 1);
        strcpy(file_path, ROOT);
        strcat(file_path, url);
    }

    DEBUG_LOG("http response start ...\n");
    resp = response_new();
    set_default_response_header(resp);
    
    ret = stat(file_path, &file_stat);
    if (ret < 0) {
        ERROR_LOG("file not found: %s\n", file_path);
        char *content = "Not Found";
        
        set_response_status(resp, 404);
        set_response_header(resp, "Content-Type", "text/plain; charset=utf-8");
        set_response_content(resp, (void *)content, strlen(content) + 1);
    } else {
        set_response_status(resp, 200);
    }

    send_response(conn, resp);

    close(conn);
    free(file_path);
    response_release(resp);
    return 0;
}
