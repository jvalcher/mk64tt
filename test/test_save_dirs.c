#include "jv_test.h"
#include "misc_test_utils.h"
#include "utils.h"
#include <sys/stat.h>
#include <categ.h>
#include "save_dirs.h"



void save_dirs_test(void)
{
    int rv;
    size_t i;
    char *path;

    test_init();

    // Category count
    test_cond(categ_cnt() > 0);

    // Create dirs
    rv = create_save_dirs();
    test_cond(rv == 0, "%d != 0", rv);
    test_cond(dir_exists(get_save_dir_path(NULL, CATEG_DEF)));
    for (i = 0; i < categ_cnt(); ++i) {
        test_cond(dir_exists(get_save_dir_path(categ_name[i], CATEG_USR)));
        test_cond(dir_exists(get_save_dir_path(categ_name[i], CATEG_REC)));
    }

    // Unknown type
    path = get_save_dir_path("BB_3lap", 3);
    test_cond(path == NULL);

    // Default with non-used categ
    path = get_save_dir_path("BB_3lap", CATEG_DEF);
    test_cond(strstr(path, "BB_3lap") == NULL);

    save_dirs_clean();

    test_results();
}

int main(void)
{
    putchar('\n');
    init_log_stream(NULL);

    save_dirs_test();

    close_log_stream();

    return 0;
}
