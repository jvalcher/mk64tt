/*
    src/mupen64plus.c
*/
#ifdef _WIN32
#else
#define _GNU_SOURCE
#endif

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <SDL2/SDL.h>
#include <ctype.h>

#ifdef _WIN32
#else
#include <spawn.h>
#include <signal.h>
#include <wait.h>
#include <pty.h>
#endif

#include "utils.h"
#include "path.h"
#include "save_dirs.h"
#include "mup.h"

#define R         "\033[1;0m"  // reset to default
#define RED       "\033[1;31m"
#define CYAN      "\033[1;36m"
#define GREEN     "\033[1;32m"
#define YELLOW    "\033[1;33m"
#define PURPLE    "\033[1;35m"

#define MUP_OUTPUT_BUF_SIZE  2048
#define MUP_ERR_BUF_SIZE  256
#define ROM_INFO_BUF_SIZE  48
#define TRACK_CATEG_BUF_SIZE  12

typedef struct mup_cmd_arg {
    char *str;
    struct mup_cmd_arg *next;
} mup_cmd_list_arg_t;

typedef struct {
    char *bin
    mup_cmd_list_arg_t *arg_head;
    mup_cmd_list_arg_t *arg_tail;
    char *rom;
    int arg_count;
} mup_cmd_list_t;

typedef struct {
    pid_t pid;
    int pty_fd;

    mup_cmd_list_t cmd_list;
    char **cmd;
    char *config_path;
    char *save_path;
    int ghost_type;
    char track_categ[TRACK_CATEG_BUF_SIZE];
    int rom_index;
    int save_opt_index;

    char rom_goodname_buf[ROM_INFO_BUF_SIZE];
    char rom_md5_buf[ROM_INFO_BUF_SIZE];
    char rom_country_buf[ROM_INFO_BUF_SIZE];
    char rom_imagetype_buf[ROM_INFO_BUF_SIZE];
    char rom_mup_id_buf[ROM_INFO_BUF_SIZE];

    bool error_occurred;
    char err_buf[MUP_ERR_BUF_SIZE];
} mup_proc_t

static int get_screen_resolution(int *w, int *h);
static void destroy_mup_cmd_list(mup_proc_t *mp);
static mup_arg_t* mup_cmd_list_arg_create(mup_proc_t *mp, const char *str);
static bool mup_cmd_list_arg_is_set(mup_proc_t *mp, const char *str);
static void destroy_mup_cmd(mup_proc_t *mp);
static int mup_start_impl(mup_proc_t *mp);
static int get_rom_key_val(const char *buf, const char *key, char *ret_buf);
static int get_rom_info(mup_proc_t *mp);



int mup_add_arg(mup_proc_t *mp, const char *str)
{
    mup_cmd_list_arg_t *arg = NULL;

    if (strstr(str, CORE_SRAM_EQ) != NULL) {
        _logp("Setting \"%s\" is not allowed", CORE_SRAM_EQ);
        return -1;
    }

    arg = mup_cmd_list_arg_create(str);
    if (!arg)
        return -1;

    if (mp->cmd_list.arg_head == NULL) {
        mp->cmd_list.arg_head = arg;
        mp->cmd_list.tail = arg;
    } else {
        mp->cmd_list.tail->next = arg;
        mp->arg_list.tail = arg;
    }

    mp->cmd_list.arg_count += 1;

    return 0;
}

bool mup_running(mup_proc_t *mp)
{
    return mp->pid > 0;
}

int mup_stop(mup_proc_t *mp)
{
    if (mp->pid <= 0) {
        return 0;
    }

    _logp("Stopping mupen64plus process...");

    if (kill(-(mp->pid), SIGKILL)) {
        if (errno == ESRCH)
            return 0;
        _logpe("Mupen64plus process kill failed");   
        return -1;
    }

    if (mup_wait(mp))
        return -1;

    return 0;
}

