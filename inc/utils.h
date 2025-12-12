#ifndef MK64TT_UTILS_H
#define MK64TT_UTILS_H



/*
    Create directory
    -------
    Functions like `mkdir -p`, creating parent directories as needed

    Parameters:
    - dir_path: absolute path
    - mode: directory permissions, e.g. 0755

    Returns:
    -  0: success
    - -1: failure
*/
int create_dir(const char *dir_path, int mode);



#endif
