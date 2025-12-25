/*
   mupen64plus subprocess
*/
#ifndef MUP_H
#define MUP_H

#include <stdbool.h>

#ifdef _WIN32
#else
#define MUP_CONFIG_DIR_PATH  "/.config/mk64t/mupen64plus/config"
#endif



/*
    Initialize mupen64plus process command
    -------
    - Required function calls before mup_init():
        - mup_load_rom: load ROM path
        - mup_load_bin: load mupen64plus binary path
        - mup_set_categ_type:
            - categ: track, category string (track_categ.h)
            - type: CATEG_DEF, CATEG_REC, CATEG_USR (save_dirs.h)
    - mup_cmd_add_arg: add individual mupen64plus flag, option, value, etc.
    - mup_init
        - Creates mk64 command
        - Gets ROM data
            - mup_load_rom can be called after without calling mup_init again
        - Parameters
            - categ: category string (categ.h), ignored for CATEG_DEF type (NULL)
            - type: CATEG_DEF, CATEG_REC, CATEG_USR (save_dirs.h)
    - Return 0 on sucess, -1 on failure
*/
int mup_load_bin(mup_proc_t *mp, const char *path);
int mup_load_rom(mup_proc_t *mp, const char *path);
int mup_set_categ_type(mup_proc_t *mp, const char *categ, int type)
int mup_add_arg(mup_proc_t *mp, const char *str);
    //
int mup_init(mup_proc_t **mp)

/*
    Start mupen64plus process
    ---------
    - Returns 0 on sucess, -1 on failure
*/
int mup_start(void);

/*
    Get ROM info
    ---------
    - Populates info variables for getters
    - Returns 0 on success, -1 on failure
*/
int mup_get_rom_info(void);
    //
const char* mup_rom_goodname(void);
const char* mup_rom_md5(void);
const char* mup_rom_country(void);
const char* mup_rom_imagetype(void);
const char* rom_mup_id(void);

/*
    Wait for mupen64plus to exit or be terminated by signal
    ---------
    - Returns 0 on success, -1 on failure
*/
int mup_wait(void);

/*
    Stop mupen64plus process
    ---------
    - Returns 0 on sucess, -1 on failure
*/
int mup_stop(void);

/*
    Check if mupen64plus running
*/
bool mup_running(void);

/*
    Get mupen64plus error message
    -------
    - Usage: 
        if (mup_err()) 
            err = get_mup_err_msg();
*/
bool mup_err(void);
const char* mup_get_err_msg(void);


/*
    Free all resources
*/
void mup_cleanup(void);



#endif
