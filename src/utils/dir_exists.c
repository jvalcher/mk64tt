#include <stdbool.h>
#include <sys/stat.h>


bool dir_exists(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}
