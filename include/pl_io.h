#pragma once

#include "pl_defs.h"
#include "pl_string.h"
#include <errno.h>
#include <stdio.h>

enum pl_io_log_level : u8
{
    PL_IO_LOG_LEVEL_RAW = 0,  // printf
    PL_IO_LOG_LEVEL_LOG,
    PL_IO_LOG_LEVEL_WARN,
    PL_IO_LOG_LEVEL_ERR,
    PL_IO_LOG_LEVEL_DBG,
};

enum pl_io_term_color : u8
{
    PL_IO_COLOR_ZERO = 0,
    PL_IO_COLOR_GREEN,
    PL_IO_COLOR_ORANGE,
    PL_IO_COLOR_RED,
    PL_IO_COLOR_CORAL,
    PL_IO_COLOR_BLUE,
    PL_IO_COLOR_MAGENTA,
};

enum pl_io_log_mode : u8
{
    PL_IO_LOG_ALL   = 0,
    PL_IO_LOG_QUIET = 1,
};

#define VA_CHECK(fmt_arg_n) __attribute__((format(__printf__, fmt_arg_n, fmt_arg_n + 1)))

void pl_printf(const char *fmt, ...) VA_CHECK(1);
void pl_log(const char *fmt, ...) VA_CHECK(1);
void pl_warn(const char *fmt, ...) VA_CHECK(1);
void pl_err(const char *fmt, ...) VA_CHECK(1);
void pl_dbg(const char *fmt, ...) VA_CHECK(1);

void pl_io_set_debug(bool enabled);
void pl_io_set_prefix(enum pl_io_log_level level, const char *prefix, enum pl_io_term_color color);

//-------------------------------------

#define PL_ASSERT_ERRLOG(msg, ...)                                                                 \
    do                                                                                             \
    {                                                                                              \
        char err_buf[PL_BUF_SIZE_256];                                                             \
        if (errno != 0)                                                                            \
        {                                                                                          \
            if (strerror_r(errno, err_buf, sizeof(err_buf)) != 0)                                  \
            {                                                                                      \
                snprintf(err_buf, sizeof(err_buf), "Unknown error %d", errno);                     \
            }                                                                                      \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            snprintf(err_buf, sizeof(err_buf), "n/a");                                             \
        }                                                                                          \
                                                                                                   \
        pl_err("In --> %s:%d(%s)\n          \u2514\u2500\u2500Sys Error [%s]: " msg,               \
               __FILE__,                                                                           \
               __LINE__,                                                                           \
               __func__,                                                                           \
               err_buf,                                                                            \
               ##__VA_ARGS__);                                                                     \
    } while (0)
