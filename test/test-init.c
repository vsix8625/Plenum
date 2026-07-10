#include "plenum.h"

i32 main(void)
{
    pl_log("test");

    if (pl_init() != PL_OK)
    {
        return PL_EXIT_FAILURE;
    }

    i32 result = PL_EXIT_SUCCESS;

    pl_io_set_prefix(PL_IO_LOG_LEVEL_LOG, "[test]: ", PL_IO_COLOR_BLUE);
    pl_io_set_debug(true);

    pl_dbg("Debug enabled");

    pl_log("Hello world from test-init.c");

    pl_dbg("Pagesize %lu", pl_sys_pagesize());
    pl_dbg("Stream MAX: %lu", pl_sys_stream_max());
    pl_dbg("POSIX version: %lu", pl_sys_posix_version());
    pl_dbg("POSIX2 version: %lu", pl_sys_posix2_version());
    pl_dbg("PHYS_PAGES: %lu | AVAIL: %lu", pl_sys_phys_pages(), pl_sys_avail_phys_pages());
    pl_warn("CPU cores: %lu | Cacheline: %lu", pl_cpu_nproc(), pl_cpu_cacheline());
    pl_log("RAM: %.1f/%.1f",
           pl_sys_used_phys_memory() / (1024.0 * 1024.0 * 1024.0),
           pl_sys_total_phys_memory() / (1024.0 * 1024.0 * 1024.0));

    PL_ASSERT_ERRLOG("Error in test");

    pl_quit();

    return result;
}