int mup_get_rom_info(mup_proc_t *mp)
{
    if (mup_running(mp))
        if (mup_stop(mp)) 
            return -1;

    if (!mp->cmd_list.rom) {
        _logp("Unable to get ROM info: ROM path not set");
        return -1;
    }
    if (!mp->cmd_list.bin) {
        _logp("Unable to get ROM info: mupen64plus binary path not set");
        return -1;
    }

    if (mup_start_impl(mp))
        return -1;
    if (get_rom_info(mp))
        return -1;
    if (mup_stop(mp))
        return -1;

    return 0;
}

int mup_init(mup_proc_t **mp)
{
    int i, w, h;
    char *tmp = NULL;
    char *path = NULL;
    mup_cmd_list_arg_t *curr_arg;

    if (mp->initialized == true) {
        _logp("Mupen64plus process already initialized");
        return 0;
    }
    mp->initialized = false;
    mp->

    destroy_mup_cmd_list(mp);
    destroy_mup_cmd(mp);

    // Verify binary, ROM set
    if (!(mp->cmd_list.bin)) {
        _logp("Mupen64plus binary not set");
        return -1;
    }
    if (!(mp->cmd_list.rom)) {
        _logp("Mupen64plus ROM path not set");
        return -1;
    }

    // Create config directory
    asprintf(&mp->config_path, "%s%s", getenv("HOME"), MUP_CONFIG_DIR_PATH);
    if (mkdir_p(mp->config_path, MK64T_DIR_MODE))
        return -1;

    // Add, create config directory
    if (!mup_cmd_list_arg_is_set("--configdir")) {
        mup_arg_add("--configdir");
        asprintf(&tmp, "%s%s", getenv("HOME"), MUP_CONFIG_DIR_PATH);
        if (mkdir_p(tmp, MK64T_DIR_MODE))
            return -1;
        mup_arg_add(tmp);
        free(tmp);
    }

    // Get ROM info
    mp->cmd = malloc(sizeof *(mp->cmd) * 3);
    if (!mp->cmd) {
        _logpe("Failed to create mupen64plus command array");
        return -1;
    }
    mp->cmd[0] = mp->cmd_list.bin;
    mp->cmd[1] = mp->cmd_list.rom;
    mp->cmd[2] = NULL;
    if (mup_get_rom_info(mp))
        return -1;
    free(mp->cmd);

    // Add SRAM save directory path
    mup_arg_add("--set");
    // Create mp->cmd[]
    mp->cmd = malloc(sizeof *(mp->cmd) * (mp->cmd_list.arg_count + 3));
    if (!mp->cmd) {
        _logpe("Failed to create mupen64plus command array");
        return -1;
    }
    i = 0;
    mp->cmd[i++] = mp->cmd_list.bin;
    for (curr_arg = mp->cmd_list.head; curr_arg; ++i) {
        if (strstr(curr_arg->str, CORE_SRAM_EQ))
            mp->save_opt_index = i;
        mp->cmd[i] = curr_arg->str;
        curr_arg = curr_arg->next;
    }
    mp->cmd[i] = mp->cmd_list.rom;
    mp->rom_index = i++;
    mp->cmd[i] = NULL;

    mp->initialized = true;

    return 0;
}

int mup_load_bin(mup_proc_t *mp, const char *path)
{
    if (strlen(path) == 0) {
        _logp("Error: empty mupen64plus binary path");
        return -1;
    }

    if (mp->cmd_list.bin) {
        free(mp->cmd_list.bin);
        mp->cmd_list.bin = NULL;
    }

    mp->cmd_list.bin = strdup(path);
    if (!mp->cmd_list.bin) {
        _logpe("Failed to create mupen64plus binary path string");
        return -1;
    }

    return 0;
}

int mup_load_rom(mup_proc_t *mp, const char *path)
{
    if (strlen(path) == 0) {
        _logp("Error: empty ROM path");
        return -1;
    }

    if (mp->cmd_list.rom) {
        free(mp->cmd_list.rom);
        mp->cmd_list.rom = NULL;
    }

    mp->cmd_list.rom = strdup(path);
    if (!mp->cmd_list.rom) {
        _logpe("Failed to create ROM path string");
        return -1;
    }

    if (mp->initialized) {
        if (mp->cmd[mp->rom_index])
            free(mp->cmd[mp->rom_index]);
        mp->cmd[mp->rom_index] = mp->cmd_list.rom;
    }

    _log("ROM loaded: %s", path);
    _logf("ROM loaded: " PURPLE "%s" R "\n", path);

    return 0;
}

