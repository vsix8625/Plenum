#include "pl_fs.h"
#include "pl_io.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>

pl_status pl_fs_mkdir_p(const char *path)
{
    if (path == nullptr)
    {
        return PL_ERROR;
    }

    char tmp[PL_PATH_MAX];
    u64  len = (u64) strlen(path);

    if (len == 0 || len >= sizeof(tmp))
    {
        return PL_ERROR;
    }

    memcpy(tmp, path, len);
    tmp[len] = '\0';

    char *p = tmp;

    if (tmp[0] == '/' || tmp[0] == '\\')
    {
        p = tmp + 1;
    }

    for (; *p; p++)
    {
        if (*p == '/' || *p == '\\')
        {
            char bkup = *p;

            *p = '\0';

            if (strlen(tmp) > 0)
            {
                if (mkdir(tmp, 0755) != 0 && errno != EEXIST)
                {
                    struct stat st;
                    if (stat(tmp, &st) != 0 || !S_ISDIR(st.st_mode))
                    {
                        PL_ASSERT_ERRLOG("mkdir failed for: %s", tmp);
                        return PL_ERROR;
                    }
                }
            }

            *p = bkup;
        }
    }

    if (mkdir(tmp, 0755) != 0 && errno != EEXIST)
    {
        struct stat st;
        if (stat(tmp, &st) != 0 || !S_ISDIR(st.st_mode))
        {
            PL_ASSERT_ERRLOG("mkdir failed for: %s", tmp);
            return PL_ERROR;
        }
    }

    return PL_OK;
}

bool pl_fs_mv(const char *src, const char *dest)
{
    if (src == nullptr || dest == nullptr)
    {
        return false;
    }

    return rename(src, dest) == 0;
}

bool pl_fs_link(const char *src, const char *dest, bool replace)
{
    if (src == nullptr || dest == nullptr)
    {
        return false;
    }

    if (replace)
    {
        unlink(dest);
    }

    return link(src, dest) == 0;
}

bool pl_fs_cp(const char *src, const char *dest)
{
    if (src == nullptr || dest == nullptr)
    {
        return false;
    }

    i32 src_fd = open(src, O_RDONLY);

    if (src_fd < 0)
    {
        return false;
    }

    struct stat st;
    if (fstat(src_fd, &st) < 0)
    {
        close(src_fd);
        return false;
    }

    i32 dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode);

    if (dest_fd < 0)
    {
        close(src_fd);
        return false;
    }

    off_t   offset = 0;
    ssize_t sent   = sendfile(dest_fd, src_fd, &offset, st.st_size);
    bool    result = (sent == st.st_size);

    close(src_fd);
    close(dest_fd);

    return result;
}

void pl_fs_which(const char *name, char *out_path, u64 out_size)
{
    if (name == nullptr || out_path == nullptr || out_size == 0)
    {
        return;
    }

    if (strchr(name, '/') != nullptr)
    {
        if (access(name, X_OK) == 0)
        {
            snprintf(out_path, out_size, "%s", name);
            return;
        }

        // error
        return;
    }

    const char *env_path = getenv("PATH");
    if (env_path == nullptr)
    {
        return;
    }

    char *path_copy = pl_strdup(env_path);

    char *dir = strtok(path_copy, ":");

    while (dir != nullptr)
    {
        char tmp[PL_PATH_MAX];
        snprintf(tmp, sizeof(tmp), "%s/%s", dir, name);

        if (access(tmp, X_OK) == 0)
        {
            snprintf(out_path, out_size, "%s", tmp);

            pl_free(path_copy);

            return;
        }

        dir = strtok(nullptr, ":");
    }

    pl_free(path_copy);
}
