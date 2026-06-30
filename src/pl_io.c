#include "pl_io.h"
#include "pl_init.h"
#include "pl_string.h"
#include "pl_util.h"

#include <stdatomic.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

static char g_pl_io_color_string_pool[5][PL_BUF_SIZE_128];

static const char *g_pl_io_ansi_color_codes[] = {
    [PL_IO_COLOR_ZERO]    = "",
    [PL_IO_COLOR_GREEN]   = "38;5;40",
    [PL_IO_COLOR_ORANGE]  = "38;5;202",
    [PL_IO_COLOR_RED]     = "38;5;160",
    [PL_IO_COLOR_CORAL]   = "38;5;167",
    [PL_IO_COLOR_BLUE]    = "38;5;75",
    [PL_IO_COLOR_MAGENTA] = "38;5;13",
};

static enum pl_io_log_mode g_pl_io_log_mode = PL_IO_LOG_ALL;

static pl_sv g_io_plain_prefixes[5] = {
    [PL_IO_LOG_LEVEL_RAW]  = PL_SV(""),
    [PL_IO_LOG_LEVEL_LOG]  = PL_SV("[log]: "),
    [PL_IO_LOG_LEVEL_WARN] = PL_SV("[warning]: "),
    [PL_IO_LOG_LEVEL_ERR]  = PL_SV("[error]: "),
    [PL_IO_LOG_LEVEL_DBG]  = PL_SV("[debug]: "),
};

static pl_sv g_io_color_prefixes[5] = {
    [PL_IO_LOG_LEVEL_RAW]  = PL_SV(""),
    [PL_IO_LOG_LEVEL_LOG]  = PL_SV("\033[2k\r\033[38;5;40m[log]: \033[0m"),
    [PL_IO_LOG_LEVEL_WARN] = PL_SV("\033[2K\r\033[38;5;202m[warning]: \033[0m"),
    [PL_IO_LOG_LEVEL_ERR]  = PL_SV("\033[2K\r\033[38;5;160m[error]: \033[0m"),
    [PL_IO_LOG_LEVEL_DBG]  = PL_SV("\033[2K\r\033[38;5;167m[debug]: \033[0m"),
};

static atomic_flag g_io_log_atomic_lock = ATOMIC_FLAG_INIT;
// static atomic_flag g_io_write_atomic_lock = ATOMIC_FLAG_INIT;

static i32 g_io_stdout_tty = -1;
static i32 g_io_stderr_tty = -1;

static void io_log_core(enum pl_io_log_level level, const char *fmt, va_list args)
    __attribute__((format(__printf__, 2, 0)));

static inline bool io_istty(i32 fd)
{
    if (fd == STDOUT_FILENO)
    {
        if (g_io_stdout_tty == -1)
        {
            g_io_stdout_tty = isatty(fd);
            return g_io_stdout_tty;
        }
    }

    if (fd == STDERR_FILENO)
    {
        if (g_io_stderr_tty == -1)
        {
            g_io_stderr_tty = isatty(fd);
            return g_io_stderr_tty;
        }
    }

    return isatty(fd);
}

// core

