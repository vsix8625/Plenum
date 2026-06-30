#include "pl_util.h"
#include <time.h>

void pl_util_yield(void)
{
    struct timespec ts = {.tv_sec = 0, .tv_nsec = 100 * 100};
    nanosleep(&ts, NULL);
}
