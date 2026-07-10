#include "pl_util.h"
#include <ctype.h>
#include <time.h>

char *pl_trimstr(char *s)
{
    if (s == nullptr)
    {
        return nullptr;
    }

    while (isspace((u8) *s))
    {
        s++;
    }

    if (*s == 0)
    {
        return s;
    }

    char *end = s + strlen(s) - 1;

    while (end > s && isspace((u8) *end))
    {
        *end = 0;
        end--;
    }

    return s;
}

void pl_util_yield(void)
{
    struct timespec ts = {.tv_sec = 0, .tv_nsec = 100 * 100};
    nanosleep(&ts, NULL);
}
