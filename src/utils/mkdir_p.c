#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include <stdio.h>

int mkdir_p(const char *path, mode_t mode)
{
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;

    if (!path || !*path)
        return -1;

    len = strlen(path);
    if (len >= sizeof(tmp))
        return -1;

    memcpy(tmp, path, len + 1);

    if (tmp[len - 1] == '/')
        tmp[len - 1] = '\0';

    for (p = tmp + 1; *p; ++p) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, mode) == -1 && errno != EEXIST)
                return -1;
            *p = '/';
        }
    }

    if (mkdir(tmp, mode) == -1 && errno != EEXIST)
        return -1;

    return 0;
}