static void io_log_core(enum pl_io_log_level level, const char *fmt, va_list args)
{
    if (!pl_is_initialized() || fmt == nullptr)
    {
        return;
    }

    i32 fd = STDOUT_FILENO;
    if (level == PL_IO_LOG_LEVEL_WARN || level == PL_IO_LOG_LEVEL_ERR)
    {
        fd = STDERR_FILENO;
    }

    if (g_pl_io_log_mode == PL_IO_LOG_QUIET)
    {
        if (level == PL_IO_LOG_LEVEL_LOG || level == PL_IO_LOG_LEVEL_DBG)
        {
            // log nothing if quiet mode
            return;
        }
    }

    bool use_color = (io_istty(fd) != 0);

    pl_sv prefix = use_color ? g_io_color_prefixes[level] : g_io_plain_prefixes[level];

    // setup complete

    char buf[PL_PATH_MAX];
    memcpy(buf, prefix.data, prefix.len);

    va_list aq;
    va_copy(aq, args);
    i32 msg_len = vsnprintf(buf + prefix.len, sizeof(buf) - prefix.len - 1, fmt, aq);
    va_end(aq);

    u64 total_len = prefix.len + (u64) msg_len;

    if (total_len < sizeof(buf) - 1)
    {
        if (level != PL_IO_LOG_LEVEL_RAW)
        {
            buf[total_len++] = '\n';
        }

        if (write(fd, buf, total_len) < 0)
        {
            char err_buf[PL_BUF_SIZE_256];

            // _DEFAULT_SOURCE
            if (strerror_r(errno, err_buf, sizeof(err_buf)) != 0)
            {
                snprintf(err_buf, sizeof(err_buf), "Unknown error: %d", errno);
            }

            fprintf(stderr, "%s: write failed: %s", __func__, err_buf);
            return;
        }
    }
    else
    {
        while (atomic_flag_test_and_set(&g_io_log_atomic_lock))
        {
            pl_util_yield();
        }

        char *big = pl_malloc(total_len + 2);

        if (big == nullptr)
        {
            return;
        }

        memcpy(big, prefix.data, prefix.len);
        va_list aq_heap;
        va_copy(aq_heap, args);
        vsnprintf(big + prefix.len, (size_t) msg_len + 1, fmt, aq_heap);
        va_end(aq_heap);

        if (level != PL_IO_LOG_LEVEL_RAW)
        {
            big[total_len] = '\n';
            total_len++;
        }

        if (write(fd, big, total_len) < 0)
        {
            char err_buf[PL_BUF_SIZE_256];

            // _DEFAULT_SOURCE
            if (strerror_r(errno, err_buf, sizeof(err_buf)) != 0)
            {
                snprintf(err_buf, sizeof(err_buf), "Unknown error: %d", errno);
            }

            fprintf(stderr, "%s: write failed: %s", __func__, err_buf);
            pl_free(big);
            return;
        }
        pl_free(big);
        atomic_flag_clear(&g_io_log_atomic_lock);
    }
}

void pl_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    io_log_core(PL_IO_LOG_LEVEL_RAW, fmt, args);
    va_end(args);
}

void pl_log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    io_log_core(PL_IO_LOG_LEVEL_LOG, fmt, args);
    va_end(args);
}

void pl_warn(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    io_log_core(PL_IO_LOG_LEVEL_WARN, fmt, args);
    va_end(args);
}

void pl_err(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    io_log_core(PL_IO_LOG_LEVEL_ERR, fmt, args);
    va_end(args);
}

static bool g_pl_io_set_dbg = false;

void pl_io_set_debug(bool enabled)
{
    g_pl_io_set_dbg = enabled;
}

void pl_dbg(const char *fmt, ...)
{
    if (g_pl_io_set_dbg == false)
    {
        return;
    }

    va_list args;
    va_start(args, fmt);
    io_log_core(PL_IO_LOG_LEVEL_DBG, fmt, args);
    va_end(args);
}

void pl_io_set_prefix(enum pl_io_log_level level, const char *prefix, enum pl_io_term_color color)
{
    if (level < PL_IO_LOG_LEVEL_LOG || level > PL_IO_LOG_LEVEL_DBG || prefix == nullptr)
    {
        return;
    }

    g_io_plain_prefixes[level] = pl_sv_from_cstr(prefix);

    if (color != PL_IO_COLOR_ZERO && color < (sizeof(g_pl_io_ansi_color_codes) / sizeof(char *)))
    {
        char tmp_buf[PL_BUF_SIZE_128];
        i32  len = snprintf(tmp_buf,
                            sizeof(tmp_buf),
                            "\033[2K\r\033[%sm%s\033[0m",
                            g_pl_io_ansi_color_codes[color],
                            prefix);

        if (len > 0 && (u64) len < sizeof(tmp_buf))
        {
            memcpy(g_pl_io_color_string_pool[level], tmp_buf, (u64) len + 1);

            g_io_color_prefixes[level].data = g_pl_io_color_string_pool[level];
            g_io_color_prefixes[level].len  = (u64) len;
        }
        else
        {
            g_io_color_prefixes[level] = g_io_plain_prefixes[level];
        }
    }
    else
    {
        g_io_color_prefixes[level] = g_io_plain_prefixes[level];
    }
}
