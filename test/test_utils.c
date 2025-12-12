#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <libgen.h>
#include <assert.h>

#include "../inc/utils.h"
#include "_utils.h"

void create_dir_tests(void)
{
    int rv;
    int mode = 0775;

    TEST_MSG;

    char *dir1 = "test_dir";
    rv = create_dir(dir1, mode);
    assert(rv != -1);
    assert(opendir(dir1) != NULL);
    remove(dir1);
    assert(opendir(dir1) == NULL);

    char *dir2 = "test_dir1/test_dir2";
    create_dir(dir2, mode);
    char *base_dir2 = strdup(dir2);
    base_dir2 = dirname(base_dir2);
    assert(opendir(dir2) != NULL);
    remove(dir2);
    remove(base_dir2);
    assert(opendir(dir2) == NULL);
    assert(opendir(base_dir2) == NULL);
    free(base_dir2);
    base_dir2 = NULL;

    PASSED;
}

int main(void)
{
    create_dir_tests();

    return 0;
}

