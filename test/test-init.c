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

    pl_printf("This is raw printf style %d\n", 101);
    pl_log("This is log style %s", "logger");
    pl_warn("This is warn style %d", -1);
    pl_err("This is err style");

    PL_ASSERT_ERRLOG("Error in test");

    pl_quit();

    return result;
}
