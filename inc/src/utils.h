/*
    Miscellaneous utilities
*/
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#ifdef _WIN32
#else
#define LOG_DEFAULT_PATH   "/.local/share/mk64t/log"
#endif



/*
    Log, print formatted error messages
    ----------
    - Requires setting LOG_DEFAULT_PATH, "/.../..."
    - Example usage:

        init_log_stream(NULL);
        if (LOG_STREAM_OPEN) {
            _log("error: %s", str);
            _loge("error: %s", str);
            _logp("error: %s", str);
            _logep("error: %s", str);
        }
        close_log_stream();
        print_see_log_msg();
*/
#define LOG_STREAM_OPEN  log_file_stream

extern FILE *log_file_stream;

//
// Open log file stream
// -----------
// - log_path: absolute path to log file, NULL to use $HOME+LOG_DEFAULT_PATH
// - Returns 0 on success, -1 on error
//
int init_log_stream(const char *log_path);

//
// Close log file stream
// --------
// - Returns 0 on success, -1 on error
//
int close_log_stream(void);

// Print log path information message
void print_see_log_msg(void);

// Formatted log (no info)
#define _logf(...) \
do { \
    if (log_file_stream) { \
        fprintf((log_file_stream), __VA_ARGS__); \
    } \
} while (0)

// Formatted print
#define _logpf(...) \
do { \
    fprintf(stderr, __VA_ARGS__); \
} while (0)

// Log
#define _log(...) \
do { \
    if (log_file_stream) { \
        fprintf((log_file_stream), "%s: %s: %d: ", __FILE__, __func__, __LINE__); \
        fprintf((log_file_stream), __VA_ARGS__); \
        fprintf((log_file_stream), "\n"); \
    } \
} while (0)

// Log errno
#define _loge(...) \
do { \
    _log(__VA_ARGS__); \
    if (log_file_stream) \
        fprintf((log_file_stream), "    errno: %s\n", strerror(errno)); \
} while (0)

// Log + print
#define _logp(...) \
do { \
    _log(__VA_ARGS__); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
} while (0)

// Log + print errno
#define _logpe(...) \
do { \
    _loge(__VA_ARGS__); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, ": %s\n", strerror(errno)); \
} while (0)



/*
   Create directory in the style of `mkdir -p`
   --------
   - Returns 0 on success, -1 on failure
*/
int mkdir_p(const char *path, mode_t mode);

/*
    Check if directory exists
*/
bool dir_exists(const char *path);



#endif
