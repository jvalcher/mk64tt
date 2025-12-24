#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>

#include "jv_test.h"
#include "utils.h"
#include "ghost_data.h"

#define MPK_DIR  "/../data/ghost/mpk"



static void ghost_data_test(void)
{
    char *cwd;
    char *mpk_dir_path;

    test_init();

    struct dirent *de = NULL;
    cwd = getcwd(NULL, 0);
    asprintf(&mpk_dir_path, "%s%s", cwd, MPK_DIR);
    DIR *mpk_dir = NULL;
    size_t mpk_file_count;

    //
    // mpk_dir/*.mpk count == mpk_count()
    //
    errno = 0;
    mpk_dir = opendir(mpk_dir_path);
    if (mpk_dir == NULL)
        perror("opendir mpk dir failed");

    test_cond(mpk_dir != NULL);

    mpk_file_count = 0;
    for (;;) {
        de = readdir(mpk_dir);
        if (de == NULL) 
            break;
        if (strcmp(de->d_name, ".") == 0 ||
            strcmp(de->d_name, "..") == 0)
            continue;
        ++mpk_file_count;
    }

    test_cond(mpk_file_count == mpk_count());

    closedir(mpk_dir);
    free(mpk_dir_path);
    free(cwd);

    test_results();
}

int main(void)
{
    init_log_stream(NULL);

    ghost_data_test();

    close_log_stream();

    return 0;
}
