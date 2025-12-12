#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <libgen.h>

#include "../inc/utils.h"



int create_dir(const char *dir_path, int mode)
{
    int rv;
    DIR *dir = NULL;
    char *div_char = NULL;
    char *base_dir = NULL;

    // Check if directory exists
    errno = 0;
    dir = opendir(dir_path);
    if (dir != NULL) {
        closedir(dir);
        return 0;
    }
    if (errno != ENOENT) {
        fprintf(stderr, "Unable to open directory \"%s\": %s\n", 
                dir_path, strerror(errno));
        return -1;
    }

    // Recurse if parent directory
    div_char = strchr(dir_path, '/');
    if (div_char != NULL) {     
        base_dir = strdup(dir_path);
        rv = create_dir(dirname(base_dir), mode);
        free(base_dir);
        base_dir = NULL;
        if (rv == -1)
            return -1;
    }

    // Create directory
    errno = 0;
    rv = mkdir(dir_path, mode);
    if (rv == -1) {
        fprintf(stderr, "Unable to create directory \"%s\": %s\n", 
                dir_path, strerror(errno));
        return -1;
    }

    return 0;
}

