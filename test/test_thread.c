#include "plenum.h"

void *worker_function(void *arg)
{
    PL_UNUSED(arg);
    for (i32 i = 0; i < 100; i++)
    {
        usleep(10000);
    }
    pl_log("[Worker] Done counting to 100");
    return nullptr;
}

i32 main(void)
{
    if (!pl_is_initialized())
    {
        return PL_EXIT_FAILURE;
    }

    pl_log("[Main] Creating thread...");
    struct pl_thread *my_thread = pl_thread_create(worker_function, nullptr);

    pl_log("[Main] Monitoring thread execution status...");
    if (pl_thread_is_running(my_thread))
    {
        pl_log("[Main] Thread is actively running, waiting for it to finish...");
    }
    while (pl_thread_is_running(my_thread))
    {
        usleep(20000);
    }

    pl_log("[Main] Destroying thread. Test complete!");
    pl_thread_destroy(my_thread);
    return PL_EXIT_SUCCESS;
}
