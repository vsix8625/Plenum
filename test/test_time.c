#include "plenum.h"
#include <unistd.h>

i32 main(void)
{
    if (pl_init() != PL_OK)
    {
        fprintf(stderr, "Failed to init\n");
        return PL_EXIT_FAILURE;
    }
    struct pl_ticks profile = {0};
    pl_ticks_start(&profile);
    pl_log("Plenum Initialized");

    u64 etime = pl_time_epoch_sec();
    pl_log("Epoch1: %lu", etime);

    sleep(1);

    u64 etime2 = pl_time_epoch_sec();
    pl_log("Epoch2: %lu", etime2);
    pl_log("Elapsed: %lu", etime2 - etime);

    pl_ticks_end(&profile);
    char time_buf[PL_BUF_SIZE_32];
    if (pl_time_ticks_format(&profile, time_buf, sizeof(time_buf)) != nullptr)
    {
        pl_log("Profile: %s", time_buf);
    }
    pl_quit();
    return PL_EXIT_SUCCESS;
}