int mup_set_race(mup_proc_t *mp, const char *track_categ, int ghost_type)
{
    char *tmp = NULL;

    if (mp->save_path)
        free(mp->save_path;

    switch(ghost_type) {
    case GHOST_DEF:
        __attribute__((fallthrough));
    case GHOST_USR:
        if (asprintf(&(mp->save_path), "%s%s%s%s%s", getenv("HOME"), MK64T_BASE_PATH, 
                     get_save_dir_path(track_categ, ghost_type), "/", mp->rom_mup_id_buf) == -1) {
            _logp("Failed to create SRAM path string");
            return -1;
        }
        break;
    case GHOST_REC:
        if (asprintf(&(mp->save_path), "%s%s%s%s", getenv("HOME"), MK64T_BASE_PATH, 
                     get_save_dir_path(track_categ, ghost_type)) == -1) {
            _logp("Failed to create SRAM path string");
            return -1;
        }
        break;
    }
    if (asprintf(%tmp, "%s%s", CORE_SRAM_EQ, mp->save_path) == -1) {
        _logp("Failed to create SRAM opt string");
        return -1;
    }

    if (ghost_type != GHOST_REC && mkdir_p(mp->save_path, MK64T_DIR_MODE))
        return -1;

    mp->ghost_type = ghost_type;
    strncpy(mp->track_categ, track_categ, TRACK_CATEG_BUF_SIZE);

    mup_arg_add(tmp);
    free(tmp);

    return 0;
}

int mup_wait(mup_proc_t *mp)
{
    int wstatus;
    pid_t wpid;

    do {
        wpid = waitpid(-(mp->pid), &wstatus, 0);
    } while (wpid == -1 && errno == EINTR);
    if (wpid == -1) {
        _logpe("Mupen64plus process waitpid failed");
        return -1;
    }

    if (WIFEXITED(wstatus))
        _logp("Mupen64plus process exited (%d)", WEXITSTATUS(wstatus));
    else if (WIFSIGNALED(wstatus))
        _logp("Mupen64plus process exited (SIG%s%s)", 
               sigabbrev_np(WTERMSIG(wstatus)), WCOREDUMP(wstatus) ? ", core dumped" : "");
    else
        _logp("Mupen64plus process exited with unknown status (%d)", wstatus);

    mp->pid = -1;

    if (mp->pty_fd >= 0) {
        fclose(mp->pty_fd);
        mp->pty_fd = -1;
    }

    return 0;
}

const char* mup_get_err_msg(mup_proc_t *mp)
{
    return mp->err_buf;
}

bool mup_err(mup_proc_t *mp)
{
    return mp->error_occurred;
}

int mup_start(mup_proc_t *mp)
{
    if (mup_running(mp))
        if (mup_stop(mp)) 
            return -1;
    if (mup_start_impl(mp))
        return -1;
    if (get_rom_info(mp)) {
        if (mup_err(mp))
            if (mup_stop(mp)) 
                return -1;
        return -1;
    }

    return 0;
}

const char* mup_rom_goodname(mup_proc_t *mp)
{
    return mp->rom_goodname_buf;
}

const char* mup_rom_md5(mup_pro_t *mp)
{
    return mp->rom_md5_buf;
}

const char* mup_rom_imagetype(mup_proc_t *mp)
{
    return mp->rom_imagetype_buf;
}

const char* mup_rom_country(mup_proc_t *mp)
{
    return mp->rom_country_buf;
}

const char* rom_mup_id(mup_proc_t *mp)
{
    return mp->rom_mup_id_buf;
}

int mup_cleanup(mup_proc_t *mp)
{
    _logp("Cleaning up mupen64plus process...");

    if (mup_stop(mp))
        return -1;

    mp->initialized = false;

    destroy_mup_cmd_list(mp);
    destroy_mup_cmd(mp);

    if (mp->config_path) {
        free(mp->config_path);
        mp->config_path = NULL;
    }
    if (mp->save_path) {
        free(mp->save_path);
        mp->save_path = NULL;
    }

    mp->rom_goodname_buf[0] = '\0';
    mp->rom_md5_buf[0] = '\0';
    mp->rom_country_buf[0] = '\0';
    mp->rom_imagetype_buf[0] = '\0';
    mp->rom_mup_id_buf[0] = '\0';

    mp->error_occurred = false;
    mp->err_buf[0] = '\0';

    return 0;
}



/*********
  Private
 *********/

static int get_screen_resolution(int *w, int *h)
{
    SDL_DisplayMode mode;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        _logp("SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    if (SDL_GetCurrentDisplayMode(0, &mode) != 0) {
        _logp("SDL_GetCurrentDisplayMode failed: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    *w = mode.w;
    *h = mode.h;

    SDL_Quit();

    return 0;
}

static void destroy_mup_cmd_list(mup_proc_t *mp)
{
    mup_cmd_list_arg_t *curr_arg;
    mup_cmd_list_arg_t *next_arg;

    if (mp->cmd_list.bin) {
        free(mp->cmd_list.bin);
        mp->cmd_list.bin = NULL;
    }
    if (mp->cmd_list.rom) {
        free(mp->cmd_list.rom);
        mp->cmd_list.rom = NULL;
    }

    curr_arg = mp->cmd_list.head;
    while (curr_arg) {
        next_arg = curr_arg->next;
        if (curr_arg->str)
            free(curr_arg->str);
        free(curr_arg);
        curr_arg = next_arg;
    }

    mp->cmd_list.arg_head = NULL;
    mp->cmd_list.arg_tail = NULL;
    mp->cmd_list.arg_count = 0;
}

static mup_arg_t* mup_cmd_list_arg_create(const char *str)
{
    if (!str) {
        _logp("Empty mup_cmd_list_arg_t string");
        return NULL;
    }

    mup_cmd_list_arg_t *arg = malloc(sizeof *arg);
    if (!arg) {
        _logpe("Failed to allocate new mup_cmd_list_arg_t");
        return NULL;
    }

    arg->str = strdup(str);
    if (!(arg->str)) {
        _logpe("Failed to create mup_cmd_list_arg_t string \"%s\"", str);
        free(arg);
        return NULL;
    }

    return arg;
}

static bool mup_cmd_list_arg_is_set(const char *str)
{
    mup_cmd_list_arg_t *curr = NULL;

    curr = mp->cmd_list.head;
    while (curr) {
        if (strstr(curr->str, str) != NULL)
            return true;
        curr = curr->next;
    }

    return false;
}

static void destroy_mup_cmd(mup_proc_t *mp)
{
    if (!mp->cmd)
        return;
    free(mp->cmd);
    mp->cmd = NULL;
}

static int mup_start_impl(void)
{
    int arg_tog;
    char arg_color[12];

    if (!mp->initialized) {
        _logp("Failed to start mupen64plus: not initialized");
        return -1;
    }

    _logp("Starting mupen64plus process...");

#ifdef DEBUG
    // Print command
    arg_tog = 0;
    for (int i = 0; mp->cmd[i] != NULL; ++i) {
        if (i == 0)
            _logpf(YELLOW "%s ", mp->cmd[i]);
        else if (mp->cmd[i+1] == NULL)
            _logpf(PURPLE "%s" R "\n", mp->cmd[i]);
        else if (strstr(mp->cmd[i], "--")) {
            if (arg_tog == 0) {
                _logpf(GREEN "%s ", mp->cmd[i]);
                arg_tog = 1;
            } else {
                _logpf(CYAN "%s ", mp->cmd[i]);
                arg_tog = 0;
            }
        } else {
            _logpf("%s ", mp->cmd[i]);
        }
    }
#endif

    // Log command
    _log("Command:\n-----------");
    for (int i = 0; mp->cmd[i] != NULL; ++i)
        _logf("%s ", mp->cmd[i]);
    _logf("\n-----------\n");


    mp->pid = forkpty(&mp->pty_fd, NULL, NULL, NULL);

    if (mp->pid == -1) {
        _logpe("Mupen64plus fork failed");
        return -1;
    } else if (mp->pid == 0) {
        execvp(mp->cmd[0], mp->cmd);
        perror("Mupen64plus execvp failed");
        _exit(127);
    } 

    return 0;
}

static int get_rom_key_val(const char *buf, const char *key, char *ret_buf)
{
    int i;
    char *ptr;
    char *rom_info_not_found = "not found";

    ptr = strstr(buf, key);
    if (ptr != NULL) {
        ptr += strlen(key) + strlen(": ");
        i = 0;
        while (*ptr != '\r' && *ptr != '\n' && *ptr != '\0' && i < ROM_INFO_BUF_SIZE-1)
            ret_buf[i++] = *ptr++;
        ret_buf[i] = '\0';
    } else {
        _logp("Failed to find ROM key \"%s\" in mupen64plus output", key);
        strcpy(ret_buf, rom_info_not_found);
        return -1;
    }

    return 0;
}

/*
    Get info from mupen64plus terminal output
*/
static int get_rom_info(mup_proc_t *mp)
{
    int i;
    ssize_t bytes = 0;
    int buf_offset;
    char buf[MUP_OUTPUT_BUF_SIZE];
    char warn_buf[MUP_ERR_BUF_SIZE];
    char *ptr;

    char *ui_error_key = "UI-Console Error";
    char *ui_warn_key = "UI-Console Warning";
    char *goodname_key = "Goodname";
    char *md5_key = "MD5";
    char *imagetype_key = "Imagetype";
    char *country_key = "Country";
    char *end_key = "UI-Console Status";

    buf_offset = 0;

    for (;;) {
        if (buf_offset >= (int)(sizeof(buf) - 1))
            break;

        bytes = read(mp->pty_fd, buf + buf_offset, sizeof(buf)-buf_offset-1);
        if (bytes == -1 && errno != EAGAIN) {
            _logpe("Failed to read mupen64plus file descriptor");
            return -1;
        }
        if (bytes == 0)
            break;

        buf[buf_offset + bytes] = '\0';
        buf_offset += bytes;

        ptr = strstr(buf, ui_error_key);
        if (ptr != NULL)
            break;
        ptr = strstr(buf, end_key);
        if (ptr != NULL)
            break;
    }

    mp->error_occurred = false;

    // UI-Console Error
    ptr = strstr(buf, ui_error_key);
    if (ptr != NULL) {
        i = 0;
        ptr += strlen(ui_error_key) + strlen(": ");
        while (*ptr != '\r' && *ptr != '\n' && *ptr != '\0' && i < MUP_ERR_BUF_SIZE-1)
            mp->err_buf[i++] = *ptr++;
        mp->err_buf[i] = '\0';
        mp->error_occurred = true;
        _logpf(GREEN "Mupen64plus" R ": " RED "%s" R "\n", mp->err_buf);
        _log("%s", mp->err_buf);
        return -1;
    }

    // UI-Console Warning
    ptr = strstr(buf, ui_warn_key);
    if (ptr != NULL) {
        i = 0;
        ptr += strlen(ui_warn_key) + strlen(": ");
        while (*ptr != '\r' && *ptr != '\n' && *ptr != '\0' && i < MUP_ERR_BUF_SIZE-1)
            warn_buf[i++] = *ptr++;
        warn_buf[i] = '\0';
        _logpf(GREEN "Mupen64plus" R ": " YELLOW "%s" R "\n", warn_buf);
        _log("%s", warn_buf);
    }

    get_rom_key_val(buf, goodname_key, mp->rom_goodname_buf);
    get_rom_key_val(buf, md5_key, mp->rom_md5_buf);
    get_rom_key_val(buf, imagetype_key, mp->rom_imagetype_buf);
    get_rom_key_val(buf, country_key, mp->rom_country_buf);

    sprintf(mp->rom_mup_id_buf, ROM_INFO_BUF_SIZE, "%s-%.*s", 
            mp->rom_goodname_buf, 8, mp->rom_md5_buf);

    return 0;
}

