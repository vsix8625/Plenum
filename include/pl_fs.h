#pragma once

#include "pl_defs.h"
#include <sys/stat.h>
#include <unistd.h>

static inline bool pl_fs_isexec(const char *path)
{
    if (path == nullptr || path[0] == '\0')
    {
        return false;
    }

    return access(path, X_OK) == 0;
}

static inline bool pl_fs_exists(const char *path)
{
    if (path == nullptr || path[0] == '\0')
    {
        return false;
    }

    return access(path, F_OK) == 0;
}

static inline bool pl_fs_is_abspath(const char *path)
{
    return path[0] == '/';
}

/*
 * Thread local buffer
 */
static inline const char *pl_fs_getcwd(void)
{
    static thread_local char buf[PL_PATH_MAX];

    if (getcwd(buf, sizeof(buf)) == nullptr)
    {
        return nullptr;
    }

    return buf;
}

static inline u64 pl_fs_get_filesize(const char *path)
{
    if (path == nullptr)
    {
        return 0;
    }

    struct stat st;
    if (stat(path, &st) != 0)
    {
        return 0;
    }

    return (u64) st.st_size;
}

pl_status pl_fs_mkdir_p(const char *path);
bool      pl_fs_mv(const char *src, const char *dest);
bool      pl_fs_link(const char *src, const char *dest, bool replace);
bool      pl_fs_cp(const char *src, const char *dest);
void      pl_fs_which(const char *name, char *out_path, u64 out_size);
