#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <paths.h>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <stdbool.h>

#include "utils.h"

#define LOG_DIR_MODE   0700
#define LOG_FILE_MODE  "w"

char *log_file_path = NULL;
FILE *log_file_stream = NULL;


int close_log_stream(void)
{
    if (fclose(log_file_stream) == EOF) {
        fprintf(stderr, "Failed to close log file \"%s\": %s\n", 
                log_file_path, strerror(errno));
        return -1;
    }
    log_file_stream = NULL;

    free(log_file_path);
    log_file_path = NULL;

    return 0;
}

static int open_log_file(const char *log_path)
{
    if (log_path == NULL) {
        fprintf(stderr, "Log file path is NULL\n");
        close_log_stream();
        return -1;
    }

    log_file_stream = fopen(log_path, LOG_FILE_MODE);
    if (log_file_stream == NULL) {
        fprintf(stderr, "Failed to open log file \"%s\": %s\n", 
                log_path, strerror(errno));
        return -1;
    }

    return 0;
}

int init_log_stream(const char *log_path)
{
    if (log_path == NULL) {
        if (asprintf(&log_file_path, "%s%s", getenv("HOME"), LOG_DEFAULT_PATH) == -1) {
            fprintf(stderr, "Failed to create default log file path string\n");
            return -1;
        }
    } else {
        log_file_path = strdup(log_path);
        if (!log_file_path) {
            fprintf(stderr, "Failed to duplicate log file path \"%s\": %s\n", 
                    log_path, strerror(errno));
            return -1;
        }
    }

    if (open_log_file(log_file_path) == -1) {
        free(log_file_path);
        return -1;
    }

    return 0;
}

void print_see_log_msg(void)
{
    fprintf(stderr, "See log file for more information: %s\n", log_file_path);
}

