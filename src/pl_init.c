#include "pl_init.h"
#include "pl_arena.h"
#include "pl_io.h"
#include "pl_internal.h"

#include <stdatomic.h>

static atomic_int g_pl_is_initialized = 0;

struct pl_arena *g_pl_global_arena = nullptr;

static pl_status init_stage_two(void)
{
    if (pl_arena_init() != PL_OK)
    {
        PL_ASSERT_ERRLOG("failed to initialized arena system");
        atomic_store(&g_pl_is_initialized, 0);
        return PL_FATAL;
    }

    g_pl_global_arena = pl_arena_create("pl_global_arena", PL_MiB(2));

    if (g_pl_global_arena == nullptr)
    {
        PL_ASSERT_ERRLOG("failed to create global arena");
        atomic_store(&g_pl_is_initialized, 0);
        return PL_FATAL;
    }

#if defined(DEBUG)
    pl_io_set_debug(true);
#elif defined(DNDEBUG)
    pl_io_set_debug(false);
#endif

    pl_io_set_prefix(PL_IO_LOG_LEVEL_LOG, "[plenum]: ", PL_IO_COLOR_GREEN);

    return PL_OK;
}

pl_status pl_init(void)
{
    if (atomic_load(&g_pl_is_initialized))
    {
        // already initialized
        return PL_ERROR;
    }
    atomic_store(&g_pl_is_initialized, 1);

    return init_stage_two();
}

void pl_quit(void)
{
    if (g_pl_global_arena)
    {
        pl_arena_destroy(g_pl_global_arena);
    }

    pl_arena_quit();
    atomic_store(&g_pl_is_initialized, 0);
}

bool pl_is_initialized(void)
{
    return atomic_load(&g_pl_is_initialized);
}
